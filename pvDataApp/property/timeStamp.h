/* timeStamp.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <ctime>
#include <epicsTime.h>

#include <pv/pvType.h>

#include <sharelib.h>

namespace epics { namespace pvData { 

epicsShareExtern int32 milliSecPerSec;
epicsShareExtern int32 microSecPerSec;
epicsShareExtern int32 nanoSecPerSec;
epicsShareExtern int64 posixEpochAtEpicsEpoch;

class epicsShareClass TimeStamp {
public:
    TimeStamp() 
    :secondsPastEpoch(0), nanoSeconds(0), userTag(0) {}
    TimeStamp(int64 secondsPastEpoch,int32 nanoSeconds = 0,int32 userTag = 0);
    //default constructors and destructor are OK
    //This class should not be extended
    void normalize();
    void fromTime_t(const time_t &);
    void toTime_t(time_t &) const;
    int64 getSecondsPastEpoch() const {return secondsPastEpoch;}
    int64 getEpicsSecondsPastEpoch() const {
        return secondsPastEpoch - posixEpochAtEpicsEpoch;
    }
    int32 getNanoSeconds() const  {return nanoSeconds;}
    int32 getUserTag() const {return userTag;}
    void setUserTag(int userTag) {this->userTag = userTag;}
    void put(int64 secondsPastEpoch,int32 nanoSeconds = 0) {
        this->secondsPastEpoch = secondsPastEpoch;
        this->nanoSeconds = nanoSeconds;
        normalize();
    }
    void put(int64 milliseconds);
    void getCurrent();
    double toSeconds() const ;
    bool operator==(TimeStamp const &) const;
    bool operator!=(TimeStamp const &) const;
    bool operator<=(TimeStamp const &) const;
    bool operator< (TimeStamp const &) const;
    bool operator>=(TimeStamp const &) const;
    bool operator> (TimeStamp const &) const;
    static double diff(TimeStamp const & a,TimeStamp const & b);
    TimeStamp & operator+=(int64 seconds);
    TimeStamp & operator-=(int64 seconds);
    TimeStamp & operator+=(double seconds);
    TimeStamp & operator-=(double seconds);
    int64 getMilliseconds(); // milliseconds since epoch
private:
    static int64 diffInt(TimeStamp const &left,TimeStamp const  &right );
    int64 secondsPastEpoch;
    int32 nanoSeconds;
    int32 userTag;
};
  

}}
#endif  /* TIMESTAMP_H */
