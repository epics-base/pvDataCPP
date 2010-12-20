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
#include "factory.h"

namespace epics { namespace pvData {

static DebugLevel debugLevel = lowDebug;

static void newLine(StringBuilder buffer, int indentLevel)
{
    *buffer += "\n";
    for(int i=0; i<indentLevel; i++) *buffer += "    ";
}

class FieldPvt {
public :
    FieldPvt(String fieldName,Type type);
    String fieldName;
    Type type;
    mutable volatile int referenceCount;
};

FieldPvt::FieldPvt(String fieldName,Type type)
 : fieldName(fieldName),type(type),referenceCount(0) { }

static volatile int64 totalReferenceCount = 0;
static volatile int64 totalConstruct = 0;
static volatile int64 totalDestruct = 0;
static Mutex *globalMutex = 0;

static int64 getTotalConstruct()
{
    Lock xx(globalMutex);
    return totalConstruct;
}

static int64 getTotalDestruct()
{
    Lock xx(globalMutex);
    return totalDestruct;
}

static int64 getTotalReferenceCount()
{
    return totalReferenceCount;
}

static ConstructDestructCallback *pConstructDestructCallback;

static void init()
{
     static Mutex mutex = Mutex();
     Lock xx(&mutex);
     if(globalMutex==0) {
        globalMutex = new Mutex();
        pConstructDestructCallback = new ConstructDestructCallback(
            String("field"),
            getTotalConstruct,getTotalDestruct,getTotalReferenceCount);
     }
}


Field::Field(String fieldName,Type type)
    : pImpl(new FieldPvt(fieldName,type))
{
    Lock xx(globalMutex);
    totalConstruct++;
}

Field::~Field() {
    Lock xx(globalMutex);
    totalDestruct++;
    // note that compiler automatically calls destructor for fieldName
    delete pImpl;
    if(debugLevel==highDebug) printf("~Field %s\n",pImpl->fieldName.c_str());
}

int Field::getReferenceCount() const {
    Lock xx(globalMutex);
    return pImpl->referenceCount;
}

String Field::getFieldName() const {return pImpl->fieldName;}

Type Field::getType() const {return pImpl->type;}

void Field::incReferenceCount() const {
    Lock xx(globalMutex);
    pImpl->referenceCount++;
    totalReferenceCount++;
}

void Field::decReferenceCount() const {
    Lock xx(globalMutex);
     if(pImpl->referenceCount<=0) {
          String message("logicError field ");
          message += pImpl->fieldName;
          throw std::logic_error(message);
     }
     pImpl->referenceCount--;
    totalReferenceCount--;
     if(pImpl->referenceCount==0) delete this;
}

 
void Field::toString(StringBuilder buffer,int indentLevel) const{
    *buffer += " ";
    *buffer += pImpl->fieldName.c_str();
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
    pstructure->incReferenceCount();
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
      fields(new FieldConstPtr[numberFields])
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
        // inc reference counter
        fields[i]->incReferenceCount();
    }
}

Structure::~Structure() {
    if(debugLevel==highDebug)
        printf("~Structure %s\n",Field::getFieldName().c_str());
    for(int i=0; i<numberFields; i++) {
        FieldConstPtr pfield = fields[i];
        pfield->decReferenceCount();
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
     init();
}

FieldCreate * getFieldCreate() {
    static Mutex mutex = Mutex();
    Lock xx(&mutex);

    if(fieldCreate==0) fieldCreate = new FieldCreate();
    return fieldCreate;
}

}}
