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

namespace epics { namespace pvData { 

    /**
     * Class instance representing monitor element.
     * @author mrk
     */
    class MonitorElement {
        public:
        /**
         * Get the PVStructure.
         * @return The PVStructure.
         */
        virtual PVStructure* getPVStructure() = 0;
        /**
         * Get the bitSet showing which fields have changed.
         * @return The bitSet.
         */
        virtual BitSet* getChangedBitSet() = 0;
        /**
         * Get the bitSet showing which fields have been changed more than once.
         * @return The bitSet.
         */
        virtual BitSet* getOverrunBitSet() = 0;
    };
    
    
    /**
     * Interface for Monitor.
     * @author mrk
     */
    class Monitor : public Destroyable, private NoDefaultMethods {
        public:
        /**
         * Start monitoring.
         * @return completion status.
         */
        virtual Status* start() = 0;
        /**
         * Stop Monitoring.
         * @return completion status.
         */
        virtual Status* stop() = 0;
        /**
         * If monitor has occurred return data.
         * @return monitorElement for modified data on null if no monitors have occurred.
         */
        virtual MonitorElement* poll() = 0;
        /**
         * Release a MonitorElement that was returned by poll.
         * @param monitorElement
         */
        virtual void release(MonitorElement* monitorElement) = 0;
    };
    
    
    /**
     * Requester for ChannelMonitor.
     * @author mrk
     */
    class MonitorRequester : public Requester {
        public:
        /**
         * The client and server have both completed the createMonitor request.
         * @param status Completion status.
         * @param monitor The monitor
         * @param structure The structure defining the data.
         */
        virtual void monitorConnect(Status* status, Monitor* monitor, Structure* structure) = 0;
        /**
         * A monitor event has occurred. The requester must call Monitor.poll to get data.
         * @param monitor The monitor.
         */
        virtual void monitorEvent(Monitor* monitor) = 0;
        /**
         * The data source is no longer available.
         * @param monitor The monitor.
         */
        virtual void unlisten(Monitor* monitor) = 0;
    };

}}
#endif  /* MONITOR_H */
