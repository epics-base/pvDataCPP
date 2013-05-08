/* testPVdata.cpp */
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
#include <cstring>
#include <cstdio>

#include <epicsAssert.h>
#include <epicsExit.h>

#include <pv/requester.h>
#include <pv/pvIntrospect.h>
#include <pv/pvData.h>
#include <pv/convert.h>
#include <pv/standardField.h>
#include <pv/standardPVField.h>
#include <pv/alarm.h>
#include <pv/control.h>
#include <pv/display.h>
#include <pv/timeStamp.h>
#include <pv/pvAlarm.h>
#include <pv/pvControl.h>
#include <pv/pvDisplay.h>
#include <pv/pvEnumerated.h>
#include <pv/pvTimeStamp.h>

using namespace epics::pvData;

static bool debug = false;

static FieldCreatePtr fieldCreate;
static PVDataCreatePtr pvDataCreate;
static StandardFieldPtr standardField;
static StandardPVFieldPtr standardPVField;
static ConvertPtr convert;
static String builder;
static String alarmTimeStamp("alarm,timeStamp");
static String allProperties("alarm,timeStamp,display,control");

static PVStructurePtr doubleRecord;
static PVStructurePtr enumeratedRecord;

static void createRecords(FILE * fd,FILE */*auxfd*/)
{
    doubleRecord = standardPVField->scalar(pvDouble,allProperties);
    if(debug) {
        builder.clear();
        doubleRecord->toString(&builder);
        fprintf(fd,"doubleRecord\n%s\n",builder.c_str());
    }
    StringArray choices;
    choices.reserve(4);
    choices.push_back("1");
    choices.push_back("2");
    choices.push_back("3");
    choices.push_back("4");
    enumeratedRecord = standardPVField->enumerated(choices,alarmTimeStamp);
    if(debug) {
        builder.clear();
        enumeratedRecord->toString(&builder);
        fprintf(fd,"enumeratedRecord\n%s\n",builder.c_str());
    }
}

static void printRecords(FILE * fd,FILE */*auxfd*/)
{
    fprintf(fd,"doubleRecord\n");
    builder.clear();
    doubleRecord->toString(&builder);
    fprintf(fd,"%s\n",builder.c_str());
    fprintf(fd,"enumeratedRecord\n");
    builder.clear();
    enumeratedRecord->toString(&builder);
    fprintf(fd,"%s\n",builder.c_str());
}

static void testAlarm(FILE * fd,FILE */*auxfd*/)
{
    if(debug) fprintf(fd,"testAlarm\n");
    Alarm alarm;
    PVAlarm pvAlarm; 
    bool result;
    PVFieldPtr pvField = doubleRecord->getSubField(String("alarm"));
    if(pvField.get()==NULL) {
        printf("testAlarm ERROR did not find field alarm\n");
        return;
    }
    result = pvAlarm.attach(pvField);
    assert(result);
    Alarm al;
    al.setMessage(String("testMessage"));
    al.setSeverity(majorAlarm);
    al.setStatus(clientStatus);
    result = pvAlarm.set(al);
    assert(result);
    pvAlarm.get(alarm);
    assert(al.getMessage().compare(alarm.getMessage())==0);
    assert(al.getSeverity()==alarm.getSeverity());
    assert(al.getStatus()==alarm.getStatus());
    String message = alarm.getMessage();
    String severity = (*AlarmSeverityFunc::getSeverityNames())[alarm.getSeverity()];
    String status = (*AlarmStatusFunc::getStatusNames())[alarm.getStatus()];
    if(debug) {
        fprintf(fd," message %s severity %s status %s\n",
            message.c_str(),severity.c_str(),status.c_str());
    }
    fprintf(fd,"testAlarm PASSED\n");
}

static void testTimeStamp(FILE * fd,FILE *auxfd)
{
    if(debug) fprintf(fd,"testTimeStamp\n");
    TimeStamp timeStamp;
    PVTimeStamp pvTimeStamp; 
    bool result;
    PVFieldPtr pvField = doubleRecord->getSubField(String("timeStamp"));
    if(pvField.get()==NULL) {
        printf("testTimeStamp ERROR did not find field timeStamp\n");
        return;
    }
    result = pvTimeStamp.attach(pvField);
    assert(result);
    TimeStamp ts;
    ts.getCurrent();
    ts.setUserTag(32);
    result = pvTimeStamp.set(ts);
    assert(result);
    pvTimeStamp.get(timeStamp);
    assert(ts.getSecondsPastEpoch()==timeStamp.getSecondsPastEpoch());
    assert(ts.getNanoSeconds()==timeStamp.getNanoSeconds());
    assert(ts.getUserTag()==timeStamp.getUserTag());
    time_t tt;
    timeStamp.toTime_t(tt);
    struct tm ctm;
    memcpy(&ctm,localtime(&tt),sizeof(struct tm));
    if(debug) {
        fprintf(auxfd,
            "%4.4d.%2.2d.%2.2d %2.2d:%2.2d:%2.2d %d nanoSeconds isDst %s userTag %d\n",
            ctm.tm_year+1900,ctm.tm_mon + 1,ctm.tm_mday,
            ctm.tm_hour,ctm.tm_min,ctm.tm_sec,
            timeStamp.getNanoSeconds(),
            (ctm.tm_isdst==0) ? "false" : "true",
            timeStamp.getUserTag());
    }
    timeStamp.put(0,0);
    pvTimeStamp.set(timeStamp);
    fprintf(fd,"testTimeStamp PASSED\n");
}

static void testControl(FILE * fd,FILE */*auxfd*/)
{
    if(debug) fprintf(fd,"testControl\n");
    Control control;
    PVControl pvControl; 
    bool result;
    PVFieldPtr pvField = doubleRecord->getSubField(String("control"));
    if(pvField.get()==NULL) {
        printf("testControl ERROR did not find field control\n");
        return;
    }
    result = pvControl.attach(pvField);
    assert(result);
    Control cl;
    cl.setLow(1.0);
    cl.setHigh(10.0);
    result = pvControl.set(cl);
    assert(result);
    pvControl.get(control);
    assert(cl.getLow()==control.getLow());
    assert(cl.getHigh()==control.getHigh());
    double low = control.getLow();
    double high = control.getHigh();
    if(debug) fprintf(fd," low %f high %f\n",low,high);
    fprintf(fd,"testControl PASSED\n");
}

static void testDisplay(FILE * fd,FILE */*auxfd*/)
{
    if(debug) fprintf(fd,"testDisplay\n");
    Display display;
    PVDisplay pvDisplay; 
    bool result;
    PVFieldPtr pvField = doubleRecord->getSubField(String("display"));
    if(pvField.get()==NULL) {
        printf("testDisplay ERROR did not find field display\n");
        return;
    }
    result = pvDisplay.attach(pvField);
    assert(result);
    Display dy;
    dy.setLow(-10.0);
    dy.setHigh(-1.0);
    dy.setDescription(String("testDescription"));
    dy.setFormat(String("%f10.0"));
    dy.setUnits(String("volts"));
    result = pvDisplay.set(dy);
    assert(result);
    pvDisplay.get(display);
    assert(dy.getLow()==display.getLow());
    assert(dy.getHigh()==display.getHigh());
    assert(dy.getDescription().compare(display.getDescription())==0);
    assert(dy.getFormat().compare(display.getFormat())==0);
    assert(dy.getUnits().compare(display.getUnits())==0);
    double low = display.getLow();
    double high = display.getHigh();
    if(debug) fprintf(fd," low %f high %f\n",low,high);
    fprintf(fd,"testDisplay PASSED\n");
}

static void testEnumerated(FILE * fd,FILE */*auxfd*/)
{
    if(debug) fprintf(fd,"testEnumerated\n");
    PVEnumerated pvEnumerated; 
    bool result;
    PVFieldPtr pvField = enumeratedRecord->getSubField(String("value"));
    if(pvField.get()==NULL) {
        printf("testEnumerated ERROR did not find field enumerated\n");
        return;
    }
    result = pvEnumerated.attach(pvField);
    assert(result);
    int32 index = pvEnumerated.getIndex();
    String choice = pvEnumerated.getChoice();
    PVStringArray::const_svector choices = pvEnumerated.getChoices();
    int32 numChoices = pvEnumerated.getNumberChoices();
    if(debug) {
        fprintf(fd,"index %d choice %s choices",index,choice.c_str());
        for(int i=0; i<numChoices; i++ ) fprintf(fd," %s",choices[i].c_str());
        fprintf(fd,"\n");
    }
    pvEnumerated.setIndex(2);
    index = pvEnumerated.getIndex();
    choice = pvEnumerated.getChoice();
    if(debug) fprintf(fd,"index %d choice %s\n",index,choice.c_str());
    fprintf(fd,"testEnumerated PASSED\n");
}

int main(int argc,char *argv[])
{
    char *fileName = 0;
    if(argc>1) fileName = argv[1];
    FILE * fd = stdout;
    if(fileName!=0 && fileName[0]!=0) {
        fd = fopen(fileName,"w+");
    }
    char *auxFileName = 0;
    if(argc>2) auxFileName = argv[2];
    FILE *auxfd = stdout;
    if(auxFileName!=0 && auxFileName[0]!=0) {
        auxfd = fopen(auxFileName,"w+");
    }
    fieldCreate = getFieldCreate();
    pvDataCreate = getPVDataCreate();
    standardField = getStandardField();
    standardPVField = getStandardPVField();
    convert = getConvert();
    createRecords(fd,auxfd);
    testAlarm(fd,auxfd);
    testTimeStamp(fd,auxfd);
    testControl(fd,auxfd);
    testDisplay(fd,auxfd);
    testEnumerated(fd,auxfd);
    if(debug) printRecords(fd,auxfd);
    fprintf(fd,"ALL PASSED\n");
    return(0);
}

