/* testBitSetUtil.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/* Author:  Marty Kraimer Date: 2013.05 */

#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>

#include <epicsUnitTest.h>
#include <testMain.h>

#include <pv/requester.h>
#include <pv/bitSetUtil.h>
#include <pv/convert.h>
#include <pv/standardField.h>
#include <pv/standardPVField.h>

using namespace epics::pvData;
using std::tr1::static_pointer_cast;

static bool debug = false;

static FieldCreatePtr fieldCreate;
static PVDataCreatePtr pvDataCreate;
static StandardFieldPtr standardField;
static StandardPVFieldPtr standardPVField;
static ConvertPtr convert;
static String builder("");

static void test()
{
    if(debug) printf("\ntestBitSetUtil\n");
    StringArray fieldNames;
    PVFieldPtrArray pvFields;
    fieldNames.reserve(5);
    pvFields.reserve(5);
    fieldNames.push_back("timeStamp");
    fieldNames.push_back("alarm");
    fieldNames.push_back("voltage");
    fieldNames.push_back("power");
    fieldNames.push_back("current");
    pvFields.push_back(
        pvDataCreate->createPVStructure(standardField->timeStamp()));
    pvFields.push_back(
        pvDataCreate->createPVStructure(standardField->alarm()));
    pvFields.push_back(
        pvDataCreate->createPVStructure(
            standardField->scalar(pvDouble,"alarm")));
    pvFields.push_back(
        pvDataCreate->createPVStructure(
            standardField->scalar(pvDouble,"alarm")));
    pvFields.push_back(
        pvDataCreate->createPVStructure(
            standardField->scalar(pvDouble,"alarm")));
    PVStructurePtr pvs =  pvDataCreate->createPVStructure(
         fieldNames,pvFields);
    builder.clear();
    pvs->toString(&builder);
    if(debug) printf("pvs\n%s\n",builder.c_str());
    int32 nfields = (int32)pvs->getNumberFields();
    BitSetPtr bitSet = BitSet::create(nfields);
    for(int32 i=0; i<nfields; i++) bitSet->set(i);
    builder.clear();
    bitSet->toString(&builder);
    if(debug) printf("bitSet\n%s\n",builder.c_str());
    BitSetUtil::compress(bitSet,pvs);
    builder.clear();
    bitSet->toString(&builder);
    if(debug) printf("bitSet\n%s\n",builder.c_str());
    bitSet->clear();
    PVFieldPtr pvField = pvs->getSubField("timeStamp");
    int32 offsetTimeStamp = (int32)pvField->getFieldOffset();
    pvField = pvs->getSubField("timeStamp.secondsPastEpoch");
    int32 offsetSeconds = (int32)pvField->getFieldOffset();
    pvField = pvs->getSubField("timeStamp.nanoSeconds");
    int32 offsetNano = (int32)pvField->getFieldOffset();
    pvField = pvs->getSubField("timeStamp.userTag");
    int32 offsetUserTag = (int32)pvField->getFieldOffset();
    bitSet->set(offsetSeconds);
    BitSetUtil::compress(bitSet,pvs);
    testOk1(bitSet->get(offsetSeconds)==true);
    bitSet->set(offsetNano);
    bitSet->set(offsetUserTag);
    builder.clear();
    bitSet->toString(&builder);
    if(debug) printf("bitSet\n%s\n",builder.c_str());
    BitSetUtil::compress(bitSet,pvs);
    testOk1(bitSet->get(offsetSeconds)==false);
    testOk1(bitSet->get(offsetTimeStamp)==true);
    builder.clear();
    bitSet->toString(&builder);
    if(debug) printf("bitSet\n%s\n",builder.c_str());
    bitSet->clear();

    pvField = pvs->getSubField("current");
    int32 offsetCurrent = (int32)pvField->getFieldOffset();
    pvField = pvs->getSubField("current.value");
    int32 offsetValue = (int32)pvField->getFieldOffset();
    pvField = pvs->getSubField("current.alarm");
    int32 offsetAlarm = (int32)pvField->getFieldOffset();
    pvField = pvs->getSubField("current.alarm.severity");
    int32 offsetSeverity = (int32)pvField->getFieldOffset();
    pvField = pvs->getSubField("current.alarm.status");
    int32 offsetStatus = (int32)pvField->getFieldOffset();
    pvField = pvs->getSubField("current.alarm.message");
    int32 offsetMessage = (int32)pvField->getFieldOffset();
    bitSet->set(offsetValue);
    bitSet->set(offsetSeverity);
    bitSet->set(offsetStatus);
    bitSet->set(offsetMessage);
    builder.clear();
    bitSet->toString(&builder);
    if(debug) printf("bitSet\n%s\n",builder.c_str());
    BitSetUtil::compress(bitSet,pvs);
    builder.clear();
    bitSet->toString(&builder);
    if(debug) printf("bitSet\n%s\n",builder.c_str());
    testOk1(bitSet->get(offsetCurrent)==true);
    bitSet->clear();
    bitSet->set(offsetSeverity);
    bitSet->set(offsetStatus);
    bitSet->set(offsetMessage);
    builder.clear();
    bitSet->toString(&builder);
    if(debug) printf("bitSet\n%s\n",builder.c_str());
    BitSetUtil::compress(bitSet,pvs);
    builder.clear();
    bitSet->toString(&builder);
    if(debug) printf("bitSet\n%s\n",builder.c_str());
    testOk1(bitSet->get(offsetAlarm)==true);
    bitSet->clear();
    printf("testBitSetUtil PASSED\n");
}

MAIN(testBitSetUtil)
{
    testPlan(5);
    fieldCreate = getFieldCreate();
    pvDataCreate = getPVDataCreate();
    standardField = getStandardField();
    standardPVField = getStandardPVField();
    convert = getConvert();
    test();
    return testDone();
}

