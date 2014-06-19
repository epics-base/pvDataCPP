/* testStandardPVField.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/* Author:  Marty Kraimer Date: 2012.08 */

#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>
#include <sstream>

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
using std::string;

static bool debug = false;

static PVDataCreatePtr pvDataCreate = getPVDataCreate();
static StandardFieldPtr standardField = getStandardField();
static StandardPVFieldPtr standardPVField = getStandardPVField();

static void print(const string& name, PVFieldPtr const & f)
{
    if(debug) {
        std::cout << std::endl << name << std::endl << *f << std::endl;
    }
}

MAIN(testStandardPVField)
{
    testPlan(1);
    PVStructurePtr pvStructure = standardPVField->scalar(pvDouble,
        "alarm,timeStamp,display,control,valueAlarm");
    PVDoublePtr pvValue = pvStructure->getDoubleField("value");
    pvValue->put(10.0);
    PVIntPtr pvSeverity = pvStructure->getIntField("alarm.severity");
    pvSeverity->put(2);
    PVStringPtr pvMessage = pvStructure->getStringField("alarm.message");
    pvMessage->put("test message");
    print("scalarTest", pvStructure);
    pvStructure = standardPVField->scalar(pvBoolean,"alarm,timeStamp,valueAlarm");
    print("booleanTest", pvStructure);
    StringArray choices;
    choices.reserve(3);
    choices.push_back("one");
    choices.push_back("two");
    choices.push_back("three");
    pvStructure = standardPVField->enumerated(choices, "alarm,timeStamp,valueAlarm");
    print("enumeratedTest", pvStructure);
    pvStructure = standardPVField->scalarArray(pvBoolean,"alarm,timeStamp");
    print("scalarArrayTest", pvStructure);
    StructureConstPtr structure = standardField->scalar(pvDouble, "alarm,timeStamp");
    pvStructure = standardPVField->structureArray(structure,"alarm,timeStamp");
    size_t num = 2;
    PVStructureArray::svector pvStructures(num);
    for(size_t i=0; i<num; i++) {
        pvStructures[i]=
            pvDataCreate->createPVStructure(structure);
    }
    PVStructureArrayPtr pvStructureArray = pvStructure->getStructureArrayField("value");
    pvStructureArray->replace(freeze(pvStructures));
    print("structureArrayTest", pvStructure);
    testPass("testStandardPVField");
    return testDone();
}

