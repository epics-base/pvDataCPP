/* testPVAppend.cpp */
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

#include "requester.h"
#include "pvIntrospect.h"
#include "pvData.h"
#include "convert.h"
#include "standardField.h"
#include "standardPVField.h"
#include "CDRMonitor.h"

using namespace epics::pvData;

static FieldCreate * fieldCreate = 0;
static PVDataCreate * pvDataCreate = 0;
static StandardField *standardField = 0;
static StandardPVField *standardPVField = 0;
static Convert *convert = 0;
static String builder("");
static String alarmTimeStamp("alarm,timeStamp");
static String alarmTimeStampValueAlarm("alarm,timeStamp,valueAlarm");
static String allProperties("alarm,timeStamp,display,control,valueAlarm");


static void testAppendMore(FILE * fd)
{
    PVStructure* pvStructure = pvDataCreate->createPVStructure(
        0,"top", 0);
    PVStructure* pvChild1 = pvDataCreate->createPVStructure(
          pvStructure, "child1", 0);
    PVString *pvStringField = static_cast<PVString*>(
        pvDataCreate->createPVScalar(pvChild1,"value", pvString));
    pvStringField->put("bla");
    pvChild1->appendPVField(pvStringField);
    pvStructure->appendPVField(pvChild1);
    PVStructure* pvChild2 = pvDataCreate->createPVStructure(
         pvStructure, "child2", 0);
    pvStringField = static_cast<PVString*>(
        pvDataCreate->createPVScalar(pvChild2,"value", pvString));
    pvStringField->put("bla");
    pvChild2->appendPVField(pvStringField);
    pvStructure->appendPVField(pvChild2);
    builder.clear();
    pvStructure->toString(&builder);
    fprintf(fd,"%s\n",builder.c_str());
    PVField *pvField = pvStructure->getSubField(String("child1.value"));
    pvField->message(String("test message"),infoMessage);
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
    testAppendMore(fd);
    epicsExitCallAtExits();
    CDRMonitor::get().show(fd);
    return(0);
}

