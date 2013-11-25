/* testPVAuxInfo.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/* Author:  Marty Kraimer Date: 2010.11 */

#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>

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

static bool debug = false;

static FieldCreatePtr fieldCreate;
static PVDataCreatePtr pvDataCreate;
static StandardFieldPtr standardField;
static StandardPVFieldPtr standardPVField;
static ConvertPtr convert;
static String buffer;

static void printOffsets(PVStructurePtr pvStructure)
{
    printf("%s offset %lu next %lu number %lu\n",
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
             printOffsets(xxx);
             continue;
        }
        printf("%s offset %lli next %lli number %lli\n",
            pvField->getFieldName().c_str(),
            (long long)pvField->getFieldOffset(),
            (long long)pvField->getNextFieldOffset(),
            (long long)pvField->getNumberFields());
    }
}

static void testPVAuxInfo()
{
    if(debug) printf("\ntestPVAuxInfo\n");
    PVStructurePtr pvStructure = standardPVField->scalar(
        pvDouble,"alarm,timeStamp,display,control");
    PVStructurePtr display
        = pvStructure->getStructureField("display");
    testOk1(display.get()!=NULL);
    PVAuxInfoPtr auxInfo = display->getPVAuxInfo();
    auxInfo->createInfo("factory",pvString);
    auxInfo->createInfo("junk",pvDouble);
    PVScalarPtr pscalar = auxInfo->getInfo(String("factory"));
    testOk1(pscalar.get()!=0);
    convert->fromString(pscalar,"factoryName");
    pscalar = auxInfo->getInfo("junk");
    testOk1(pscalar.get()!=0);
    convert->fromString(pscalar,"3.0");
    buffer.clear();
    pvStructure->toString(&buffer);
    if(debug) printf("%s\n",buffer.c_str());
    // now show field offsets
    if(debug) printOffsets(pvStructure);
    printf("testPVAuxInfo PASSED\n");
}

MAIN(testPVAuxInfo)
{
    testPlan(3);
    fieldCreate = getFieldCreate();
    pvDataCreate = getPVDataCreate();
    standardField = getStandardField();
    standardPVField = getStandardPVField();
    convert = getConvert();
    testPVAuxInfo();
    return testDone();
}

