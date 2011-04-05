/* monitor.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef MONITOR_H
#define MONITOR_H

#include <status.h>
#include <destroyable.h>
#include <sharedPtr.h>
#include <pvData.h>

namespace epics { namespace pvData { 

class MonitorElement {
public:
    MonitorElement(){}
    virtual ~MonitorElement(){}
    virtual PVStructure* getPVStructure() = 0;
    virtual BitSet* getChangedBitSet() = 0;
    virtual BitSet* getOverrunBitSet() = 0;
};

class Monitor : public virtual Destroyable{
    public:
    Monitor(){}
    virtual ~Monitor(){}
    virtual Status start() = 0;
    virtual Status stop() = 0;
    virtual MonitorElement* poll() = 0;
    virtual void release(MonitorElement* monitorElement) = 0;
};

class MonitorRequester : public virtual Requester {
    public:
    MonitorRequester() {}
    virtual ~MonitorRequester() {}
    virtual void monitorConnect(
        const Status &status, Monitor* monitor, StructureConstPtr structure) = 0;
    virtual void monitorEvent(Monitor* monitor) = 0;
    virtual void unlisten(Monitor* monitor) = 0;
};

}}
#endif  /* MONITOR_H */
