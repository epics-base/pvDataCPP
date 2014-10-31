/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
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

#include <epicsUnitTest.h>
#include <testMain.h>

#include <pv/timeStamp.h>

using namespace epics::pvData;

static bool debug = false;

void testTimeStampInternal()
{
    testOk1(nanoSecPerSec==1000000000);
    TimeStamp current;
    current.getCurrent();
    printf("current %lli %i milliSec %lli\n",
        (long long)current.getSecondsPastEpoch(),
        current.getNanoseconds(),
        (long long)current.getMilliseconds());
    time_t tt;
    current.toTime_t(tt);
    struct tm ctm;
    memcpy(&ctm,localtime(&tt),sizeof(struct tm));
    printf(
        "%4.4d.%2.2d.%2.2d %2.2d:%2.2d:%2.2d %d isDst %s\n",
        ctm.tm_year+1900,ctm.tm_mon + 1,ctm.tm_mday,
        ctm.tm_hour,ctm.tm_min,ctm.tm_sec,
        current.getNanoseconds(),
        (ctm.tm_isdst==0) ? "false" : "true");
    tt = time(&tt);
    current.fromTime_t(tt);
    printf("fromTime_t\ncurrent %lli %i milliSec %lli\n",
        (long long)current.getSecondsPastEpoch(),
        current.getNanoseconds(),
        (long long)current.getMilliseconds());
    current.toTime_t(tt);
    memcpy(&ctm,localtime(&tt),sizeof(struct tm));
    printf(
        "%4.4d.%2.2d.%2.2d %2.2d:%2.2d:%2.2d %d isDst %s\n",
        ctm.tm_year+1900,ctm.tm_mon + 1,ctm.tm_mday,
        ctm.tm_hour,ctm.tm_min,ctm.tm_sec,
        current.getNanoseconds(),
        (ctm.tm_isdst==0) ? "false" : "true");
    TimeStamp right;
    TimeStamp left;
    right.put(current.getSecondsPastEpoch(),current.getNanoseconds());
    left.put(current.getSecondsPastEpoch(),current.getNanoseconds());
    double diff;
    diff = TimeStamp::diff(left,right);
    if(debug) printf("diff %e\n",diff);
    testOk1(diff==0.0);
    testOk1((left==right));
    testOk1(!(left!=right));
    testOk1((left<=right));
    testOk1(!(left<right));
    testOk1((left>=right));
    testOk1(!(left>right));
    left.put(current.getSecondsPastEpoch()+1,current.getNanoseconds());
    diff = TimeStamp::diff(left,right);
    if(debug) printf("diff %e\n",diff);
    testOk1(diff==1.0);
    testOk1(!(left==right));
    testOk1((left!=right));
    testOk1(!(left<=right));
    testOk1(!(left<right));
    testOk1((left>=right));
    testOk1((left>right));
    left.put(current.getSecondsPastEpoch()-1,current.getNanoseconds());
    diff = TimeStamp::diff(left,right);
    if(debug) printf("diff %e\n",diff);
    testOk1(diff==-1.0);
    testOk1(!(left==right));
    testOk1((left!=right));
    testOk1((left<=right));
    testOk1((left<right));
    testOk1(!(left>=right));
    testOk1(!(left>right));
    left.put(current.getSecondsPastEpoch(),current.getNanoseconds()-nanoSecPerSec);
    diff = TimeStamp::diff(left,right);
    if(debug) printf("diff %e\n",diff);
    testOk1(diff==-1.0);
    testOk1(!(left==right));
    testOk1((left!=right));
    testOk1((left<=right));
    testOk1((left<right));
    testOk1(!(left>=right));
    testOk1(!(left>right));
    left.put(current.getSecondsPastEpoch(),current.getNanoseconds()-1);
    diff = TimeStamp::diff(left,right);
    if(debug) printf("diff %e\n",diff);
    testOk1(diff<0.0);
    testOk1(!(left==right));
    testOk1((left!=right));
    testOk1((left<=right));
    testOk1((left<right));
    testOk1(!(left>=right));
    testOk1(!(left>right));
    left.put(current.getSecondsPastEpoch(),current.getNanoseconds());
    left += .1;
    diff = TimeStamp::diff(left,right);
    if(debug) printf("diff %e\n",diff);
    left.put(current.getSecondsPastEpoch(),current.getNanoseconds());
    int64 inc = -1;
    left += inc;
    diff = TimeStamp::diff(left,right);
    testOk1(diff==-1.0);
    printf("PASSED\n");
}

MAIN(testTimeStamp)
{
    testPlan(37);
    testDiag("Tests timeStamp");
    testTimeStampInternal();
    return testDone();
}
