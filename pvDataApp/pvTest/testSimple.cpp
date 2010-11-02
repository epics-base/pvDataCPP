/* testSimpleStructure.cpp */
/* Author:  Marty Kraimer Date: 2010.09 */

#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>

#include <epicsAssert.h>

#include "requester.h"
#include "pvIntrospect.h"
#include "pvData.h"
#include "standardField.h"
#include "standardPVField.h"

using namespace epics::pvData;

static FieldCreate * fieldCreate = 0;
static PVDataCreate * pvDataCrete = 0;
static StandardField *standardField = 0;
static StandardPVField *standardPVField = 0;
static String buffer("");


void testSimpleStructure(FILE * fd) {
    int initialTotalReferences,finalTotalReferences;
    fprintf(fd,"\ntestSimpleStructure\n");
    String properties("alarm,timeStamp,display,control,valueAlarm");
    initialTotalReferences = Field::getTotalReferenceCount();
    PVStructure *ptop = standardPVField->scalar(
        0,String(""),pvDouble,properties);
    buffer.clear();
    ptop->toString(&buffer);
    fprintf(fd,"%s\n",buffer.c_str());
    finalTotalReferences = Field::getTotalReferenceCount();
    printf("before delete initialTotalReferences %d finalTotalReferences %d\n",
        initialTotalReferences,finalTotalReferences);
    delete ptop;
    finalTotalReferences = Field::getTotalReferenceCount();
    printf("after initialTotalReferences %d finalTotalReferences %d\n",
        initialTotalReferences,finalTotalReferences);
    assert(initialTotalReferences==finalTotalReferences);
}

void testPowerSupply(FILE * fd) {
    int initialTotalReferences,finalTotalReferences;
    fprintf(fd,"\ntestPowerSupply\n");
    initialTotalReferences = Field::getTotalReferenceCount();
    PVStructure *ptop = standardPVField->powerSupply(0);
    buffer.clear();
    ptop->toString(&buffer);
    fprintf(fd,"%s\n",buffer.c_str());
    finalTotalReferences = Field::getTotalReferenceCount();
    printf("before delete initialTotalReferences %d finalTotalReferences %d\n",
        initialTotalReferences,finalTotalReferences);
    delete ptop;
    finalTotalReferences = Field::getTotalReferenceCount();
    printf("after initialTotalReferences %d finalTotalReferences %d\n",
        initialTotalReferences,finalTotalReferences);
    assert(initialTotalReferences==finalTotalReferences);
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
    pvDataCrete = getPVDataCreate();
    standardField = getStandardField();
    standardPVField = getStandardPVField();
    testSimpleStructure(fd);
    testPowerSupply(fd);
    long long totalConstruct = Field::getTotalConstruct();
    long long totalDestruct = Field::getTotalDestruct();
    int totalReference = Field::getTotalReferenceCount();
    printf("Field:   totalConstruct %lli totalDestruct %lli totalReferenceCount %i\n",
        totalConstruct,totalDestruct,totalReference);
    assert(totalConstruct==(totalDestruct+totalReference));
    totalConstruct = PVField::getTotalConstruct();
    totalDestruct = PVField::getTotalDestruct();
    printf("PVField: totalConstruct %lli totalDestruct %lli\n",
        totalConstruct,totalDestruct);
    assert(totalConstruct==totalDestruct);
    return(0);
}

