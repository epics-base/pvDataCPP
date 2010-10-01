/* testSimpleStructure.cpp */
/* Author:  Marty Kraimer Date: 2010.09 */

#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "requester.h"
#include "pvIntrospect.h"
#include "pvData.h"
#include "standardField.h"

using namespace epics::pvData;

static FieldCreate * pfieldCreate = 0;
static PVDataCreate *pvDataCreate = 0;
static String buffer("");


void testSimpleStructure() {
    printf("\ntestSimpleStructure\n");
    String secondsPastEpoch("secondsPastEpoch");
    String nanoSeconds("nanoSeconds");
    String timeStamp("timeStamp");
    String severity("severity");
    String message("message");
    String alarm("alarm");
    String value("value");
    String top("top");

    ScalarConstPtr pseconds = pfieldCreate->createScalar(
        secondsPastEpoch,pvLong);
    ScalarConstPtr pnano = pfieldCreate->createScalar(
        nanoSeconds,pvInt);
    FieldConstPtrArray fields = new FieldConstPtr[2];
    fields[0] = pseconds;
    fields[1] = pnano;
    StructureConstPtr ptimeStamp = pfieldCreate->createStructure(
        timeStamp,2,fields);
    ScalarConstPtr pseverity = pfieldCreate->createScalar(
        severity,pvInt);
    ScalarConstPtr pmessage = pfieldCreate->createScalar(
        message,pvString);
    fields = new FieldConstPtr[2];
    fields[0] = pseverity;
    fields[1] = pmessage;
    StructureConstPtr palarm = pfieldCreate->createStructure(
        alarm,2,fields);
    ScalarConstPtr pvalue = pfieldCreate->createScalar(
        value,pvDouble);
    fields = new FieldConstPtr[3];
    fields[0] = ptimeStamp;
    fields[1] = palarm;
    fields[2] = pvalue;
    StructureConstPtr ptop = pfieldCreate->createStructure(
        top,3,fields);
buffer.clear();
ptop->toString(&buffer);
printf("%s\n",buffer.c_str());
    PVStructure *pvStructure = pvDataCreate->createPVStructure( 0, ptop);
    buffer.clear();
    pvStructure->toString(&buffer);
    printf("%s\n",buffer.c_str());
}

int main(int argc,char *argv[])
{
    pfieldCreate = getFieldCreate();
    pvDataCreate = getPVDataCreate();
    testSimpleStructure();
    return(0);
}

