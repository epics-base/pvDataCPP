/* timeStamp.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>

#include <epicsTime.h>
#include "noDefaultMethods.h"
#include "pvType.h"
#include "timeStamp.h"

namespace epics { namespace pvData { 

int32 milliSecPerSec = 1000;
int32 microSecPerSec = milliSecPerSec*milliSecPerSec;
int32 nanoSecPerSec = milliSecPerSec*microSecPerSec;
int64 posixEpochAtEpicsEpoch = POSIX_TIME_AT_EPICS_EPOCH;

TimeStamp::TimeStamp(int64 secondsPastEpoch,int32 nanoSeconds)
: secondsPastEpoch(secondsPastEpoch),nanoSeconds(nanoSeconds)
{}


void TimeStamp::getCurrent()
{
    epicsTimeStamp epicsTime;
    epicsTimeGetCurrent(&epicsTime);
    secondsPastEpoch = epicsTime.secPastEpoch;
    secondsPastEpoch += posixEpochAtEpicsEpoch;
    nanoSeconds = epicsTime.nsec;
}

double TimeStamp::toSeconds() const
{
    double value = secondsPastEpoch;
    double nano = nanoSeconds;
    value += nano/1e9;
    return value;
}

int64 TimeStamp::diffInt(TimeStamp const & left,TimeStamp  const&right )
{
    int64 sl = left.secondsPastEpoch;
    int32 nl = left.nanoSeconds;
    int64 sr = right.secondsPastEpoch;
    int32 nr = right.nanoSeconds;
    int64 sdiff = sl - sr;
    sdiff *= nanoSecPerSec;
    sdiff += nl - nr;
    return sdiff;
}

bool TimeStamp::operator==(TimeStamp const &right) const
{
    int64 sdiff = diffInt(*this,right);
    if(sdiff==0) return true;
    return false;
}

bool TimeStamp::operator!=(TimeStamp const &right) const
{
    int64 sdiff = diffInt(*this,right);
    if(sdiff!=0) return true;
    return false;
}

bool TimeStamp::operator<=(TimeStamp const &right) const
{
    int64 sdiff = diffInt(*this,right);
    if(sdiff<=0) return true;
    return false;
}

bool TimeStamp::operator< (TimeStamp const &right) const
{
    int64 sdiff = diffInt(*this,right);
    if(sdiff<0) return true;
    return false;
}

bool TimeStamp::operator>=(TimeStamp const &right) const
{
    int64 sdiff = diffInt(*this,right);
    if(sdiff>=0) return true;
    return false;
}

bool TimeStamp::operator>(TimeStamp const &right) const
{
    int64 sdiff = diffInt(*this,right);
    if(sdiff>0) return true;
    return false;
}

double TimeStamp::diff(TimeStamp const & a,TimeStamp const & b)
{
    double result = a.secondsPastEpoch - b.secondsPastEpoch;
    result += (a.nanoSeconds - b.nanoSeconds)/1e9;
    return result;
}


TimeStamp & TimeStamp::operator+=(int64 seconds)
{
    secondsPastEpoch += seconds;
    return *this;
}

TimeStamp & TimeStamp::operator-=(int64 seconds)
{
    secondsPastEpoch -= seconds;
    return *this;
}

TimeStamp & TimeStamp::operator+=(double seconds)
{
    int64 secs = seconds;
    int64 nano = (seconds - secs)*1e9;
    nanoSeconds += nano;
    if(nanoSeconds>nanoSecPerSec) {
        nanoSeconds -= nanoSecPerSec;
        secondsPastEpoch += 1;
    } else if(nanoSeconds<-nanoSecPerSec) {
        nanoSeconds += -nanoSecPerSec;
        secondsPastEpoch -= 1;
    }
    secondsPastEpoch += secs;
    return *this;
}

TimeStamp & TimeStamp::operator-=(double seconds)
{
   return operator+=(-seconds);
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
