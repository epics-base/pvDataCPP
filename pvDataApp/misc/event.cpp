/* event.cpp */
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
#include <stdexcept>

#include <memory>
#include <vector>
#include <epicsThread.h>
#include <epicsMutex.h>
#include <epicsEvent.h>
#include "noDefaultMethods.h"
#include "pvType.h"
#include "lock.h"
#include "event.h"
#include "CDRMonitor.h"

namespace epics { namespace pvData { 

PVDATA_REFCOUNT_MONITOR_DEFINE(event);
static String alreadyOn("already on list");

Event::~Event() {
    epicsEventDestroy(id);
    id = 0;
    PVDATA_REFCOUNT_MONITOR_DESTRUCT(event);
}


Event::Event(bool full)
: id(epicsEventCreate(full?epicsEventFull : epicsEventEmpty))
{
    PVDATA_REFCOUNT_MONITOR_CONSTRUCT(event);
}

void Event::signal()
{
    if(id==0) throw std::logic_error(String("event was deleted"));
    epicsEventSignal(id);
}

bool Event::wait ()
{
    if(id==0) throw std::logic_error(String("event was deleted"));
    epicsEventWaitStatus status = epicsEventWait(id);
    return status==epicsEventWaitOK ? true : false;
}

bool Event::wait ( double timeOut )
{
    if(id==0) throw std::logic_error(String("event was deleted"));
    epicsEventWaitStatus status = epicsEventWaitWithTimeout(id,timeOut);
    return status==epicsEventWaitOK ? true : false;
}

bool Event::tryWait ()
{
    if(id==0) throw std::logic_error(String("event was deleted"));
    epicsEventWaitStatus status = epicsEventTryWait(id);
    return status==epicsEventWaitOK ? true : false;
}

}}
