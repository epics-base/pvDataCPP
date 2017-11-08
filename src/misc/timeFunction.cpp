/* timeFunction.cpp */
/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */
/**
 *  @author mrk
 */
#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>

#include <epicsTime.h>

// Suppress deprecation warnings for the implementation
#include <compilerDependencies.h>
#undef EPICS_DEPRECATED
#define EPICS_DEPRECATED

#define epicsExportSharedSymbols
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
