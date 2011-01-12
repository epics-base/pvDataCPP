/* testPVdata.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/* Author:  Marty Kraimer Date: 2010.11 */

#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>

#include <epicsAssert.h>

#include "requester.h"
#include "pvIntrospect.h"
#include "pvData.h"
#include "convert.h"
#include "standardField.h"
#include "standardPVField.h"
#include "showConstructDestruct.h"

using namespace epics::pvData;

static FieldCreate * fieldCreate = 0;
static PVDataCreate * pvDataCreate = 0;
static StandardField *standardField = 0;
static StandardPVField *standardPVField = 0;
static Convert *convert = 0;
static String builder("");
static String alarmTimeStamp("alarm,timeStamp");
static String alarmTimeStampValueAlarm("alarm,timeStamp,valueAlarm");
static String allProperties("alarm,timeStamp,display,control,valueAlarm");

static void testAppend(FILE * fd)
{
    FieldConstPtrArray fields = new FieldConstPtr[0];
    PVStructure *pvParent = pvDataCreate->createPVStructure(
        0,String("request"),0,fields);
    PVString* pvStringField = static_cast<PVString*>(
        pvDataCreate->createPVScalar(pvParent, "fieldList", pvString));
    pvStringField->put(String("value,timeStamp"));
    pvParent->appendPVField(pvStringField);
    builder.clear();
    pvParent->toString(&builder);
    fprintf(fd,"%s\n",builder.c_str());
    delete pvParent;
}

static void testPVScalarCommon(FILE * fd,String fieldName,ScalarType stype)
{
    PVScalar *pvScalar = standardPVField->scalar(0,fieldName,stype);
    if(stype==pvBoolean) {
        convert->fromString(pvScalar,String("true"));
    } else {
        convert->fromString(pvScalar,String("10"));
    }
    builder.clear();
    pvScalar->toString(&builder);
    fprintf(fd,"%s\n",builder.c_str());
    delete pvScalar;
}

static void testPVScalarWithProperties(
    FILE * fd,String fieldName,ScalarType stype)
{
    PVStructure *pvStructure = 0;
    bool hasValueAlarm = false;
    bool hasDisplayControl = false;
    switch(stype) {
        case pvBoolean: {
             pvStructure = standardPVField->scalar(
                 0,fieldName,stype,alarmTimeStampValueAlarm);
             hasValueAlarm = true;
             PVBoolean *pvField = pvStructure->getBooleanField(String("value"));
             pvField->put(true);
             break;
        }
        case pvByte: {
             pvStructure = standardPVField->scalar(
                 0,fieldName,stype,allProperties);
             hasValueAlarm = true;
             hasDisplayControl = true;
             PVByte *pvField = pvStructure->getByteField(String("value"));
             pvField->put(127);
             break;
        }
        case pvShort: {
             pvStructure = standardPVField->scalar(
                 0,fieldName,stype,allProperties);
             hasValueAlarm = true;
             hasDisplayControl = true;
             PVShort *pvField = pvStructure->getShortField(String("value"));
             pvField->put(32767);
             break;
        }
        case pvInt: {
             pvStructure = standardPVField->scalar(
                 0,fieldName,stype,allProperties);
             hasValueAlarm = true;
             hasDisplayControl = true;
             PVInt *pvField = pvStructure->getIntField(String("value"));
             pvField->put((int)0x80000000);
             break;
        }
        case pvLong: {
             pvStructure = standardPVField->scalar(
                 0,fieldName,stype,allProperties);
             hasValueAlarm = true;
             hasDisplayControl = true;
             PVLong *pvField = pvStructure->getLongField(String("value"));
             int64 value = 0x80000000;
             value <<= 32;
             value |= 0xffffffff;
             pvField->put(value);
             break;
        }
        case pvFloat: {
             pvStructure = standardPVField->scalar(
                 0,fieldName,stype,allProperties);
             hasValueAlarm = true;
             hasDisplayControl = true;
             PVFloat *pvField = pvStructure->getFloatField(String("value"));
             pvField->put(1.123e8);
             break;
        }
        case pvDouble: {
             pvStructure = standardPVField->scalar(
                 0,fieldName,stype,allProperties);
             hasValueAlarm = true;
             hasDisplayControl = true;
             PVDouble *pvField = pvStructure->getDoubleField(String("value"));
             pvField->put(1.123e35);
             break;
        }
        case pvString: {
             pvStructure = standardPVField->scalar(
                 0,fieldName,stype,alarmTimeStamp);
             PVString *pvField = pvStructure->getStringField(String("value"));
             pvField->put(String("this is a string"));
             break;
        }
    }
    PVLong *seconds = pvStructure->getLongField(
        String("timeStamp.secondsPastEpoch"));
    assert(seconds!=0);
    seconds->put(123456789);
    PVInt *nano = pvStructure->getIntField(String("timeStamp.nanoSeconds"));
    assert(nano!=0);
    nano->put(1000000);
    PVInt *severity = pvStructure->getIntField(String("alarm.severity"));
    assert(severity!=0);
    severity->put(2);
    PVString *message = pvStructure->getStringField(String("alarm.message"));
    assert(message!=0);
    message->put(String("messageForAlarm"));
    if(hasDisplayControl) {
        PVString *desc = pvStructure->getStringField(
            String("display.description"));
        assert(desc!=0);
        desc->put(String("this is a description"));
        PVString *format = pvStructure->getStringField(
            String("display.format"));
        assert(format!=0);
        format->put(String("f10.2"));
        PVString *units = pvStructure->getStringField(
            String("display.units"));
        assert(units!=0);
        units->put(String("SomeUnits"));
        PVDouble *limit = pvStructure->getDoubleField(
            String("display.limit.low"));
        assert(limit!=0);
        limit->put(0.0);
        limit = pvStructure->getDoubleField(
            String("display.limit.high"));
        assert(limit!=0);
        limit->put(10.0);
        limit = pvStructure->getDoubleField(
            String("control.limit.low"));
        assert(limit!=0);
        limit->put(1.0);
        limit = pvStructure->getDoubleField(
            String("control.limit.high"));
        assert(limit!=0);
        limit->put(9.0);
        PVScalar *pvtemp = (PVScalar *)pvStructure->getSubField(
            String("valueAlarm.lowAlarmLimit"));
        assert(pvtemp!=0);
        convert->fromDouble(pvtemp,1.0);
        pvtemp = (PVScalar *)pvStructure->getSubField(
            String("valueAlarm.highAlarmLimit"));
        assert(pvtemp!=0);
        convert->fromDouble(pvtemp,9.0);
        severity = pvStructure->getIntField(
            String("valueAlarm.lowAlarmSeverity"));
        assert(severity!=0);
        severity->put(2);
        severity = pvStructure->getIntField(
            String("valueAlarm.highAlarmSeverity"));
        assert(severity!=0);
        severity->put(2);
        PVBoolean *active = pvStructure->getBooleanField(
            String("valueAlarm.active"));
        assert(active!=0);
        active->put(true);
    }
    builder.clear();
    pvStructure->toString(&builder);
    fprintf(fd,"%s\n",builder.c_str());
    delete pvStructure;
}

static void testPVScalar(FILE * fd) {
    fprintf(fd,"\ntestScalar\n");
    testPVScalarCommon(fd,String("boolean"),pvBoolean);
    testPVScalarCommon(fd,String("byte"),pvByte);
    testPVScalarCommon(fd,String("short"),pvShort);
    testPVScalarCommon(fd,String("int"),pvInt);
    testPVScalarCommon(fd,String("long"),pvLong);
    testPVScalarCommon(fd,String("float"),pvFloat);
    testPVScalarCommon(fd,String("double"),pvDouble);
    testPVScalarCommon(fd,String("string"),pvString);

    testPVScalarWithProperties(fd,String("boolean"),pvBoolean);
    testPVScalarWithProperties(fd,String("byte"),pvByte);
    testPVScalarWithProperties(fd,String("short"),pvShort);
    testPVScalarWithProperties(fd,String("int"),pvInt);
    testPVScalarWithProperties(fd,String("long"),pvLong);
    testPVScalarWithProperties(fd,String("float"),pvFloat);
    testPVScalarWithProperties(fd,String("double"),pvDouble);
    testPVScalarWithProperties(fd,String("string"),pvString);
}

static void testScalarArrayCommon(FILE * fd,String fieldName,ScalarType stype)
{
    PVStructure *pvStructure = standardPVField->scalarArray(
        0,fieldName,stype,alarmTimeStamp);
    PVScalarArray *scalarArray = pvStructure->getScalarArrayField(
        String("value"),stype);
    assert(scalarArray!=0);
    if(stype==pvBoolean) {
        String values[] = {String("true"),String("false"),String("true")};
        convert->fromStringArray(scalarArray, 0,3,values,0);
    } else {
        String values[] = {String("0"),String("1"),String("2")};
        convert->fromStringArray(scalarArray, 0,3,values,0);
    }
    builder.clear();
    pvStructure->toString(&builder);
    fprintf(fd,"%s\n",builder.c_str());
    delete pvStructure;
}

static void testScalarArray(FILE * fd) {
    fprintf(fd,"\ntestScalarArray\n");
    testScalarArrayCommon(fd,String("boolean"),pvBoolean);
    testScalarArrayCommon(fd,String("byte"),pvByte);
    testScalarArrayCommon(fd,String("short"),pvShort);
    testScalarArrayCommon(fd,String("int"),pvInt);
    testScalarArrayCommon(fd,String("long"),pvLong);
    testScalarArrayCommon(fd,String("float"),pvFloat);
    testScalarArrayCommon(fd,String("double"),pvDouble);
    testScalarArrayCommon(fd,String("string"),pvString);
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
    standardPVField = getStandardPVField();
    convert = getConvert();
    testAppend(fd);
    testPVScalar(fd);
    testScalarArray(fd);
    getShowConstructDestruct()->showDeleteStaticExit(fd);
    return(0);
}

