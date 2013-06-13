/* testOperators.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/* Author:  Matej Sekoranja Date: 2013.02 */

#include <iostream>

#include <pv/pvData.h>
#include <pv/standardField.h>
#include <pv/standardPVField.h>

#include <epicsAssert.h>

using namespace epics::pvData;

static PVDataCreatePtr pvDataCreate = getPVDataCreate();
static StandardFieldPtr standardField = getStandardField();
static StandardPVFieldPtr standardPVField = getStandardPVField();

int main(int, char **)
{
    PVStructurePtr pvStructure = standardPVField->scalar(pvDouble,
        "alarm,timeStamp,display,control,valueAlarm");

    const double testDV = 12.8;

    PVDoublePtr pvValue = pvStructure->getDoubleField("value");
    *pvValue <<= testDV;

    double dv;
    *pvValue >>= dv;
    assert(testDV == dv);


    const std::string testSV = "test message";

    PVStringPtr pvMessage = pvStructure->getStringField("alarm.message");
    *pvMessage <<= testSV;

    std::string sv;
    *pvMessage >>= sv;
    assert(testSV == sv);

    //
    // to stream tests
    //

    std::cout << *pvValue << std::endl;
    std::cout << *pvMessage << std::endl;
    std::cout << *pvStructure << std::endl;


    StringArray choices;
    choices.reserve(3);
    choices.push_back("one");
    choices.push_back("two");
    choices.push_back("three");
    pvStructure = standardPVField->enumerated(choices, "alarm,timeStamp,valueAlarm");
    std::cout << *pvStructure << std::endl;



    pvStructure = standardPVField->scalarArray(pvDouble,"alarm,timeStamp");
    std::cout << *pvStructure << std::endl;
    
    double values[] = { 1.1, 2.2, 3.3 };
    PVDoubleArrayPtr darray = std::tr1::dynamic_pointer_cast<PVDoubleArray>(pvStructure->getScalarArrayField("value", pvDouble));
    darray->put(0, 3, values, 0);
    std::cout << *darray << std::endl;
    std::cout << format::array_at(1) << *darray << std::endl;


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
    std::cout << *pvStructure << std::endl;

    return 0;
}

