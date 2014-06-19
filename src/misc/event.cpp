/* event.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>
#include <stdexcept>

#include <memory>
#include <vector>
#include <epicsThread.h>
#include <epicsMutex.h>
#include <epicsEvent.h>

#define epicsExportSharedSymbols
#include <pv/noDefaultMethods.h>
#include <pv/pvType.h>
#include <pv/lock.h>
#include <pv/event.h>

using std::string;

namespace epics { namespace pvData { 


Event::~Event() {
    epicsEventDestroy(id);
    id = 0;
}


Event::Event(bool full)
: id(epicsEventCreate(full?epicsEventFull : epicsEventEmpty)),
  alreadyOn("already on list")
{
}

void Event::signal()
{
    if(id==0) throw std::logic_error(string("event was deleted"));
    epicsEventSignal(id);
}

bool Event::wait ()
{
    if(id==0) throw std::logic_error(string("event was deleted"));
    epicsEventWaitStatus status = epicsEventWait(id);
    return status==epicsEventWaitOK ? true : false;
}

bool Event::wait ( double timeOut )
{
    if(id==0) throw std::logic_error(string("event was deleted"));
    epicsEventWaitStatus status = epicsEventWaitWithTimeout(id,timeOut);
    return status==epicsEventWaitOK ? true : false;
}

bool Event::tryWait ()
{
    if(id==0) throw std::logic_error(string("event was deleted"));
    epicsEventWaitStatus status = epicsEventTryWait(id);
    return status==epicsEventWaitOK ? true : false;
}

}}
