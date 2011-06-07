/* timeFunction.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <pv/noDefaultMethods.h>
#include <pv/pvType.h>
#include <pv/timeStamp.h>

namespace epics { namespace pvData { 

class TimeFunctionRequester {
public:
    virtual void function() = 0;
};

class TimeFunction : private NoDefaultMethods {
public:
    TimeFunction(TimeFunctionRequester *requester);
    ~TimeFunction();
    double timeCall();
private:
    TimeFunctionRequester *requester;
};
  
TimeFunction::TimeFunction(TimeFunctionRequester *requester)
: requester(requester) {}


TimeFunction::~TimeFunction() {}

double TimeFunction::timeCall()
{
    TimeStamp startTime;
    TimeStamp endTime;
    double perCall = 0.0;
    long ntimes = 1;
    while(true) {
        startTime.getCurrent();
        for(long i=0; i<ntimes; i++) requester->function();
        endTime.getCurrent();
        double diff = TimeStamp::diff(endTime,startTime);
        if(diff>=1.0) {
            perCall = diff/(double)ntimes;
            break;
        }
        ntimes *= 2;
    }
    return perCall;

}

}}
