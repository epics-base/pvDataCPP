/*FieldCreateFactory.cpp*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <lock.h>
#include "pvIntrospect.h"
#include "convert.h"
#include "factory.h"
#include "CDRMonitor.h"

namespace epics { namespace pvData {

static DebugLevel debugLevel = lowDebug;

static void newLine(StringBuilder buffer, int indentLevel)
{
    *buffer += "\n";
    for(int i=0; i<indentLevel; i++) *buffer += "    ";
}

PVDATA_REFCOUNT_MONITOR_DEFINE(field);

class FieldPvt {
public :
    FieldPvt(String fieldName,Type type);
    String fieldName;
    Type type;
    int referenceCount;
};

FieldPvt::FieldPvt(String fieldName,Type type)
 : fieldName(fieldName),type(type),referenceCount(1)
{PVDATA_REFCOUNT_MONITOR_INCREF(field);}

static Mutex refCountMutex;

Field::Field(String fieldName,Type type)
    : pImpl(new FieldPvt(fieldName,type))
{
    PVDATA_REFCOUNT_MONITOR_CONSTRUCT(field);
}

Field::~Field() {
    PVDATA_REFCOUNT_MONITOR_DESTRUCT(field);
    // note that compiler automatically calls destructor for fieldName
    if(debugLevel==highDebug) printf("~Field %s\n",pImpl->fieldName.c_str());
    delete pImpl;
    pImpl = 0;
}

int Field::getReferenceCount() const {
    Lock xx(&refCountMutex);
    return pImpl->referenceCount;
}

String Field::getFieldName() const {return pImpl->fieldName;}

Type Field::getType() const {return pImpl->type;}

void Field::renameField(String  newName)
{
    pImpl->fieldName = newName;
}

void Field::incReferenceCount() const {
    PVDATA_REFCOUNT_MONITOR_INCREF(field);
    Lock xx(&refCountMutex);
    pImpl->referenceCount++;
    if(pImpl->type!=structure) return;
    StructureConstPtr structure = static_cast<StructureConstPtr>(this);
    FieldConstPtrArray fields = structure->getFields();
    int numberFields = structure->getNumberFields();
    for(int i=0; i<numberFields; i++) {
        fields[i]->incReferenceCount();
    }
}

void Field::decReferenceCount() const {
    PVDATA_REFCOUNT_MONITOR_DECREF(field);
    Lock xx(&refCountMutex);
    if(pImpl->referenceCount<=0) {
          String message("logicError field ");
          message += pImpl->fieldName;
          throw std::logic_error(message);
    }
    pImpl->referenceCount--;
    if(pImpl->type!=structure) {
        if(pImpl->referenceCount==0) {
             delete this;
        }
        return;
    }
    StructureConstPtr structure = static_cast<StructureConstPtr>(this);
    FieldConstPtrArray fields = structure->getFields();
    int numberFields = structure->getNumberFields();
    for(int i=0; i<numberFields; i++) {
        fields[i]->decReferenceCount();
    }
    if(pImpl->referenceCount==0) {
        delete this;
    }
}

void Field:: dumpReferenceCount(StringBuilder buffer,int indentLevel) const {
    *buffer += getFieldName();
    char buf[40];
    sprintf(buf," referenceCount %d",getReferenceCount());
    *buffer += buf;
    if(pImpl->type!=structure) return;
    Convert *convert = getConvert();
    StructureConstPtr structure = static_cast<StructureConstPtr>(this);
    FieldConstPtrArray fields = structure->getFields();
    int numberFields = structure->getNumberFields();
    for(int i=0; i<numberFields; i++) {
        convert->newLine(buffer,indentLevel+1);
        fields[i]->dumpReferenceCount(buffer,indentLevel +1);
    }
}
 
void Field::toString(StringBuilder buffer,int indentLevel) const{
    *buffer += " ";
    *buffer += pImpl->fieldName.c_str();
}

static bool fieldEquals(FieldConstPtr a,FieldConstPtr b);
inline bool scalarFieldEquals(ScalarConstPtr a,ScalarConstPtr b)
{
    ScalarType ascalarType = a->getScalarType();
    ScalarType bscalarType = b->getScalarType();
    if(ascalarType != bscalarType)
    {
    	return false;
    }
    return true;
}

inline bool scalarArrayFieldEquals(ScalarArrayConstPtr a,ScalarArrayConstPtr b)
{
	ScalarType aType = a->getElementType();
	ScalarType bType = b->getElementType();
	if(aType != bType)
	{
		return false;
	}
	return true;
}

inline bool structureFieldEquals(StructureConstPtr a,StructureConstPtr b)
{
    int length = a->getNumberFields();
    if(length != b->getNumberFields()) return false;
    FieldConstPtrArray aFields = a->getFields();
    FieldConstPtrArray bFields = b->getFields();
    for(int i=0; i<length; i++)
    {
        if(!fieldEquals(aFields[i],bFields[i])) return false;
    }
    return true;
}

inline bool structureArrayFieldEquals(StructureArrayConstPtr a,StructureArrayConstPtr b)
{
    StructureConstPtr aStruct = a->getStructure();
    StructureConstPtr bStruct = b->getStructure();
    return fieldEquals(aStruct,bStruct);
}

static bool fieldEquals(FieldConstPtr a,FieldConstPtr b)
{
    const void * avoid = static_cast<const void *>(a);
    const void * bvoid = static_cast<const void *>(b);
    if(avoid == bvoid) return true;
    if(a->getFieldName() != b->getFieldName()) return false;
    Type atype = a->getType();
    Type btype = b->getType();
    if(atype!=btype) return false;
    if(atype==scalar) return scalarFieldEquals(
        static_cast<ScalarConstPtr>(a),static_cast<ScalarConstPtr>(b));
    if(atype==scalarArray) return scalarArrayFieldEquals(
        static_cast<ScalarArrayConstPtr>(a),static_cast<ScalarArrayConstPtr>(b));
    if(atype==structureArray) return structureArrayFieldEquals(
        static_cast<StructureArrayConstPtr>(a),static_cast<StructureArrayConstPtr>(b));
    if(atype==structure) return structureFieldEquals(
        static_cast<StructureConstPtr>(a),static_cast<StructureConstPtr>(b));
    String message("should not get here");
    throw std::logic_error(message);
}

bool Field::operator==(const Field& field) const
{
    return fieldEquals(this, &field);
}

bool Field::operator!=(const Field& field) const
{
    return !fieldEquals(this, &field);
}

Scalar::Scalar(String fieldName,ScalarType scalarType)
       : Field(fieldName,scalar),scalarType(scalarType){}

Scalar::~Scalar(){}

void Scalar::toString(StringBuilder buffer,int indentLevel) const{
    ScalarTypeFunc::toString(buffer,scalarType);
    Field::toString(buffer,indentLevel);
}


ScalarArray::ScalarArray(String fieldName,ScalarType elementType)
: Field(fieldName,scalarArray),elementType(elementType){}

ScalarArray::~ScalarArray() {}

void ScalarArray::toString(StringBuilder buffer,int indentLevel) const{
    ScalarTypeFunc::toString(buffer,elementType);
    *buffer += "[]";
    Field::toString(buffer,indentLevel);
}

StructureArray::StructureArray(String fieldName,StructureConstPtr structure)
: Field(fieldName,structureArray),pstructure(structure)
{
}

StructureArray::~StructureArray() {
    if(debugLevel==highDebug) printf("~StructureArray\n");
    pstructure->decReferenceCount();
}

void StructureArray::toString(StringBuilder buffer,int indentLevel) const {
    *buffer +=  "structure[]";
    Field::toString(buffer,indentLevel);
    newLine(buffer,indentLevel + 1);
    pstructure->toString(buffer,indentLevel + 1);
}


Structure::Structure (String fieldName,
    int numberFields, FieldConstPtrArray infields)
: Field(fieldName,structure),
      numberFields(numberFields),
      fields(infields)
{
    for(int i=0; i<numberFields; i++) {
        fields[i] = infields[i];
    }
    for(int i=0; i<numberFields; i++) {
        String name = fields[i]->getFieldName();
        // look for duplicates
        for(int j=i+1; j<numberFields; j++) {
            String otherName = fields[j]->getFieldName();
            int result = name.compare(otherName);
            if(result==0) {
                String  message("duplicate fieldName ");
                message += name;
                throw std::invalid_argument(message);
            }
        }
    }
}

Structure::~Structure() {
    if(debugLevel==highDebug)
        printf("~Structure %s\n",Field::getFieldName().c_str());
    for(int i=0; i<numberFields; i++) {
        fields[i] = 0;
    }
    delete[] fields;
}

FieldConstPtr  Structure::getField(String fieldName) const {
    for(int i=0; i<numberFields; i++) {
        FieldConstPtr pfield = fields[i];
        int result = fieldName.compare(pfield->getFieldName());
        if(result==0) return pfield;
    }
    return 0;
}

int Structure::getFieldIndex(String fieldName) const {
    for(int i=0; i<numberFields; i++) {
        FieldConstPtr pfield = fields[i];
        int result = fieldName.compare(pfield->getFieldName());
        if(result==0) return i;
    }
    return -1;
}

void Structure::appendField(FieldConstPtr field)
{
    FieldConstPtr *newFields = new FieldConstPtr[numberFields+1];
    for(int i=0; i<numberFields; i++) newFields[i] = fields[i];
    newFields[numberFields] = field;
    delete[] fields;
    fields = newFields;
    numberFields++;
}

void Structure::appendFields(int numberNew,FieldConstPtrArray nfields)
{
    FieldConstPtr *newFields = new FieldConstPtr[numberFields+numberNew];
    for(int i=0; i<numberFields; i++) newFields[i] = fields[i];
    for(int i=0; i<numberNew; i++) newFields[numberFields+i] = nfields[i];
    delete[] fields;
    fields = newFields;
    numberFields += numberNew;
}

void Structure::removeField(int index)
{
    if(index<0 || index>=numberFields) {
        throw std::invalid_argument(
           String("Structure::removeField index out of bounds"));
    }
    FieldConstPtr *newFields = new FieldConstPtr[numberFields-1];
    fields[index]->decReferenceCount();
    int ind=0;
    for(int i=0; i<numberFields; i++) {
        if(i==index) continue;
        newFields[ind++] = fields[i];
    }
    delete[] fields;
    fields = newFields;
    --numberFields;
}

void Structure::toString(StringBuilder buffer,int indentLevel) const{
    *buffer += "structure";
    Field::toString(buffer,indentLevel);
    newLine(buffer,indentLevel+1);
    for(int i=0; i<numberFields; i++) {
        FieldConstPtr pfield = fields[i];
        pfield->toString(buffer,indentLevel+1);
        if(i<numberFields-1) newLine(buffer,indentLevel+1);
    }
}


ScalarConstPtr  FieldCreate::createScalar(String fieldName,
    ScalarType scalarType) const
{
     Scalar *scalar = new Scalar(fieldName,scalarType);
     return scalar;
}
 
ScalarArrayConstPtr FieldCreate::createScalarArray(
    String fieldName,ScalarType elementType) const
{
      ScalarArray *scalarArray = new ScalarArray(fieldName,elementType);
      return scalarArray;
}
StructureConstPtr FieldCreate::createStructure (
    String fieldName,int numberFields,
    FieldConstPtr fields[]) const
{
      Structure *structure = new Structure(
          fieldName,numberFields,fields);
      return structure;
}
StructureArrayConstPtr FieldCreate::createStructureArray(
    String fieldName,StructureConstPtr structure) const
{
     StructureArray *structureArray = new StructureArray(fieldName,structure);
     return structureArray;
}

FieldConstPtr FieldCreate::create(String fieldName,
    FieldConstPtr pfield) const
{
    Type type = pfield->getType();
    switch(type) {
    case scalar: {
        ScalarConstPtr pscalar = static_cast<ScalarConstPtr>(pfield);
        return createScalar(fieldName,pscalar->getScalarType());
    }
    case scalarArray: {
        ScalarArrayConstPtr pscalarArray = static_cast<ScalarArrayConstPtr>(pfield);
        return createScalarArray(fieldName,pscalarArray->getElementType());
    }
    case structure: {
        StructureConstPtr pstructure = static_cast<StructureConstPtr>(pfield);
        return createStructure(fieldName,pstructure->getNumberFields(),pstructure->getFields());
    }
    case structureArray: {
        StructureArrayConstPtr pstructureArray = static_cast<StructureArrayConstPtr>(pfield);
        return createStructureArray(fieldName,pstructureArray->getStructure());
    }
    }
    String  message("field ");
    message += fieldName;
    throw std::logic_error(message);
}

static FieldCreate* fieldCreate = 0;

FieldCreate::FieldCreate()
{
}

FieldCreate * getFieldCreate() {
    static Mutex mutex;
    Lock xx(&mutex);

    if(fieldCreate==0) fieldCreate = new FieldCreate();
    return fieldCreate;
}

}}
