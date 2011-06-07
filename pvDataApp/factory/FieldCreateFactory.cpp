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
#include <pv/lock.h>
#include <pv/pvIntrospect.h>
#include <pv/convert.h>
#include <pv/factory.h>
#include <pv/CDRMonitor.h>

using std::tr1::static_pointer_cast;

namespace epics { namespace pvData {

static DebugLevel debugLevel = lowDebug;

static void newLine(StringBuilder buffer, int indentLevel)
{
    *buffer += "\n";
    for(int i=0; i<indentLevel; i++) *buffer += "    ";
}

PVDATA_REFCOUNT_MONITOR_DEFINE(field);

Field::Field(String fieldName,Type type)
    :m_fieldName(fieldName)
    ,m_type(type)
{
    PVDATA_REFCOUNT_MONITOR_CONSTRUCT(field);
}

Field::~Field() {
    PVDATA_REFCOUNT_MONITOR_DESTRUCT(field);
    // note that compiler automatically calls destructor for fieldName
    if(debugLevel==highDebug) printf("~Field %s\n",m_fieldName.c_str());
}

void Field::renameField(String  newName)
{
    m_fieldName = newName;
}

void Field::toString(StringBuilder buffer,int indentLevel) const{
    *buffer += " ";
    *buffer += m_fieldName.c_str();
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
        String name = fields[i]->getFieldName();
        // look for duplicates
        for(int j=i+1; j<numberFields; j++) {
            String otherName = fields[j]->getFieldName();
            int result = name.compare(otherName);
            if(result==0) {
                String  message("duplicate fieldName ");
                message += name;
                delete[] fields;
                throw std::invalid_argument(message);
            }
        }
    }
}

Structure::~Structure() {
    if(debugLevel==highDebug)
        printf("~Structure %s\n",Field::getFieldName().c_str());

    delete[] fields;
}

FieldConstPtr  Structure::getField(String fieldName) const {
    for(int i=0; i<numberFields; i++) {
        FieldConstPtr pfield = fields[i];
        int result = fieldName.compare(pfield->getFieldName());
        if(result==0) return pfield;
    }
    return FieldConstPtr();
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
     ScalarConstPtr scalar(new Scalar(fieldName,scalarType), Field::Deleter());
     return scalar;
}
 
ScalarArrayConstPtr FieldCreate::createScalarArray(
    String fieldName,ScalarType elementType) const
{
      ScalarArrayConstPtr scalarArray(new ScalarArray(fieldName,elementType), Field::Deleter());
      return scalarArray;
}
StructureConstPtr FieldCreate::createStructure (
    String fieldName,int numberFields,
    FieldConstPtr fields[]) const
{
      StructureConstPtr structure(new Structure(
          fieldName,numberFields,fields), Field::Deleter());
      return structure;
}
StructureArrayConstPtr FieldCreate::createStructureArray(
    String fieldName,StructureConstPtr structure) const
{
     StructureArrayConstPtr structureArray(new StructureArray(fieldName,structure), Field::Deleter());
     return structureArray;
}

FieldConstPtr FieldCreate::create(String fieldName,
    FieldConstPtr pfield) const
{
    FieldConstPtr ret;
    Type type = pfield->getType();
    switch(type) {
    case scalar: {
        ScalarConstPtr pscalar = static_pointer_cast<const Scalar>(pfield);
        return createScalar(fieldName,pscalar->getScalarType());
    }
    case scalarArray: {
        ScalarArrayConstPtr pscalarArray = static_pointer_cast<const ScalarArray>(pfield);
        return createScalarArray(fieldName,pscalarArray->getElementType());
    }
    case structure: {
        StructureConstPtr pstructure = static_pointer_cast<const Structure>(pfield);
        return createStructure(fieldName,pstructure->getNumberFields(),pstructure->getFields());
    }
    case structureArray: {
        StructureArrayConstPtr pstructureArray = static_pointer_cast<const StructureArray>(pfield);
        return createStructureArray(fieldName,pstructureArray->getStructure());
    }
    }
    String  message("field ");
    message += fieldName;
    THROW_EXCEPTION2(std::logic_error, message);
}

static FieldCreate* fieldCreate = 0;

FieldCreate::FieldCreate()
{
}

FieldCreate * getFieldCreate() {
    static Mutex mutex;
    Lock xx(mutex);

    if(fieldCreate==0) fieldCreate = new FieldCreate();
    return fieldCreate;
}

}}
