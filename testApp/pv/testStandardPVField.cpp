/* testStandardPVField.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/* Author:  Marty Kraimer Date: 2012.08 */

#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>

#include <epicsAssert.h>
#include <epicsExit.h>

#include <pv/requester.h>
#include <pv/pvIntrospect.h>
#include <pv/pvData.h>
#include <pv/convert.h>
#include <pv/standardField.h>
#include <pv/standardPVField.h>

using namespace epics::pvData;
using std::tr1::static_pointer_cast;

static bool debug = false;

static PVDataCreatePtr pvDataCreate = getPVDataCreate();
static StandardFieldPtr standardField = getStandardField();
static StandardPVFieldPtr standardPVField = getStandardPVField();
static String builder("");

static void print(String name)
{
    if(debug) printf("\n%s\n%s\n",name.c_str(),builder.c_str());
}

int main(int argc,char *argv[])
{
    PVStructurePtr pvStructure = standardPVField->scalar(pvDouble,
        "alarm,timeStamp,display,control,valueAlarm");
    PVDoublePtr pvValue = pvStructure->getDoubleField("value");
    pvValue->put(10.0);
    PVIntPtr pvSeverity = pvStructure->getIntField("alarm.severity");
    pvSeverity->put(2);
    PVStringPtr pvMessage = pvStructure->getStringField("alarm.message");
    pvMessage->put("test message");
    builder.clear();
    pvStructure->toString(&builder);
    print("scalarTest");
    pvStructure = standardPVField->scalar(pvBoolean,"alarm,timeStamp,valueAlarm");
    builder.clear();
    pvStructure->toString(&builder);
    print("booleanTest");
    StringArray choices;
    choices.reserve(3);
    choices.push_back("one");
    choices.push_back("two");
    choices.push_back("three");
    pvStructure = standardPVField->enumerated(choices, "alarm,timeStamp,valueAlarm");
    builder.clear();
    pvStructure->toString(&builder);
    print("enumeratedTest");
    pvStructure = standardPVField->scalarArray(pvBoolean,"alarm,timeStamp");
    builder.clear();
    pvStructure->toString(&builder);
    print("scalarArrayTest");
    StructureConstPtr structure = standardField->scalar(pvDouble, "alarm,timeStamp");
    pvStructure = standardPVField->structureArray(structure,"alarm,timeStamp");
    size_t num = 2;
    PVStructurePtrArray pvStructures;
    pvStructures.reserve(num);
    for(size_t i=0; i<num; i++) {
        pvStructures.push_back(
            pvDataCreate->createPVStructure(structure));
    }
    PVStructureArrayPtr pvStructureArray = pvStructure->getStructureArrayField("value");
    pvStructureArray->put(0, num, pvStructures, 0);
    builder.clear();
    pvStructure->toString(&builder);
    print("structureArrayTest");
    printf("PASSED\n");
    return(0);
}

