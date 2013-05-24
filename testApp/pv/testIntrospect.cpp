/* testIntrospect.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/* Author:  Marty Kraimer Date: 2010.09 */

#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>

#include <epicsUnitTest.h>
#include <testMain.h>

#include <pv/requester.h>
#include <pv/executor.h>
#include <pv/pvIntrospect.h>
#include <pv/pvData.h>
#include <pv/standardField.h>

using namespace epics::pvData;

static FieldCreatePtr fieldCreate;
static PVDataCreatePtr pvDataCreate;
static StandardFieldPtr standardField;

static void testScalarCommon(ScalarType stype,
    bool isInteger,bool isNumeric,bool isPrimitive)
{
    String builder;
    ScalarConstPtr pscalar = fieldCreate->createScalar(stype);
    Type type = pscalar->getType();
    testOk1(type==scalar);
    builder.clear();
    TypeFunc::toString(&builder,type);
    testOk1(builder.compare("scalar")==0);
    ScalarType scalarType = pscalar->getScalarType();
    testOk1(scalarType==stype);
    testOk1(ScalarTypeFunc::isInteger(scalarType)==isInteger);
    testOk1(ScalarTypeFunc::isNumeric(scalarType)==isNumeric);
    testOk1(ScalarTypeFunc::isPrimitive(scalarType)==isPrimitive);
}

static void testScalar() {
    testDiag("testScalar");
    testScalarCommon(pvBoolean,false,false,true);
    testScalarCommon(pvByte,true,true,true);
    testScalarCommon(pvShort,true,true,true);
    testScalarCommon(pvInt,true,true,true);
    testScalarCommon(pvLong,true,true,true);
    testScalarCommon(pvFloat,false,true,true);
    testScalarCommon(pvDouble,false,true,true);
    testScalarCommon(pvString,false,false,false);
}

static void testScalarArrayCommon(ScalarType stype,
    bool isInteger,bool isNumeric,bool isPrimitive)
{
    String builder;
    ScalarArrayConstPtr pscalar = fieldCreate->createScalarArray(stype);
    Type type = pscalar->getType();
    testOk1(type==scalarArray);
    builder.clear();
    TypeFunc::toString(&builder,type);
    testOk1(builder.compare("scalarArray")==0);
    ScalarType scalarType = pscalar->getElementType();
    testOk1(scalarType==stype);
    testOk1(ScalarTypeFunc::isInteger(scalarType)==isInteger);
    testOk1(ScalarTypeFunc::isNumeric(scalarType)==isNumeric);
    testOk1(ScalarTypeFunc::isPrimitive(scalarType)==isPrimitive);
}

static void testScalarArray() {
    testDiag("testScalarArray");
    testScalarArrayCommon(pvBoolean,false,false,true);
    testScalarArrayCommon(pvByte,true,true,true);
    testScalarArrayCommon(pvShort,true,true,true);
    testScalarArrayCommon(pvInt,true,true,true);
    testScalarArrayCommon(pvLong,true,true,true);
    testScalarArrayCommon(pvFloat,false,true,true);
    testScalarArrayCommon(pvDouble,false,true,true);
    testScalarArrayCommon(pvString,false,false,false);
}

static void testStructure()
{
    testDiag("testStructure");
    StringArray names1(2);
    names1[0] = "innerA";
    names1[1] = "innerB";
    FieldConstPtrArray fields1(2);
    fields1[0] = fieldCreate->createScalar(pvDouble);
    fields1[1] = fieldCreate->createScalarArray(pvString);

    StructureConstPtr struct1 = fieldCreate->createStructure(names1, fields1);

    testOk1(struct1->getNumberFields()==2);
    testOk1(struct1->getField("innerA")==fields1[0]);
    testOk1(struct1->getField("innerB")==fields1[1]);
    testOk1(struct1->getFieldIndex("innerA")==0);
    testOk1(struct1->getFieldIndex("innerB")==1);
    testOk1(struct1->getField(0)==fields1[0]);
    testOk1(struct1->getField(1)==fields1[1]);
    testOk1(struct1->getFieldName(0)==names1[0]);
    testOk1(struct1->getFieldName(1)==names1[1]);

    testOk1(struct1->getID() == Structure::DEFAULT_ID);

    testOk1(fields1 == struct1->getFields()); // vector equality

    StructureArrayConstPtr struct1arr = fieldCreate->createStructureArray(struct1);

    testOk1(struct1arr->getStructure()==struct1);
    testOk1(struct1arr->getID()=="structure[]");
}

#define testExcept(EXCEPT, CMD) try{ CMD; testFail( "No exception from: " #CMD); } \
catch(EXCEPT& e) {testPass("Got expected exception from: " #CMD);} \
catch(std::exception& e) {testFail("Got wrong exception %s(%s) from: " #CMD, typeid(e).name(),e.what());} \
catch(...) {testFail("Got unknown execption from: " #CMD);}

static void testError()
{
    testDiag("testError");
    ScalarType invalidtype = (ScalarType)9999;

    testExcept(std::invalid_argument, ScalarTypeFunc::getScalarType("invalidtype"));

    testExcept(std::invalid_argument, ScalarTypeFunc::elementSize(invalidtype));

    testExcept(std::invalid_argument, ScalarTypeFunc::name(invalidtype));

    testOk1(!ScalarTypeFunc::isInteger(invalidtype));
    testOk1(!ScalarTypeFunc::isUInteger(invalidtype));
    testOk1(!ScalarTypeFunc::isNumeric(invalidtype));
    testOk1(!ScalarTypeFunc::isPrimitive(invalidtype));

    testExcept(std::invalid_argument, fieldCreate->createScalar(invalidtype));
    testExcept(std::invalid_argument, fieldCreate->createScalarArray(invalidtype));

    StringArray names;
    FieldConstPtrArray fields(1);

    // fails because names.size()!=fields.size()
    testExcept(std::invalid_argument, fieldCreate->createStructure(names,fields));

    names.resize(1);;

    // fails because names[0].size()==0
    testExcept(std::invalid_argument, fieldCreate->createStructure(names,fields));

    names[0] = "hello";

    // fails because fields[0].get()==NULL
    testExcept(std::invalid_argument, fieldCreate->createStructure(names,fields));

    fields[0] = std::tr1::static_pointer_cast<const Field>(fieldCreate->createScalar(pvDouble));

    testOk1(fieldCreate->createStructure(names,fields).get()!=NULL);
}

MAIN(testIntrospect)
{
    testPlan(122);
    fieldCreate = getFieldCreate();
    pvDataCreate = getPVDataCreate();
    standardField = getStandardField();
    testScalar();
    testScalarArray();
    testStructure();
    testError();
    return testDone();
}
