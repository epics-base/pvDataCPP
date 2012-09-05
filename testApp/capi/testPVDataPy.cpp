/* testPVDataPy.cpp */
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

#include <pv/pvDataPy.h>
#include <pv/standardPVField.h>

using namespace epics::pvData;
using std::tr1::static_pointer_cast;

static StandardPVFieldPtr standardPVField = getStandardPVField();
static String alarmTimeStamp("alarm,timeStamp");

static bool debug = false;


static void testSimple(FILE * fd)
{
     PVStructurePtr pv0 = standardPVField->scalar(
         pvDouble,alarmTimeStamp);
     PVTopPyPtr pvTop = PVTopPy::createTop(pv0);
     PVStructurePyPtr const & pvStructurePyPtr = pvTop->getPVStructurePy();
     PVStructurePtr const & pvStructurePtr = pvStructurePyPtr->getPVStructurePtr();
     String buffer;
     pvStructurePtr->toString(&buffer);
     printf("%s\n",buffer.c_str());
}


int main(int argc,char *argv[])
{
    char *fileName = 0;
    if(argc>1) fileName = argv[1];
    FILE * fd = stdout;
    if(fileName!=0 && fileName[0]!=0) {
        fd = fopen(fileName,"w+");
    }
    testSimple(fd);
    return(0);
}

