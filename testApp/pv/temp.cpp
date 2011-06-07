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

#include <pv/requester.h>
#include <pv/pvIntrospect.h>
#include <pv/pvData.h>
#include <pv/convert.h>
#include <pv/standardField.h>
#include <pv/standardPVField.h>
#include <pv/CDRMonitor.h>

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

static void testAppendSimple(FILE * fd)
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
}

static void testAppendMore(FILE * fd)
{
    PVStructure* pvStructure = pvDataCreate->createPVStructure(
        0,"parent", 0);
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
    delete pvStructure;
}

static void append2(PVStructure* pvStructure,
    const char *oneName,const char *twoName,
    const char *oneValue,const char *twoValue)
{
     PVField* array[2];
     PVString *pvStringField = static_cast<PVString*>(
        pvDataCreate->createPVScalar(0,oneName, pvString));
     pvStringField->put(oneValue);
     array[0] = pvStringField;
     pvStringField = static_cast<PVString*>(
        pvDataCreate->createPVScalar(0,twoName, pvString));
     pvStringField->put(twoValue);
     array[1] = pvStringField;
     pvStructure->appendPVFields(2,array);
}
static void testAppends(FILE * fd)
{
    PVField** array = new PVField*[2];
    PVStructure* pvChild = pvDataCreate->createPVStructure(
          0, "child1", 0);
    append2(pvChild,"Joe","Mary","Good Guy","Good Girl");
    array[0] = pvChild;
    pvChild = pvDataCreate->createPVStructure(
          0, "child2", 0);
    append2(pvChild,"Bill","Jane","Bad Guy","Bad Girl");
    array[1] = pvChild;
    PVStructure* pvStructure = pvDataCreate->createPVStructure(
        0,"parent", 2,array);
    builder.clear();
    pvStructure->toString(&builder);
    fprintf(fd,"%s\n",builder.c_str());
    PVField *pvField = pvStructure->getSubField("child2.Bill");
    assert(pvField!=0);
    bool ok = pvField->renameField("Joe");
    assert(ok);
    builder.clear();
    pvStructure->toString(&builder);
    fprintf(fd,"%s\n",builder.c_str());
    pvField->getParent()->removePVField("Joe");
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
    //testAppendSimple(fd);
    //testAppendMore(fd);
    testAppends(fd);
    epicsExitCallAtExits();
    CDRMonitor::get().show(fd);
    return(0);
}

