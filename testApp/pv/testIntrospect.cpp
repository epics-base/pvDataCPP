/* testIntrospect.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/* Author:  Marty Kraimer Date: 2010.09 */

#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>

#include <epicsAssert.h>
#include <epicsExit.h>

#include <pv/requester.h>
#include <pv/executor.h>
#include <pv/pvIntrospect.h>
#include <pv/pvData.h>
#include <pv/standardField.h>

using namespace epics::pvData;

static FieldCreatePtr fieldCreate;
static PVDataCreatePtr pvDataCreate;
static StandardFieldPtr standardField;
static String builder("");

static void testScalarCommon(FILE * fd,ScalarType stype,
    bool isInteger,bool isNumeric,bool isPrimitive)
{
    ScalarConstPtr pscalar = fieldCreate->createScalar(stype);
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
    PVFieldPtr pvField = pvDataCreate->createPVScalar(pscalar);
}

static void testScalar(FILE * fd) {
    fprintf(fd,"\ntestScalar\n");
    testScalarCommon(fd,pvBoolean,false,false,true);
    testScalarCommon(fd,pvByte,true,true,true);
    testScalarCommon(fd,pvShort,true,true,true);
    testScalarCommon(fd,pvInt,true,true,true);
    testScalarCommon(fd,pvLong,true,true,true);
    testScalarCommon(fd,pvFloat,false,true,true);
    testScalarCommon(fd,pvDouble,false,true,true);
    testScalarCommon(fd,pvString,false,false,false);
}

static void testScalarArrayCommon(FILE * fd,ScalarType stype,
    bool isInteger,bool isNumeric,bool isPrimitive)
{
    ScalarArrayConstPtr pscalar = fieldCreate->createScalarArray(stype);
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
    PVFieldPtr pvField = pvDataCreate->createPVScalarArray(pscalar);
}

static void testScalarArray(FILE * fd) {
    fprintf(fd,"\ntestScalarArray\n");
    testScalarArrayCommon(fd,pvBoolean,false,false,true);
    testScalarArrayCommon(fd,pvByte,true,true,true);
    testScalarArrayCommon(fd,pvShort,true,true,true);
    testScalarArrayCommon(fd,pvInt,true,true,true);
    testScalarArrayCommon(fd,pvLong,true,true,true);
    testScalarArrayCommon(fd,pvFloat,false,true,true);
    testScalarArrayCommon(fd,pvDouble,false,true,true);
    testScalarArrayCommon(fd,pvString,false,false,false);
}

static void testSimpleStructure(FILE * fd) {
    fprintf(fd,"\ntestSimpleStructure\n");
    String properties("alarm,timeStamp,display,control,valueAlarm");
    StructureConstPtr ptop = standardField->scalar(pvDouble,properties);
    builder.clear();
    ptop->toString(&builder);
    fprintf(fd,"%s\n",builder.c_str());
    // create tempory PVField so that memory can be released
    PVStructurePtr pvStructure = pvDataCreate->createPVStructure(ptop);
}

static StructureConstPtr createPowerSupply() {
    size_t nfields = 3;
    String properties("alarm");
    StringArray names;
    names.reserve(nfields);
    FieldConstPtrArray powerSupply;
    powerSupply.reserve(nfields);
    names.push_back("voltage");
    powerSupply.push_back(standardField->scalar(pvDouble,properties));
    names.push_back("power");
    powerSupply.push_back(standardField->scalar(pvDouble,properties));
    names.push_back("current");
    powerSupply.push_back(standardField->scalar(pvDouble,properties));
    return fieldCreate->createStructure(names,powerSupply);
}

static void testStructureArray(FILE * fd) {
    fprintf(fd,"\ntestStructureArray\n");
    String properties("alarm,timeStamp");
    StructureConstPtr powerSupply = createPowerSupply();
    StructureConstPtr top = standardField->structureArray(
         powerSupply,properties);
    builder.clear();
    top->toString(&builder);
    fprintf(fd,"%s\n",builder.c_str());
}

int main(int argc,char *argv[])
{
    char *fileName = 0;
    if(argc>1) fileName = argv[1];
    FILE * fd = stdout;
    if(fileName!=0 && fileName[0]!=0) {
        fd = fopen(fileName,"w+");
    }
    fieldCreate = getFieldCreate();
    pvDataCreate = getPVDataCreate();
    standardField = getStandardField();
    testScalar(fd);
    testScalarArray(fd);
    testSimpleStructure(fd);
    testStructureArray(fd);
    return(0);
}

