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

static void testAppend(FILE * fd)
{
    FieldConstPtrArray fields = new FieldConstPtr[0];
    PVStructure *pvParent = pvDataCreate->createPVStructure(
        0,String("request"),0,fields);
    PVString* pvStringField = static_cast<PVString*>(
        pvDataCreate->createPVScalar(pvParent, "fieldList", pvString));
    pvStringField->put(String("value,timeStamp"));
    pvParent->appendPVField(pvStringField);
    builder.clear();
    pvParent->toString(&builder);
    fprintf(fd,"%s\n",builder.c_str());
    pvStringField = static_cast<PVString*>(
        pvDataCreate->createPVScalar(pvParent, "extra", pvString));
    pvStringField->put(String("junk"));
    pvParent->appendPVField(pvStringField);
    builder.clear();
    pvParent->toString(&builder);
    fprintf(fd,"%s\n",builder.c_str());
    delete pvParent;
    PVStructure* pvStructure = pvDataCreate->createPVStructure(
        0,"parent", 0);
    PVStructure* pvChild1 = pvDataCreate->createPVStructure(
          pvStructure, "child1", 0);
    pvStringField = static_cast<PVString*>(
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
    testAppend(fd);
    getShowConstructDestruct()->showDeleteStaticExit(fd);
    return(0);
}

