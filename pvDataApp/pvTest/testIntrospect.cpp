/* testIntrospect.cpp */
/* Author:  Marty Kraimer Date: 2010.09 */

#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>

#include <epicsAssert.h>

#include "requester.h"
#include "pvIntrospect.h"
#include "pvData.h"
#include "standardField.h"

using namespace epics::pvData;

static FieldCreate * fieldCreate = 0;
static PVDataCreate * pvDataCreate = 0;
static StandardField *standardField = 0;
static String builder("");

static void testScalarCommon(FILE * fd,String fieldName,ScalarType stype,
    bool isInteger,bool isNumeric,bool isPrimitive)
{
    ScalarConstPtr pscalar = standardField->scalar(fieldName,stype);
    Type type = pscalar->getType();
    assert(type==scalar);
    builder.clear();
    TypeFunc::toString(&builder,type);
    assert(builder.compare("scalar")==0);
    ScalarType scalarType = pscalar->getScalarType();
    assert(scalarType==stype);
    assert(ScalarTypeFunc::isInteger(scalarType)==isInteger);
    assert(ScalarTypeFunc::isNumeric(scalarType)==isNumeric);
    assert(ScalarTypeFunc::isPrimitive(scalarType)==isPrimitive);
    builder.clear();
    pscalar->toString(&builder);
    fprintf(fd,"%s\n",builder.c_str());
    // create tempory PVField so that memory can be released
    PVField *pvField = pvDataCreate->createPVField(0,pscalar);
    delete pvField;
}

static void testScalar(FILE * fd) {
    fprintf(fd,"\ntestScalar\n");
    testScalarCommon(fd,String("boolean"),pvBoolean,false,false,true);
    testScalarCommon(fd,String("byte"),pvByte,true,true,true);
    testScalarCommon(fd,String("short"),pvShort,true,true,true);
    testScalarCommon(fd,String("int"),pvInt,true,true,true);
    testScalarCommon(fd,String("long"),pvLong,true,true,true);
    testScalarCommon(fd,String("float"),pvFloat,false,true,true);
    testScalarCommon(fd,String("double"),pvDouble,false,true,true);
    testScalarCommon(fd,String("string"),pvString,false,false,false);
}

static void testScalarArrayCommon(FILE * fd,String fieldName,ScalarType stype,
    bool isInteger,bool isNumeric,bool isPrimitive)
{
    ScalarArrayConstPtr pscalar = standardField->scalarArray(fieldName,stype);
    Type type = pscalar->getType();
    assert(type==scalarArray);
    builder.clear();
    TypeFunc::toString(&builder,type);
    assert(builder.compare("scalarArray")==0);
    ScalarType scalarType = pscalar->getElementType();
    assert(scalarType==stype);
    assert(ScalarTypeFunc::isInteger(scalarType)==isInteger);
    assert(ScalarTypeFunc::isNumeric(scalarType)==isNumeric);
    assert(ScalarTypeFunc::isPrimitive(scalarType)==isPrimitive);
    builder.clear();
    pscalar->toString(&builder);
    fprintf(fd,"%s\n",builder.c_str());
    // create tempory PVField so that memory can be released
    PVField *pvField = pvDataCreate->createPVField(0,pscalar);
    delete pvField;
}

static void testScalarArray(FILE * fd) {
    fprintf(fd,"\ntestScalarArray\n");
    testScalarArrayCommon(fd,String("boolean"),pvBoolean,false,false,true);
    testScalarArrayCommon(fd,String("byte"),pvByte,true,true,true);
    testScalarArrayCommon(fd,String("short"),pvShort,true,true,true);
    testScalarArrayCommon(fd,String("int"),pvInt,true,true,true);
    testScalarArrayCommon(fd,String("long"),pvLong,true,true,true);
    testScalarArrayCommon(fd,String("float"),pvFloat,false,true,true);
    testScalarArrayCommon(fd,String("double"),pvDouble,false,true,true);
    testScalarArrayCommon(fd,String("string"),pvString,false,false,false);
}

static void testSimpleStructure(FILE * fd) {
    fprintf(fd,"\ntestSimpleStructure\n");
    String properties("alarm,timeStamp,display,control,valueAlarm");
    StructureConstPtr ptop = standardField->scalarValue(pvDouble,properties);
    builder.clear();
    ptop->toString(&builder);
    fprintf(fd,"%s\n",builder.c_str());
    // create tempory PVField so that memory can be released
    PVField *pvField = pvDataCreate->createPVField(0,ptop);
    delete pvField;
}

static StructureConstPtr createPowerSupply() {
    String properties("alarm");
    FieldConstPtr powerSupply[3];
    powerSupply[0] = standardField->scalar(
        String("voltage"),pvDouble,properties);
    powerSupply[1] = standardField->scalar(
        String("power"),pvDouble,properties);
    powerSupply[2] = standardField->scalar(
        String("current"),pvDouble,properties);
    return standardField->structure( String("powerSupply"),3,powerSupply);
}

static void testStructureArray(FILE * fd) {
    fprintf(fd,"\ntestStructureArray\n");
    String properties("alarm,timeStamp");
    StructureConstPtr powerSupply = createPowerSupply();
    StructureConstPtr top = standardField->structureArrayValue(
         powerSupply,properties);
    builder.clear();
    top->toString(&builder);
    fprintf(fd,"%s\n",builder.c_str());
    // create tempory PVField so that memory can be released
    PVField *pvField = pvDataCreate->createPVField(0,top);
    delete pvField;
}

int main(int argc,char *argv[])
{
    int initialTotalReferences,finalTotalReferences;
    char *fileName = 0;
    if(argc>1) fileName = argv[1];
    FILE * fd = stdout;
    if(fileName!=0 && fileName[0]!=0) {
        fd = fopen(fileName,"w+");
    }
    fieldCreate = getFieldCreate();
    pvDataCreate = getPVDataCreate();
    standardField = getStandardField();
    initialTotalReferences = Field::getTotalReferenceCount();
    testScalar(fd);
    finalTotalReferences = Field::getTotalReferenceCount();
    assert(initialTotalReferences==finalTotalReferences);
    initialTotalReferences = Field::getTotalReferenceCount();
    testScalarArray(fd);
    finalTotalReferences = Field::getTotalReferenceCount();
    assert(initialTotalReferences==finalTotalReferences);
    initialTotalReferences = Field::getTotalReferenceCount();
    testSimpleStructure(fd);
    finalTotalReferences = Field::getTotalReferenceCount();
    assert(initialTotalReferences==finalTotalReferences);
    initialTotalReferences = Field::getTotalReferenceCount();
    testStructureArray(fd);
    finalTotalReferences = Field::getTotalReferenceCount();
    assert(initialTotalReferences==finalTotalReferences);
    initialTotalReferences = Field::getTotalReferenceCount();
    long long totalConstruct = Field::getTotalConstruct();
    long long totalDestruct = Field::getTotalDestruct();
    int totalReference = Field::getTotalReferenceCount();
    fprintf(fd,"Field:   totalConstruct %lli totalDestruct %lli totalReferenceCount %i\n",
        totalConstruct,totalDestruct,totalReference);
    assert(totalConstruct==(totalDestruct+totalReference));
    totalConstruct = PVField::getTotalConstruct();
    totalDestruct = PVField::getTotalDestruct();
    fprintf(fd,"PVField: totalConstruct %lli totalDestruct %lli\n",
        totalConstruct,totalDestruct);
    assert(totalConstruct==totalDestruct);
    return(0);
}

