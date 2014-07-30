/* testPVUnion.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/* Author:  Marty Kraimer Date: 2014.07 */

#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>

#include <epicsUnitTest.h>
#include <testMain.h>

#include <pv/pvIntrospect.h>
#include <pv/pvData.h>
#include <pv/convert.h>
#include <pv/standardField.h>
#include <pv/standardPVField.h>
#include <pv/timeStamp.h>
#include <pv/pvTimeStamp.h>

using namespace epics::pvData;
using std::tr1::static_pointer_cast;
using std::string;
using std::cout;
using std::endl;

static bool debug = false;

static FieldCreatePtr fieldCreate = getFieldCreate();
static PVDataCreatePtr pvDataCreate = getPVDataCreate();
static StandardFieldPtr standardField = getStandardField();
static StandardPVFieldPtr standardPVField = getStandardPVField();
static ConvertPtr convert = getConvert();

static void testPVUnion()
{
    if(debug) 
        std::cout << std::endl << "testPVUnion" << std::endl;
    PVStructurePtr pvStructure = pvDataCreate->createPVStructure(
        standardField->regUnion(
            fieldCreate->createFieldBuilder()->
                add("doubleValue", pvDouble)->
                add("intValue", pvInt)->
                add("timeStamp",standardField->timeStamp())->
                createUnion(),
            "alarm,timeStamp"));
    PVUnionPtr pvValue = pvStructure->getSubField<PVUnion>("value");
    PVStructurePtr pvTime= pvValue->select<PVStructure>(2);
    TimeStamp timeStamp;
    timeStamp.getCurrent();
    PVTimeStamp pvTimeStamp;
    pvTimeStamp.attach(pvTime);
    pvTimeStamp.set(timeStamp);
    testOk1(
        pvTime->getSubField<PVLong>("secondsPastEpoch")->get()
        ==
        pvValue->get<PVStructure>()->getSubField<PVLong>("secondsPastEpoch")->get()
    );
    PVDoublePtr pvDouble = pvValue->select<PVDouble>("doubleValue");
    pvDouble->put(1e5);
    testOk1(pvDouble->get()==pvValue->get<PVDouble>()->get());
    PVIntPtr pvInt = pvValue->select<PVInt>("intValue");
    pvInt->put(15);
    testOk1(pvInt->get()==pvValue->get<PVInt>()->get());
    std::cout << "testPVUnion PASSED" << std::endl;
}

static void testPVUnionArray()
{
    if(debug) 
        std::cout << std::endl << "testPVUnion" << std::endl;
    PVStructurePtr pvStructure = pvDataCreate->createPVStructure(
        standardField->unionArray(
            fieldCreate->createFieldBuilder()->
                add("doubleValue", pvDouble)->
                add("intValue", pvInt)->
                add("timeStamp",standardField->timeStamp())->
                createUnion(),
            "alarm,timeStamp"));
    PVUnionArrayPtr pvValue = pvStructure->getSubField<PVUnionArray>("value");
    size_t num = 3;
    shared_vector<PVUnionPtr> unions(3);
    for(size_t i=0; i<num; ++i)
    {
        unions[i] = pvDataCreate->createPVUnion(pvValue->getUnionArray()->getUnion());
    }
    unions[0]->select("doubleValue");
    unions[1]->select("intValue");
    unions[2]->select("timeStamp");
    PVDoublePtr pvDouble = unions[0]->get<PVDouble>();
    pvDouble->put(1.235);
    PVIntPtr pvInt = unions[1]->get<PVInt>();
    pvInt->put(5);
    PVStructurePtr pvTime = unions[2]->get<PVStructure>();
    TimeStamp timeStamp;
    timeStamp.getCurrent();
    PVTimeStamp pvTimeStamp;
    pvTimeStamp.attach(pvTime);
    pvTimeStamp.set(timeStamp);
    pvValue->replace(freeze(unions));
    shared_vector<const PVUnionPtr> sharedUnions = pvValue->view();
    testOk1(pvDouble->get()==sharedUnions[0]->get<PVDouble>()->get());
    testOk1(pvInt->get()==sharedUnions[1]->get<PVInt>()->get());
    testOk1(
        pvTime->getSubField<PVLong>("secondsPastEpoch")->get()
        ==
        sharedUnions[2]->get<PVStructure>()->getSubField<PVLong>("secondsPastEpoch")->get()
    );
    std::cout << "testPVUnionArray PASSED" << std::endl;
}


MAIN(testPVData)
{
    testPlan(6);
    testPVUnion();
    testPVUnionArray();
    return testDone();
}

