/* testPVDataPyCAPI.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/* Author:  Marty Kraimer Date: 2010.11 */

#include <pv/pvDataPyCreateTest.h>

static bool debug = false;


static void testSimple(FILE * fd)
{
     void *pvTopPyAddr = pvPyCreateScalar(10,"alarm,timeStamp");
     void *top = pvPyGetTop(pvTopPyAddr);
     void *pvStructurePy = pvPyGetPVStructurePyPtr(top);
     pvPyDumpPVStructurePy(pvStructurePy);
     pvPyDeleteTop(pvTopPyAddr);
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

