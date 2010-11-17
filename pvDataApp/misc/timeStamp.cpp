/* timeStamp.cpp */
#include <epicsTime.h>
#include "noDefaultMethods.h"
#include "pvType.h"
#include "timeStamp.h"

namespace epics { namespace pvData { 

static uint64 TS_EPOCH_SEC_PAST_1970=7305*86400;

TimeStamp::TimeStamp(uint64 secondsPastEpoch,uint32 nanoSeconds)
: secondsPastEpoch(secondsPastEpoch),nanoSeconds(nanoSeconds)
{}

TimeStamp::~TimeStamp() {}

int64 TimeStamp::getEpicsSecondsPastEpoch()
{
    return secondsPastEpoch - TS_EPOCH_SEC_PAST_1970;
}

void TimeStamp::put(uint64 seconds,uint32 nano)
{
    secondsPastEpoch = seconds;
    nanoSeconds = nano;    
}

void TimeStamp::getCurrent()
{
    epicsTimeStamp epicsTime;
    epicsTimeGetCurrent(&epicsTime);
    secondsPastEpoch = epicsTime.secPastEpoch;
    secondsPastEpoch += TS_EPOCH_SEC_PAST_1970;
    nanoSeconds = epicsTime.nsec;
}

double TimeStamp::diffInSeconds(TimeStamp *left,TimeStamp *right)
{
    double diff = left->secondsPastEpoch - right->secondsPastEpoch;
    int64 nano =left->nanoSeconds - right->nanoSeconds;
    diff += ((double)nano)/1e9;
    return diff;
}

int64 TimeStamp::getMilliseconds()
{
    return secondsPastEpoch*1000 + nanoSeconds/1000000;
}

void TimeStamp::put(int64 milliseconds)
{
    secondsPastEpoch = milliseconds/1000;
    nanoSeconds = (milliseconds%1000)*1000000;
}

  

}}
