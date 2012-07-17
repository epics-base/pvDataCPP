/* timeFunction.cpp */
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

#include <pv/pvType.h>
#include <pv/timeStamp.h>
#include <pv/timeFunction.h>

namespace epics { namespace pvData { 

TimeFunction::TimeFunction(TimeFunctionRequesterPtr const &requester)
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
