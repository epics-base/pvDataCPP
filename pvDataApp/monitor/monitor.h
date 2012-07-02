/* monitor.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef MONITOR_H
#define MONITOR_H

#include <pv/status.h>
#include <pv/destroyable.h>
#include <pv/pvData.h>
#include <pv/sharedPtr.h>
#include <pv/bitSet.h>

namespace epics { namespace pvData { 

class MonitorElement;
typedef std::tr1::shared_ptr<MonitorElement> MonitorElementPtr;
typedef std::vector<MonitorElementPtr> MonitorElementPtrArray;

class Monitor;
typedef std::tr1::shared_ptr<Monitor> MonitorPtr;


/**
 * Class instance representing monitor element.
 * @author mrk
 */
class MonitorElement {
    public:
    POINTER_DEFINITIONS(MonitorElement);
    MonitorElement(){}
    MonitorElement(PVStructurePtr &pvStructurePtr)
    : pvStructurePtr(pvStructurePtr),
      changedBitSet(BitSet::create(pvStructurePtr->getNumberFields())),
      overrunBitSet(BitSet::create(pvStructurePtr->getNumberFields()))
    {}
    PVStructurePtr pvStructurePtr;
    BitSet::shared_pointer changedBitSet;
    BitSet::shared_pointer overrunBitSet;
};

/**
 * Interface for Monitor.
 * @author mrk
 */
class Monitor : public Destroyable{
    public:
    POINTER_DEFINITIONS(Monitor);
    virtual ~Monitor(){}
    /**
     * Start monitoring.
     * @return completion status.
     */
    virtual Status start() = 0;
    /**
     * Stop Monitoring.
     * @return completion status.
     */
    virtual Status stop() = 0;
    /**
     * If monitor has occurred return data.
     * @return monitorElement for modified data.
     * Must call get to determine if data is available.
     */
    virtual MonitorElementPtr poll() = 0;
    /**
     * Release a MonitorElement that was returned by poll.
     * @param monitorElement
     */
    virtual void release(MonitorElementPtr & monitorElement) = 0;
};


/**
 * Requester for ChannelMonitor.
 * @author mrk
 */
class MonitorRequester : public virtual Requester {
    public:
    POINTER_DEFINITIONS(MonitorRequester);
    virtual ~MonitorRequester(){}
    /**
     * The client and server have both completed the createMonitor request.
     * @param status Completion status.
     * @param monitor The monitor
     * @param structure The structure defining the data.
     */
    virtual void monitorConnect(Status const &status,
        MonitorPtr & monitor, StructureConstPtr const & structure) = 0;
    /**
     * A monitor event has occurred.
     * The requester must call Monitor.poll to get data.
     * @param monitor The monitor.
     */
    virtual void monitorEvent(MonitorPtr const &monitor) = 0;
    /**
     * The data source is no longer available.
     * @param monitor The monitor.
     */
    virtual void unlisten(MonitorPtr const &monitor) = 0;
};

}}
#endif  /* MONITOR_H */
