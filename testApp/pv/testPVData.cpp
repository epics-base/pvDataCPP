/* testPVdata.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/* Author:  Marty Kraimer Date: 2010.11 */

#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>

#include <epicsUnitTest.h>
#include <testMain.h>

#include <pv/requester.h>
#include <pv/pvIntrospect.h>
#include <pv/pvData.h>
#include <pv/convert.h>
#include <pv/standardField.h>
#include <pv/standardPVField.h>
#include <pv/pvTimeStamp.h>
#include <pv/bitSet.h>

using namespace epics::pvData;
using std::tr1::static_pointer_cast;
using std::string;
using std::cout;
using std::endl;

static bool debug = false;

static FieldCreatePtr fieldCreate;
static PVDataCreatePtr pvDataCreate;
static StandardFieldPtr standardField;
static StandardPVFieldPtr standardPVField;
static ConvertPtr convert;
static string alarmTimeStamp("alarm,timeStamp");
static string alarmTimeStampValueAlarm("alarm,timeStamp,valueAlarm");
static string allProperties("alarm,timeStamp,display,control,valueAlarm");

static void testCreatePVStructure()
{
    if(debug) 
        std::cout << std::endl << "testCreatePVStructure" << std::endl;
    PVStructurePtr pv0 = standardPVField->scalar(
         pvDouble,alarmTimeStampValueAlarm);
    PVScalarPtr pv1 = pvDataCreate->createPVScalar(pvString);
    PVFieldPtrArray pvFields;
    StringArray fieldNames;
    pvFields.reserve(2);
    fieldNames.reserve(2);
    fieldNames.push_back("value");
    fieldNames.push_back("extra");
    pvFields.push_back(pv0);
    pvFields.push_back(pv1);
    PVStructurePtr pvParent = pvDataCreate->createPVStructure(
        fieldNames,pvFields);

    if(debug)
        std::cout << *pvParent << std::endl;
        
    std::cout << "testCreatePVStructure PASSED" << std::endl;
}

static void testPVScalarCommon(string /*fieldName*/,ScalarType stype)
{
    PVScalarPtr pvScalar = pvDataCreate->createPVScalar(stype);
    if(stype==pvBoolean) {
        convert->fromString(pvScalar,string("true"));
    } else {
        convert->fromString(pvScalar,string("10"));
    }
    if(debug)
        std::cout << *pvScalar << std::endl;
}

static void testPVScalarWithProperties(
    string /*fieldName*/,ScalarType stype)
{
    PVStructurePtr pvStructure;
    bool hasValueAlarm = false;
    bool hasBooleanAlarm = false;
    bool hasDisplayControl = false;
    switch(stype) {
        case pvBoolean: {
             pvStructure = standardPVField->scalar(
                 stype,alarmTimeStampValueAlarm);
             hasBooleanAlarm = true;
             PVBooleanPtr pvField = pvStructure->getSubField<PVBoolean>("value");
             pvField->put(true);
             break;
        }
        case pvByte: {
             pvStructure = standardPVField->scalar(
                 stype,allProperties);
             hasValueAlarm = true;
             hasDisplayControl = true;
             PVBytePtr pvField = pvStructure->getSubField<PVByte>("value");
             pvField->put(127);
             break;
        }
        case pvShort: {
             pvStructure = standardPVField->scalar(
                 stype,allProperties);
             hasValueAlarm = true;
             hasDisplayControl = true;
             PVShortPtr pvField = pvStructure->getSubField<PVShort>("value");
             pvField->put(32767);
             break;
        }
        case pvInt: {
             pvStructure = standardPVField->scalar(
                 stype,allProperties);
             hasValueAlarm = true;
             hasDisplayControl = true;
             PVIntPtr pvField = pvStructure->getSubField<PVInt>("value");
             pvField->put((int32)0x80000000);
             break;
        }
        case pvLong: {
             pvStructure = standardPVField->scalar(
                 stype,allProperties);
             hasValueAlarm = true;
             hasDisplayControl = true;
             PVLongPtr pvField = pvStructure->getSubField<PVLong>("value");
             int64 value = 0x80000000;
             value <<= 32;
             value |= 0xffffffff;
             pvField->put(value);
             break;
        }
        case pvUByte: {
             pvStructure = standardPVField->scalar(
                 stype,allProperties);
             hasValueAlarm = true;
             hasDisplayControl = true;
             PVUBytePtr pvField = pvStructure->getSubField<PVUByte>("value");
             pvField->put(255);
             break;
        }
        case pvUShort: {
             pvStructure = standardPVField->scalar(
                 stype,allProperties);
             hasValueAlarm = true;
             hasDisplayControl = true;
             PVUShortPtr pvField = pvStructure->getSubField<PVUShort>("value");
             pvField->put(65535);
             break;
        }
        case pvUInt: {
             pvStructure = standardPVField->scalar(
                 stype,allProperties);
             hasValueAlarm = true;
             hasDisplayControl = true;
             PVUIntPtr pvField = pvStructure->getSubField<PVUInt>("value");
             pvField->put((uint32)0x80000000);
             break;
        }
        case pvULong: {
             pvStructure = standardPVField->scalar(
                 stype,allProperties);
             hasValueAlarm = true;
             hasDisplayControl = true;
             PVULongPtr pvField = pvStructure->getSubField<PVULong>("value");
             int64 value = 0x80000000;
             value <<= 32;
             value |= 0xffffffff;
             pvField->put(value);
             break;
        }
        case pvFloat: {
             pvStructure = standardPVField->scalar(
                 stype,allProperties);
             hasValueAlarm = true;
             hasDisplayControl = true;
             PVFloatPtr pvField = pvStructure->getSubField<PVFloat>("value");
             pvField->put(1.123e8);
             break;
        }
        case pvDouble: {
             pvStructure = standardPVField->scalar(
                 stype,allProperties);
             hasValueAlarm = true;
             hasDisplayControl = true;
             PVDoublePtr pvField = pvStructure->getSubField<PVDouble>("value");
             pvField->put(1.123e35);
             break;
        }
        case pvString: {
             pvStructure = standardPVField->scalar(
                 stype,alarmTimeStamp);
             PVStringPtr pvField = pvStructure->getSubField<PVString>("value");
             pvField->put(string("this is a string"));
             break;
        }
    }
    PVLongPtr seconds = pvStructure->getSubField<PVLong>(
        string("timeStamp.secondsPastEpoch"));
    testOk1(seconds.get()!=0);
    seconds->put(123456789);
    PVIntPtr nano = pvStructure->getSubField<PVInt>(string("timeStamp.nanoseconds"));
    testOk1(nano.get()!=0);
    nano->put(1000000);
    PVIntPtr severity = pvStructure->getSubField<PVInt>(string("alarm.severity"));
    testOk1(severity.get()!=0);
    severity->put(2);
    PVStringPtr message = pvStructure->getSubField<PVString>(string("alarm.message"));
    testOk1(message.get()!=0);
    message->put(string("messageForAlarm"));
    if(hasDisplayControl) {
        PVStringPtr desc = pvStructure->getSubField<PVString>(
            string("display.description"));
        testOk1(desc.get()!=0);
        desc->put(string("this is a description"));
        PVStringPtr format = pvStructure->getSubField<PVString>(
            string("display.format"));
        testOk1(format.get()!=0);
        format->put(string("f10.2"));
        PVStringPtr units = pvStructure->getSubField<PVString>(
            string("display.units"));
        testOk1(units.get()!=0);
        units->put(string("SomeUnits"));
        PVDoublePtr limit = pvStructure->getSubField<PVDouble>(
            string("display.limitLow"));
        testOk1(limit.get()!=0);
        limit->put(0.0);
        limit = pvStructure->getSubField<PVDouble>(
            string("display.limitHigh"));
        testOk1(limit.get()!=0);
        limit->put(10.0);
        limit = pvStructure->getSubField<PVDouble>(
            string("control.limitLow"));
        testOk1(limit.get()!=0);
        limit->put(1.0);
        limit = pvStructure->getSubField<PVDouble>(
            string("control.limitHigh"));
        testOk1(limit.get()!=0);
        limit->put(9.0);
    }
    if(hasValueAlarm) {
        PVFieldPtr  pvField = pvStructure->getSubField(
            string("valueAlarm.active"));
        PVBooleanPtr pvBoolean = static_pointer_cast<PVBoolean>(pvField);
        pvBoolean->put(true);
        pvField = pvStructure->getSubField(
            string("valueAlarm.lowAlarmLimit"));
        PVScalarPtr pvtemp = static_pointer_cast<PVScalar>(pvField);
        testOk1(pvtemp.get()!=0);
        convert->fromDouble(pvtemp,1.0);
        pvField = pvStructure->getSubField(
            string("valueAlarm.highAlarmLimit"));
        pvtemp = static_pointer_cast<PVScalar>(pvField);
        testOk1(pvtemp.get()!=0);
        convert->fromDouble(pvtemp,9.0);
        severity = pvStructure->getSubField<PVInt>(
            string("valueAlarm.lowAlarmSeverity"));
        testOk1(severity.get()!=0);
        severity->put(2);
        severity = pvStructure->getSubField<PVInt>(
            string("valueAlarm.highAlarmSeverity"));
        testOk1(severity.get()!=0);
        severity->put(2);
        PVBooleanPtr active = pvStructure->getSubField<PVBoolean>(
            string("valueAlarm.active"));
        testOk1(active.get()!=0);
        active->put(true);
    }
    if(hasBooleanAlarm) {
        PVFieldPtr  pvField = pvStructure->getSubField(
            string("valueAlarm.active"));
        PVBooleanPtr pvBoolean = static_pointer_cast<PVBoolean>(pvField);
        pvBoolean->put(true);
        severity = pvStructure->getSubField<PVInt>(
            string("valueAlarm.falseSeverity"));
        testOk1(severity.get()!=0);
        severity->put(0);
        severity = pvStructure->getSubField<PVInt>(
            string("valueAlarm.trueSeverity"));
        testOk1(severity.get()!=0);
        severity->put(2);
        severity = pvStructure->getSubField<PVInt>(
            string("valueAlarm.changeStateSeverity"));
        testOk1(severity.get()!=0);
        severity->put(1);
    }
    if(debug)
        std::cout << *pvStructure << std::endl;
}

static void testPVScalar()
{
    if(debug)
        std::cout << std::endl << "testScalar" << std::endl;
    testPVScalarCommon(string("boolean"),pvByte);
    testPVScalarCommon(string("byte"),pvByte);
    testPVScalarCommon(string("short"),pvShort);
    testPVScalarCommon(string("int"),pvInt);
    testPVScalarCommon(string("long"),pvLong);
    testPVScalarCommon(string("ubyte"),pvUByte);
    testPVScalarCommon(string("ushort"),pvUShort);
    testPVScalarCommon(string("uint"),pvUInt);
    testPVScalarCommon(string("ulong"),pvULong);
    testPVScalarCommon(string("float"),pvFloat);
    testPVScalarCommon(string("double"),pvDouble);
    testPVScalarCommon(string("string"),pvString);

    testPVScalarWithProperties(string("boolean"),pvBoolean);
    testPVScalarWithProperties(string("byte"),pvByte);
    testPVScalarWithProperties(string("short"),pvShort);
    testPVScalarWithProperties(string("int"),pvInt);
    testPVScalarWithProperties(string("long"),pvLong);
    testPVScalarWithProperties(string("ubyte"),pvUByte);
    testPVScalarWithProperties(string("ushort"),pvUShort);
    testPVScalarWithProperties(string("uint"),pvUInt);
    testPVScalarWithProperties(string("ulong"),pvULong);
    testPVScalarWithProperties(string("float"),pvFloat);
    testPVScalarWithProperties(string("double"),pvDouble);
    testPVScalarWithProperties(string("string"),pvString);
    
    std::cout << "testPVScalar PASSED" << std::endl;
}


static void testScalarArrayCommon(string /*fieldName*/,ScalarType stype)
{
    PVStructurePtr pvStructure = standardPVField->scalarArray(
        stype,alarmTimeStamp);
    PVScalarArrayPtr scalarArray = pvStructure->getScalarArrayField(
        "value",stype);
    testOk1(scalarArray.get()!=0);
    if(stype==pvBoolean) {
        StringArray values(3);
        values[0] = "true";
        values[1] = "false";
        values[2] = "true";
        convert->fromStringArray(scalarArray, 0,3,values,0);
    } else {
        StringArray values(3);
        values[0] = "0";
        values[1] = "1";
        values[2] = "2";
        convert->fromStringArray(scalarArray, 0,3,values,0);
    }
    if(debug)
        std::cout << *pvStructure << std::endl;
    PVFieldPtr pvField = pvStructure->getSubField("alarm.status");
    testOk1(pvField.get()!=0);
}

static void testScalarArray()
{
    if(debug) 
        std::cout << std::endl << "testScalarArray" << std::endl;
    testScalarArrayCommon(string("boolean"),pvBoolean);
    testScalarArrayCommon(string("byte"),pvByte);
    testScalarArrayCommon(string("short"),pvShort);
    testScalarArrayCommon(string("int"),pvInt);
    testScalarArrayCommon(string("long"),pvLong);
    testScalarArrayCommon(string("float"),pvFloat);
    testScalarArrayCommon(string("double"),pvDouble);
    testScalarArrayCommon(string("string"),pvString);
    std::cout << "testScalarArray PASSED" << std::endl;
}

static void testRequest()
{
    if(debug)
        std::cout << std::endl << "testScalarArray" << std::endl;
        
    StringArray nullNames;
    FieldConstPtrArray nullFields;
    StringArray optionNames(1);
    FieldConstPtrArray optionFields(1);
    optionNames[0] = "process";
    optionFields[0] = fieldCreate->createScalar(pvString);
    StringArray recordNames(1);
    FieldConstPtrArray recordFields(1);
    recordNames[0] = "_options";
    recordFields[0] = fieldCreate->createStructure(optionNames,optionFields);
    StringArray fieldNames(2);
    FieldConstPtrArray fieldFields(2);
    fieldNames[0] = "alarm";
    fieldFields[0] = fieldCreate->createStructure(nullNames,nullFields);
    fieldNames[1] = "timeStamp";
    fieldFields[1] = fieldCreate->createStructure(nullNames,nullFields);
    StringArray topNames(2);
    FieldConstPtrArray topFields(2);
    topNames[0] = "record";
    topFields[0] = fieldCreate->createStructure(recordNames,recordFields);
    topNames[1] = "field";
    topFields[1] = fieldCreate->createStructure(fieldNames,fieldFields);
    StructureConstPtr topStructure = fieldCreate->createStructure(
        topNames,topFields);
cout << *topStructure << endl;
    PVStructurePtr pvTop = pvDataCreate->createPVStructure(topStructure);
cout << *pvTop << endl;
cout << *pvTop->getStructure() << endl;
PVStructurePtr xxx = pvTop->getSubField<PVStructure>("record");
cout << *xxx << endl;
xxx = pvTop->getSubField<PVStructure>("field");
cout << *xxx << endl;
PVStringPtr pvString = pvTop->getSubField<PVString>("record._options.process");
pvString->put("true");
cout << *pvTop << endl;

string subName("record._options.process");
PVFieldPtr pvField = pvTop->getSubField(subName);
string fieldName = pvField->getFieldName();
string fullName = pvField->getFullName();
cout << "fieldName " << fieldName << " fullName " << fullName << endl;

    testOk1(fieldName.compare("process")==0);
    testOk1(fullName.compare(subName)==0);

}

#define STRINGIZE(A) #A
#define TEST_COPY(T, V) testCopyCase<T>(V, STRINGIZE(T))

template<typename T>
void testCopyCase(typename T::value_type val, const char* typeName)
{
    typename T::shared_pointer pv = pvDataCreate->createPVScalar<T>();
    pv->put(val);

    typename T::shared_pointer pv2 = pvDataCreate->createPVScalar<T>();
    pv2->copy(*pv);

    testOk(*pv == *pv2, "testCopyCase (copy) for type '%s'", typeName);

    typename T::shared_pointer pv3 = pvDataCreate->createPVScalar<T>();
    pv3->copy(*pv);

    testOk(*pv == *pv3, "testCopyCase (copyUnchecked) for type '%s'", typeName);
}

static void testCopy()
{
    if(debug)
        std::cout << std::endl << "testCopy" << std::endl;

    TEST_COPY(PVBoolean, 1);
    TEST_COPY(PVByte, 12);
    TEST_COPY(PVShort, 128);
    TEST_COPY(PVInt, 128000);
    TEST_COPY(PVLong, 128000000);
    TEST_COPY(PVUByte, 12);
    TEST_COPY(PVUShort, 128);
    TEST_COPY(PVUInt, 128000);
    TEST_COPY(PVULong, 128000000);
    TEST_COPY(PVFloat, 12.8);
    TEST_COPY(PVDouble, 8.12);
    TEST_COPY(PVString, "jikes");

    int32 testValue = 128;

    // PVStructure test
    PVStructurePtr pvStructure =
            standardPVField->scalar(pvInt, alarmTimeStampValueAlarm);
    pvStructure->getSubField<PVInt>("value")->put(testValue);

    PVTimeStamp timeStamp;
    timeStamp.attach(pvStructure->getSubField<PVStructure>("timeStamp"));
    TimeStamp current;
    current.getCurrent();
    timeStamp.set(current);

    PVStructurePtr pvStructureCopy =
            standardPVField->scalar(pvInt, alarmTimeStampValueAlarm);
    pvStructureCopy->copy(*pvStructure);
    testOk(*pvStructure == *pvStructureCopy, "PVStructure copy");


    PVStructurePtr pvStructureCopy2 =
            standardPVField->scalar(pvInt, alarmTimeStampValueAlarm);
    pvStructureCopy2->copyUnchecked(*pvStructure);
    testOk(*pvStructure == *pvStructureCopy2, "PVStructure copyUnchecked");

    BitSet mask(pvStructure->getNumberFields());
    PVStructurePtr pvStructureCopy3 =
            standardPVField->scalar(pvInt, alarmTimeStampValueAlarm);
    PVStructurePtr pvStructureCopy4 =
            standardPVField->scalar(pvInt, alarmTimeStampValueAlarm);
    pvStructureCopy3->copyUnchecked(*pvStructure, mask);
    testOk(*pvStructureCopy3 == *pvStructureCopy4, "PVStructure copyUnchecked w/ cleared mask");

    mask.set(pvStructure->getSubField<PVInt>("value")->getFieldOffset());
    pvStructureCopy3->copyUnchecked(*pvStructure, mask);
    pvStructureCopy4->getSubField<PVInt>("value")->put(testValue);
    testOk(*pvStructureCopy3 == *pvStructureCopy4, "PVStructure copyUnchecked w/ value mask only");

    mask.set(pvStructure->getSubField<PVStructure>("timeStamp")->getFieldOffset());
    PVStructurePtr pvStructureCopy5 =
            standardPVField->scalar(pvInt, alarmTimeStampValueAlarm);
    pvStructureCopy5->copyUnchecked(*pvStructure, mask);
    testOk(*pvStructure == *pvStructureCopy5, "PVStructure copyUnchecked w/ value+timeStamp mask");


    UnionConstPtr _union = fieldCreate->createFieldBuilder()->
        add("doubleValue", pvDouble)->
        add("intValue", pvInt)->
        add("timeStamp",standardField->timeStamp())->
        createUnion();

    PVUnionPtr pvUnion = pvDataCreate->createPVUnion(_union);
    PVUnionPtr pvUnion2 = pvDataCreate->createPVUnion(_union);
    pvUnion2->copy(*pvUnion);
    testOk(*pvUnion == *pvUnion2, "null PVUnion copy");

    pvUnion->select<PVInt>("intValue")->put(123);
    pvUnion2->copy(*pvUnion);
    testOk(*pvUnion == *pvUnion2, "PVUnion scalar copy, to null PVUnion");

    pvUnion->select("doubleValue");
    pvUnion2->copy(*pvUnion);
    testOk(*pvUnion == *pvUnion2, "PVUnion scalar copy, to different type PVUnion");

    pvUnion->select<PVStructure>("timeStamp")->copy(
        *pvStructure->getSubField<PVStructure>("timeStamp")
    );
    pvUnion2->copy(*pvUnion);
    testOk(*pvUnion == *pvUnion2, "PVUnion PVStructure copy, to different type PVUnion");
}

static void testFieldAccess()
{
    testDiag("Check methods for accessing structure fields");

    StructureConstPtr tdef = fieldCreate->createFieldBuilder()->
            add("test", pvInt)->
            addNestedStructure("hello")->
              add("world", pvInt)->
            endNested()->
            createStructure();

    PVStructurePtr fld = pvDataCreate->createPVStructure(tdef);

    PVIntPtr a = fld->getSubField<PVInt>("test");
    testOk1(a!=NULL);
    if(a.get()) {
        PVInt& b = fld->getAs<PVInt>("test");
        testOk(&b==a.get(), "%p == %p", &b, a.get());
    } else
        testSkip(1, "test doesn't exist?");

    a = fld->getSubField<PVInt>("hello.world");
    testOk1(a!=NULL);
    if(a.get()) {
        PVInt& b = fld->getAs<PVInt>("hello.world");
        testOk(&b==a.get(), "%p == %p", &b, a.get());
    } else
        testSkip(1, "hello.world doesn't exist?");

    // non-existent
    testOk1(fld->getSubField<PVInt>("invalid").get()==NULL);

    // wrong type
    testOk1(fld->getSubField<PVDouble>("test").get()==NULL);

    // intermediate struct non-existent
    testOk1(fld->getSubField<PVDouble>("helo.world").get()==NULL);

    // empty leaf field name
    testOk1(fld->getSubField<PVDouble>("hello.").get()==NULL);

    // empty field name
    testOk1(fld->getSubField<PVDouble>("hello..world").get()==NULL);
    testOk1(fld->getSubField<PVDouble>(".").get()==NULL);

    // whitespace
    testOk1(fld->getSubField<PVInt>(" test").get()==NULL);

    // intermediate field not structure
    testOk1(fld->getSubField<PVInt>("hello.world.invalid").get()==NULL);

    // null string
    try{
        char * name = NULL;
        fld->getAs<PVInt>(name);
        testFail("missing required exception");
    }catch(std::invalid_argument& e){
        testPass("caught expected exception: %s", e.what());
    }

    // non-existent
    try{
        fld->getAs<PVInt>("invalid");
        testFail("missing required exception");
    }catch(std::runtime_error& e){
        testPass("caught expected exception: %s", e.what());
    }

    // wrong type
    try{
        fld->getAs<PVDouble>("test");
        testFail("missing required exception");
    }catch(std::runtime_error& e){
        testPass("caught expected exception: %s", e.what());
    }

    // empty leaf field name
    try{
        fld->getAs<PVDouble>("hello.");
        testFail("missing required exception");
    }catch(std::runtime_error& e){
        testPass("caught expected exception: %s", e.what());
    }

    // empty field name
    try{
        fld->getAs<PVDouble>("hello..world");
        testFail("missing required exception");
    }catch(std::runtime_error& e){
        testPass("caught expected exception: %s", e.what());
    }
    try{
        fld->getAs<PVDouble>(".");
        testFail("missing required exception");
    }catch(std::runtime_error& e){
        testPass("caught expected exception: %s", e.what());
    }

    // whitespace
    try{
        fld->getAs<PVDouble>(" test");
        testFail("missing required exception");
    }catch(std::runtime_error& e){
        testPass("caught expected exception: %s", e.what());
    }

    // intermediate field not structure
    try{
        fld->getAs<PVDouble>("hello.world.invalid");
        testFail("missing required exception");
    }catch(std::runtime_error& e){
        testPass("caught expected exception: %s", e.what());
    }
}

MAIN(testPVData)
{
    testPlan(242);
    fieldCreate = getFieldCreate();
    pvDataCreate = getPVDataCreate();
    standardField = getStandardField();
    standardPVField = getStandardPVField();
    convert = getConvert();
    testCreatePVStructure();
    testPVScalar();
    testScalarArray();
    testRequest();
    testCopy();
    testFieldAccess();
    return testDone();
}

