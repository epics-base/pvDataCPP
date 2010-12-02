/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/*
 * testTimeStamp.cpp
 *
 *  Created on: 2010.11
 *      Author: Marty Kraimer
 */

#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <list>

#include <epicsAssert.h>

#include "timeStamp.h"

using namespace epics::pvData;

void testTimeStamp(FILE *fd,FILE *auxfd)
{
    assert(nanoSecPerSec==1000000000);
    TimeStamp current;
    current.getCurrent();
    fprintf(auxfd,"current %lli %i milliSec %lli\n",
        current.getSecondsPastEpoch(),
        current.getNanoSeconds(),
        current.getMilliseconds());
    time_t tt;
    current.toTime_t(tt);
    struct tm ctm;
    memcpy(&ctm,localtime(&tt),sizeof(struct tm));
    fprintf(auxfd,
        "%4.4d.%2.2d.%2.2d %2.2d:%2.2d:%2.2d %d nanoSeconds isDst %s\n",
        ctm.tm_year+1900,ctm.tm_mon + 1,ctm.tm_mday,
        ctm.tm_hour,ctm.tm_min,ctm.tm_sec,
        current.getNanoSeconds(),
        (ctm.tm_isdst==0) ? "false" : "true");
    tt = time(&tt);
    current.fromTime_t(tt);
    fprintf(auxfd,"fromTime_t\ncurrent %lli %i milliSec %lli\n",
        current.getSecondsPastEpoch(),
        current.getNanoSeconds(),
        current.getMilliseconds());
    current.toTime_t(tt);
    memcpy(&ctm,localtime(&tt),sizeof(struct tm));
    fprintf(auxfd,
        "%4.4d.%2.2d.%2.2d %2.2d:%2.2d:%2.2d %d nanoSeconds isDst %s\n",
        ctm.tm_year+1900,ctm.tm_mon + 1,ctm.tm_mday,
        ctm.tm_hour,ctm.tm_min,ctm.tm_sec,
        current.getNanoSeconds(),
        (ctm.tm_isdst==0) ? "false" : "true");
    TimeStamp right;
    TimeStamp left;
    right.put(current.getSecondsPastEpoch(),current.getNanoSeconds());
    left.put(current.getSecondsPastEpoch(),current.getNanoSeconds());
    double diff;
    diff = TimeStamp::diff(left,right);
    fprintf(fd,"diff %e\n",diff);
    assert(diff==0.0);
    assert((left==right));
    assert(!(left!=right));
    assert((left<=right));
    assert(!(left<right));
    assert((left>=right));
    assert(!(left>right));
    left.put(current.getSecondsPastEpoch()+1,current.getNanoSeconds());
    diff = TimeStamp::diff(left,right);
    fprintf(fd,"diff %e\n",diff);
    assert(!(left==right));
    assert((left!=right));
    assert(!(left<=right));
    assert(!(left<right));
    assert((left>=right));
    assert((left>right));
    left.put(current.getSecondsPastEpoch()-1,current.getNanoSeconds());
    diff = TimeStamp::diff(left,right);
    fprintf(fd,"diff %e\n",diff);
    assert(diff==-1.0);
    assert(!(left==right));
    assert((left!=right));
    assert((left<=right));
    assert((left<right));
    assert(!(left>=right));
    assert(!(left>right));
    left.put(current.getSecondsPastEpoch(),current.getNanoSeconds()-nanoSecPerSec);
    diff = TimeStamp::diff(left,right);
    fprintf(fd,"diff %e\n",diff);
    assert(diff==-1.0);
    assert(!(left==right));
    assert((left!=right));
    assert((left<=right));
    assert((left<right));
    assert(!(left>=right));
    assert(!(left>right));
    left.put(current.getSecondsPastEpoch(),current.getNanoSeconds()-1);
    diff = TimeStamp::diff(left,right);
    fprintf(fd,"diff %e\n",diff);
    assert(diff<0.0);
    assert(!(left==right));
    assert((left!=right));
    assert((left<=right));
    assert((left<right));
    assert(!(left>=right));
    assert(!(left>right));
    left.put(current.getSecondsPastEpoch(),current.getNanoSeconds());
    left += .1;
    diff = TimeStamp::diff(left,right);
    fprintf(fd,"diff %e\n",diff);
    left.put(current.getSecondsPastEpoch(),current.getNanoSeconds());
    int64 inc = -1;
    left += inc;
    diff = TimeStamp::diff(left,right);
    assert(diff==-1.0);
}

int main(int argc, char *argv[]) {
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
    testTimeStamp(fd,auxfd);
    return (0);
}
