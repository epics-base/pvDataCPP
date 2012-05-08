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
#include <epicsExit.h>

#include <pv/requester.h>
#include <pv/pvIntrospect.h>
#include <pv/pvData.h>
#include <pv/convert.h>
#include <pv/standardField.h>
#include <pv/standardPVField.h>

using namespace epics::pvData;
using std::tr1::static_pointer_cast;

static FieldCreatePtr fieldCreate;
static PVDataCreatePtr pvDataCreate;
static StandardFieldPtr standardField;
static StandardPVFieldPtr standardPVField;
static ConvertPtr convert;
static String buffer;

static void printOffsets(PVStructurePtr pvStructure,FILE *fd)
{
    fprintf(fd,"%s offset %lu next %lu number %lu\n",
        pvStructure->getFieldName().c_str(),
        (long unsigned)pvStructure->getFieldOffset(),
        (long unsigned)pvStructure->getNextFieldOffset(),
        (long unsigned)pvStructure->getNumberFields());
    PVFieldPtrArray fields = pvStructure->getPVFields();
    int number = pvStructure->getStructure()->getNumberFields();
    for(int i=0; i<number; i++) {
        PVFieldPtr pvField = fields[i];
        if(pvField->getField()->getType()==structure) {
             PVStructurePtr xxx = static_pointer_cast<PVStructure>(pvField);
             printOffsets(xxx,fd);
             continue;
        }
        fprintf(fd,"%s offset %d next %d number %d\n",
            pvField->getFieldName().c_str(),
            pvField->getFieldOffset(),
            pvField->getNextFieldOffset(),
            pvField->getNumberFields());
    }
}

static void testPVAuxInfo(FILE * fd) {
    fprintf(fd,"\ntestPVAuxInfo\n");
    PVStructurePtr pvStructure = standardPVField->scalar(
        pvDouble,"alarm,timeStamp,display,control");
    PVStructurePtr display
        = pvStructure->getStructureField("display");
    assert(display.get()!=NULL);
    PVAuxInfoPtr auxInfo = display->getPVAuxInfo();
    auxInfo->createInfo("factory",pvString);
    auxInfo->createInfo("junk",pvDouble);
    PVScalarPtr pscalar = auxInfo->getInfo(String("factory"));
    assert(pscalar.get()!=0);
    convert->fromString(pscalar,"factoryName");
    pscalar = auxInfo->getInfo("junk");
    assert(pscalar.get()!=0);
    convert->fromString(pscalar,"3.0");
    buffer.clear();
    pvStructure->toString(&buffer);
    fprintf(fd,"%s\n",buffer.c_str());
    // now show field offsets
    printOffsets(pvStructure,fd);
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
    return(0);
}

