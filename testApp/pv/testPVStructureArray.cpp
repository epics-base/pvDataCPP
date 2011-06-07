/* testPVStructureArray.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/* Author:  Marty Kraimer Date: 2010.10 */

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
#include <pv/standardField.h>
#include <pv/standardPVField.h>
#include <pv/CDRMonitor.h>

using namespace epics::pvData;

static FieldCreate * fieldCreate = 0;
static PVDataCreate * pvDataCreate = 0;
static StandardField *standardField = 0;
static StandardPVField *standardPVField = 0;
static String buffer("");

StructureConstPtr getPowerSupplyStructure() {
    String properties("alarm");
    FieldConstPtrArray powerSupply = new FieldConstPtr[3];
    powerSupply[0] = standardField->scalar(
        String("voltage"),pvDouble,properties);
    powerSupply[1] = standardField->scalar(
        String("power"),pvDouble,properties);
    powerSupply[2] = standardField->scalar(
        String("current"),pvDouble,properties);
    StructureConstPtr structure = standardField->structure(
        String("powerSupply"),3,powerSupply);
    return structure;
}

void testPowerSupplyArray(FILE * fd) {
    PVStructure* powerSupplyArrayStruct = standardPVField->structureArray(
        0,"powerSupply",getPowerSupplyStructure(),String("alarm,timeStamp"));
    PVStructureArray * powerSupplyArray =
        powerSupplyArrayStruct->getStructureArrayField(String("value"));
    assert(powerSupplyArray!=0);
    int offset = powerSupplyArray->append(5);
    powerSupplyArray->setLength(offset);
    buffer.clear();
    powerSupplyArrayStruct->toString(&buffer);
    fprintf(fd,"after append 5\n%s\n",buffer.c_str());
    powerSupplyArray->remove(0,2);
    powerSupplyArray->remove(3,1);
    buffer.clear();
    powerSupplyArrayStruct->toString(&buffer);
    fprintf(fd,"after remove 0,1,3%s\n",buffer.c_str());
    powerSupplyArray->compress();
    buffer.clear();
    powerSupplyArrayStruct->toString(&buffer);
    fprintf(fd,"after compress%s\n",buffer.c_str());
    delete powerSupplyArrayStruct;
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
    testPowerSupplyArray(fd);
    epicsExitCallAtExits();
    CDRMonitor::get().show(fd);
    return(0);
}

