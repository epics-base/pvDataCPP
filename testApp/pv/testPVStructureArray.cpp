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

using namespace epics::pvData;

static FieldCreatePtr fieldCreate;
static PVDataCreatePtr pvDataCreate;
static StandardFieldPtr standardField;
static StandardPVFieldPtr standardPVField;
static String buffer;

StructureConstPtr getPowerSupplyStructure() {
    String properties("alarm");
    FieldConstPtrArray  fields;
    StringArray fieldNames;
    fields.reserve(3);
    fieldNames.reserve(3);
    fieldNames.push_back("voltage");
    fieldNames.push_back("power");
    fieldNames.push_back("current");
    fields.push_back(standardField->scalar(pvDouble,properties));
    fields.push_back(standardField->scalar(pvDouble,properties));
    fields.push_back(standardField->scalar(pvDouble,properties));
    StructureConstPtr structure = fieldCreate->createStructure(
        fieldNames,fields);
    return structure;
}

void testPowerSupplyArray(FILE * fd) {
    PVStructurePtr powerSupplyArrayStruct = standardPVField->structureArray(
        getPowerSupplyStructure(),String("alarm,timeStamp"));
    PVStructureArrayPtr powerSupplyArray =
        powerSupplyArrayStruct->getStructureArrayField(String("value"));
    assert(powerSupplyArray.get()!=NULL);
    int offset = powerSupplyArray->append(5);
    powerSupplyArray->setLength(offset);
    buffer.clear();
    powerSupplyArrayStruct->toString(&buffer);
    fprintf(fd,"after append 5\n%s\n",buffer.c_str());
    powerSupplyArray->remove(0,2);
    buffer.clear();
    powerSupplyArrayStruct->toString(&buffer);
    fprintf(fd,"after remove(0,2)\n%s\n",buffer.c_str());
    powerSupplyArray->remove(2,1);
    buffer.clear();
    powerSupplyArrayStruct->toString(&buffer);
    fprintf(fd,"after remove 0,1,3%s\n",buffer.c_str());
    powerSupplyArray->compress();
    buffer.clear();
    powerSupplyArrayStruct->toString(&buffer);
    fprintf(fd,"after compress%s\n",buffer.c_str());
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
    return(0);
}

