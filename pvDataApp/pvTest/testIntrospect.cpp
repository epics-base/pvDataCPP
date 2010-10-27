/* testSimpleStructure.cpp */
/* Author:  Marty Kraimer Date: 2010.09 */

#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>

#include "requester.h"
#include "pvIntrospect.h"
#include "standardField.h"

using namespace epics::pvData;

static FieldCreate * fieldCreate = 0;
static StandardField *standardField = 0;
static String buffer("");


void testSimpleStructure(FILE * fd) {
    fprintf(fd,"\ntestSimpleStructure\n");
    String properties("alarm,timeStamp,display,control,valueAlarm");
    StructureConstPtr ptop = standardField->scalarValue(pvDouble,properties);
    buffer.clear();
    ptop->toString(&buffer);
    fprintf(fd,"%s\n",buffer.c_str());
}

int main(int argc,char *argv[])
{
    char *fileName = 0;
    if(argc>1) fileName = argv[1];
printf("fileName %p\n",fileName);
    FILE * fd = stdout;
    if(fileName!=0 && fileName[0]!=0) {
        fd = fopen(fileName,"w+");
    }
    fieldCreate = getFieldCreate();
    standardField = getStandardField();
    testSimpleStructure(fd);
    return(0);
}

