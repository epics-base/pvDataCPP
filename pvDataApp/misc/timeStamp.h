/* timeStamp.h */
#ifndef TIMESTAMP_H
#define TIMESTAMP_H
#include "noDefaultMethods.h"
#include "pvType.h"

namespace epics { namespace pvData { 

class TimeStamp : private NoDefaultMethods {
public:
    TimeStamp(uint64 secondsPastEpoch,uint32 nanoSeconds);
    ~TimeStamp();
    int64 getSecondsPastEpoch(){return secondsPastEpoch;}
    int32 getNanoSeconds() {return nanoSeconds;}
    int64 getEpicsSecondsPastEpoch();
    void put(uint64 secondsPastEpoch,uint32 nanoSeconds);
    void getCurrent();
    static double diffInSeconds(TimeStamp *left,TimeStamp *right);
    // milliseconds since epoch
    int64 getMilliseconds();
    void put(int64 milliseconds);
private:
    int64 secondsPastEpoch;
    int32 nanoSeconds;
};
  

}}
#endif  /* TIMESTAMP_H */
