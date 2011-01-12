/* testPVAuxInfo.cpp */
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
static String buffer("");

static void printOffsets(PVStructure *pvStructure,FILE *fd)
{
    fprintf(fd,"%s offset %d next %d number %d\n",
        pvStructure->getField()->getFieldName().c_str(),
        pvStructure->getFieldOffset(),
        pvStructure->getNextFieldOffset(),
        pvStructure->getNumberFields());
    PVFieldPtrArray fields = pvStructure->getPVFields();
    int number = pvStructure->getStructure()->getNumberFields();
    for(int i=0; i<number; i++) {
        PVField *pvField = fields[i];
        if(pvField->getField()->getType()==structure) {
             printOffsets((PVStructure *)pvField,fd);
             continue;
        }
        fprintf(fd,"%s offset %d next %d number %d\n",
            pvField->getField()->getFieldName().c_str(),
            pvField->getFieldOffset(),
            pvField->getNextFieldOffset(),
            pvField->getNumberFields());
    }
}

static void testPVAuxInfo(FILE * fd) {
    fprintf(fd,"\ntestPVAuxInfo\n");
    PVStructure * pvStructure = standardPVField->scalar(
        0,String("value"),pvDouble,String("alarm,timeStamp,display,conytrol"));
    PVStructure *displayLimit = pvStructure->getStructureField(
        String("display.limit"));
    assert(displayLimit!=0);
    PVAuxInfo *auxInfo = displayLimit->getPVAuxInfo();
    auxInfo->createInfo(String("factory"),pvString);
    auxInfo->createInfo(String("junk"),pvDouble);
    PVScalar *pscalar = auxInfo->getInfo(String("factory"));
    assert(pscalar!=0);
    convert->fromString(pscalar,String("factoryName"));
    pscalar = auxInfo->getInfo(String("junk"));
    assert(pscalar!=0);
    convert->fromString(pscalar,String("3.0"));
    buffer.clear();
    pvStructure->toString(&buffer);
    fprintf(fd,"%s\n",buffer.c_str());
    // now show field offsets
    printOffsets(pvStructure,fd);
    delete pvStructure;
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
    testPVAuxInfo(fd);
    getShowConstructDestruct()->showDeleteStaticExit(fd);
    return(0);
}

