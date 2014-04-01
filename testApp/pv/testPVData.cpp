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

using namespace epics::pvData;
using std::tr1::static_pointer_cast;
using std::cout;
using std::endl;

static bool debug = false;

static FieldCreatePtr fieldCreate;
static PVDataCreatePtr pvDataCreate;
static StandardFieldPtr standardField;
static StandardPVFieldPtr standardPVField;
static ConvertPtr convert;
static String builder("");
static String alarmTimeStamp("alarm,timeStamp");
static String alarmTimeStampValueAlarm("alarm,timeStamp,valueAlarm");
static String allProperties("alarm,timeStamp,display,control,valueAlarm");

static void testCreatePVStructure()
{
    if(debug) printf("\ntestCreatePVStructure\n");
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
    builder.clear();
    pvParent->toString(&builder);
    if(debug) printf("%s\n",builder.c_str());
    printf("testCreatePVStructure PASSED\n");
}

static void testPVScalarCommon(String /*fieldName*/,ScalarType stype)
{
    PVScalarPtr pvScalar = pvDataCreate->createPVScalar(stype);
    if(stype==pvBoolean) {
        convert->fromString(pvScalar,String("true"));
    } else {
        convert->fromString(pvScalar,String("10"));
    }
    builder.clear();
    pvScalar->toString(&builder);
    if(debug) printf("%s\n",builder.c_str());
}

static void testPVScalarWithProperties(
    String /*fieldName*/,ScalarType stype)
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
             PVBooleanPtr pvField = pvStructure->getBooleanField("value");
             pvField->put(true);
             break;
        }
        case pvByte: {
             pvStructure = standardPVField->scalar(
                 stype,allProperties);
             hasValueAlarm = true;
             hasDisplayControl = true;
             PVBytePtr pvField = pvStructure->getByteField("value");
             pvField->put(127);
             break;
        }
        case pvShort: {
             pvStructure = standardPVField->scalar(
                 stype,allProperties);
             hasValueAlarm = true;
             hasDisplayControl = true;
             PVShortPtr pvField = pvStructure->getShortField("value");
             pvField->put(32767);
             break;
        }
        case pvInt: {
             pvStructure = standardPVField->scalar(
                 stype,allProperties);
             hasValueAlarm = true;
             hasDisplayControl = true;
             PVIntPtr pvField = pvStructure->getIntField("value");
             pvField->put((int32)0x80000000);
             break;
        }
        case pvLong: {
             pvStructure = standardPVField->scalar(
                 stype,allProperties);
             hasValueAlarm = true;
             hasDisplayControl = true;
             PVLongPtr pvField = pvStructure->getLongField("value");
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
             PVUBytePtr pvField = pvStructure->getUByteField("value");
             pvField->put(255);
             break;
        }
        case pvUShort: {
             pvStructure = standardPVField->scalar(
                 stype,allProperties);
             hasValueAlarm = true;
             hasDisplayControl = true;
             PVUShortPtr pvField = pvStructure->getUShortField("value");
             pvField->put(65535);
             break;
        }
        case pvUInt: {
             pvStructure = standardPVField->scalar(
                 stype,allProperties);
             hasValueAlarm = true;
             hasDisplayControl = true;
             PVUIntPtr pvField = pvStructure->getUIntField("value");
             pvField->put((uint32)0x80000000);
             break;
        }
        case pvULong: {
             pvStructure = standardPVField->scalar(
                 stype,allProperties);
             hasValueAlarm = true;
             hasDisplayControl = true;
             PVULongPtr pvField = pvStructure->getULongField("value");
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
             PVFloatPtr pvField = pvStructure->getFloatField("value");
             pvField->put(1.123e8);
             break;
        }
        case pvDouble: {
             pvStructure = standardPVField->scalar(
                 stype,allProperties);
             hasValueAlarm = true;
             hasDisplayControl = true;
             PVDoublePtr pvField = pvStructure->getDoubleField("value");
             pvField->put(1.123e35);
             break;
        }
        case pvString: {
             pvStructure = standardPVField->scalar(
                 stype,alarmTimeStamp);
             PVStringPtr pvField = pvStructure->getStringField("value");
             pvField->put(String("this is a string"));
             break;
        }
    }
    PVLongPtr seconds = pvStructure->getLongField(
        String("timeStamp.secondsPastEpoch"));
    testOk1(seconds!=0);
    seconds->put(123456789);
    PVIntPtr nano = pvStructure->getIntField(String("timeStamp.nanoSeconds"));
    testOk1(nano!=0);
    nano->put(1000000);
    PVIntPtr severity = pvStructure->getIntField(String("alarm.severity"));
    testOk1(severity!=0);
    severity->put(2);
    PVStringPtr message = pvStructure->getStringField(String("alarm.message"));
    testOk1(message!=0);
    message->put(String("messageForAlarm"));
    if(hasDisplayControl) {
        PVStringPtr desc = pvStructure->getStringField(
            String("display.description"));
        testOk1(desc!=0);
        desc->put(String("this is a description"));
        PVStringPtr format = pvStructure->getStringField(
            String("display.format"));
        testOk1(format!=0);
        format->put(String("f10.2"));
        PVStringPtr units = pvStructure->getStringField(
            String("display.units"));
        testOk1(units!=0);
        units->put(String("SomeUnits"));
        PVDoublePtr limit = pvStructure->getDoubleField(
            String("display.limitLow"));
        testOk1(limit!=0);
        limit->put(0.0);
        limit = pvStructure->getDoubleField(
            String("display.limitHigh"));
        testOk1(limit!=0);
        limit->put(10.0);
        limit = pvStructure->getDoubleField(
            String("control.limitLow"));
        testOk1(limit!=0);
        limit->put(1.0);
        limit = pvStructure->getDoubleField(
            String("control.limitHigh"));
        testOk1(limit!=0);
        limit->put(9.0);
    }
    if(hasValueAlarm) {
        PVFieldPtr  pvField = pvStructure->getSubField(
            String("valueAlarm.active"));
        PVBooleanPtr pvBoolean = static_pointer_cast<PVBoolean>(pvField);
        pvBoolean->put(true);
        pvField = pvStructure->getSubField(
            String("valueAlarm.lowAlarmLimit"));
        PVScalarPtr pvtemp = static_pointer_cast<PVScalar>(pvField);
        testOk1(pvtemp.get()!=0);
        convert->fromDouble(pvtemp,1.0);
        pvField = pvStructure->getSubField(
            String("valueAlarm.highAlarmLimit"));
        pvtemp = static_pointer_cast<PVScalar>(pvField);
        testOk1(pvtemp.get()!=0);
        convert->fromDouble(pvtemp,9.0);
        severity = pvStructure->getIntField(
            String("valueAlarm.lowAlarmSeverity"));
        testOk1(severity!=0);
        severity->put(2);
        severity = pvStructure->getIntField(
            String("valueAlarm.highAlarmSeverity"));
        testOk1(severity!=0);
        severity->put(2);
        PVBooleanPtr active = pvStructure->getBooleanField(
            String("valueAlarm.active"));
        testOk1(active!=0);
        active->put(true);
    }
    if(hasBooleanAlarm) {
        PVFieldPtr  pvField = pvStructure->getSubField(
            String("valueAlarm.active"));
        PVBooleanPtr pvBoolean = static_pointer_cast<PVBoolean>(pvField);
        pvBoolean->put(true);
        severity = pvStructure->getIntField(
            String("valueAlarm.falseSeverity"));
        testOk1(severity!=0);
        severity->put(0);
        severity = pvStructure->getIntField(
            String("valueAlarm.trueSeverity"));
        testOk1(severity!=0);
        severity->put(2);
        severity = pvStructure->getIntField(
            String("valueAlarm.changeStateSeverity"));
        testOk1(severity!=0);
        severity->put(1);
    }
    builder.clear();
    pvStructure->toString(&builder);
    if(debug) printf("%s\n",builder.c_str());
}

static void testPVScalar()
{
    if(debug) printf("\ntestScalar\n");
    testPVScalarCommon(String("boolean"),pvByte);
    testPVScalarCommon(String("byte"),pvByte);
    testPVScalarCommon(String("short"),pvShort);
    testPVScalarCommon(String("int"),pvInt);
    testPVScalarCommon(String("long"),pvLong);
    testPVScalarCommon(String("ubyte"),pvUByte);
    testPVScalarCommon(String("ushort"),pvUShort);
    testPVScalarCommon(String("uint"),pvUInt);
    testPVScalarCommon(String("ulong"),pvULong);
    testPVScalarCommon(String("float"),pvFloat);
    testPVScalarCommon(String("double"),pvDouble);
    testPVScalarCommon(String("string"),pvString);

    testPVScalarWithProperties(String("boolean"),pvBoolean);
    testPVScalarWithProperties(String("byte"),pvByte);
    testPVScalarWithProperties(String("short"),pvShort);
    testPVScalarWithProperties(String("int"),pvInt);
    testPVScalarWithProperties(String("long"),pvLong);
    testPVScalarWithProperties(String("ubyte"),pvUByte);
    testPVScalarWithProperties(String("ushort"),pvUShort);
    testPVScalarWithProperties(String("uint"),pvUInt);
    testPVScalarWithProperties(String("ulong"),pvULong);
    testPVScalarWithProperties(String("float"),pvFloat);
    testPVScalarWithProperties(String("double"),pvDouble);
    testPVScalarWithProperties(String("string"),pvString);
    printf("testScalar PASSED\n");
}


static void testScalarArrayCommon(String /*fieldName*/,ScalarType stype)
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
    builder.clear();
    pvStructure->toString(&builder);
    if(debug) printf("%s\n",builder.c_str());
    PVFieldPtr pvField = pvStructure->getSubField("alarm.status");
    testOk1(pvField!=NULL);
}

static void testScalarArray()
{
    if(debug) printf("\ntestScalarArray\n");
    testScalarArrayCommon(String("boolean"),pvBoolean);
    testScalarArrayCommon(String("byte"),pvByte);
    testScalarArrayCommon(String("short"),pvShort);
    testScalarArrayCommon(String("int"),pvInt);
    testScalarArrayCommon(String("long"),pvLong);
    testScalarArrayCommon(String("float"),pvFloat);
    testScalarArrayCommon(String("double"),pvDouble);
    testScalarArrayCommon(String("string"),pvString);
    printf("testScalarArray PASSED\n");
}

static void testRequest()
{
    if(debug) printf("\ntestScalarArray\n");
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
String buffer;
topStructure->toString(&buffer);
cout << buffer.c_str() << endl;
    PVStructurePtr pvTop = pvDataCreate->createPVStructure(topStructure);
buffer.clear();
pvTop->toString(&buffer);
cout << buffer.c_str() << endl;
buffer.clear();
pvTop->getStructure()->toString(&buffer);
cout << buffer.c_str() << endl;
PVStructurePtr xxx = pvTop->getSubField<PVStructure>("record");
buffer.clear();
xxx->toString(&buffer);
cout << buffer.c_str() << endl;
xxx = pvTop->getSubField<PVStructure>("field");
buffer.clear();
xxx->toString(&buffer);
cout << buffer.c_str() << endl;
PVStringPtr pvString = pvTop->getSubField<PVString>("record._options.process");
pvString->put("true");
buffer.clear();
pvTop->toString(&buffer);
cout << buffer.c_str() << endl;
cout << pvTop->dumpValue(cout) << endl;

}

MAIN(testPVData)
{
    testPlan(187);
    fieldCreate = getFieldCreate();
    pvDataCreate = getPVDataCreate();
    standardField = getStandardField();
    standardPVField = getStandardPVField();
    convert = getConvert();
    testCreatePVStructure();
    testPVScalar();
    testScalarArray();
    testRequest();
    return testDone();
}

