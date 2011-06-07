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

    /**
     * Class instance representing monitor element.
     * @author mrk
     */
    class MonitorElement {
        public:
        typedef std::tr1::shared_ptr<MonitorElement> shared_pointer;
        typedef std::tr1::shared_ptr<const MonitorElement> const_shared_pointer;

        /**
         * Get the PVStructure.
         * @return The PVStructure.
         */
        virtual PVStructure::shared_pointer const & getPVStructure() = 0;
        /**
         * Get the bitSet showing which fields have changed.
         * @return The bitSet.
         */
        virtual BitSet::shared_pointer const & getChangedBitSet() = 0;
        /**
         * Get the bitSet showing which fields have been changed more than once.
         * @return The bitSet.
         */
        virtual BitSet::shared_pointer const & getOverrunBitSet() = 0;
    };
    
    
    /**
     * Interface for Monitor.
     * @author mrk
     */
    class Monitor : public Destroyable, private NoDefaultMethods {
        public:
        typedef std::tr1::shared_ptr<Monitor> shared_pointer;
        typedef std::tr1::shared_ptr<const Monitor> const_shared_pointer;

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
        virtual MonitorElement::shared_pointer const & poll() = 0;
        /**
         * Release a MonitorElement that was returned by poll.
         * @param monitorElement
         */
        virtual void release(MonitorElement::shared_pointer const & monitorElement) = 0;
    };
    
    
    /**
     * Requester for ChannelMonitor.
     * @author mrk
     */
    class MonitorRequester : public virtual Requester {
        public:
        typedef std::tr1::shared_ptr<MonitorRequester> shared_pointer;
        typedef std::tr1::shared_ptr<const MonitorRequester> const_shared_pointer;

        /**
         * The client and server have both completed the createMonitor request.
         * @param status Completion status.
         * @param monitor The monitor
         * @param structure The structure defining the data.
         */
        virtual void monitorConnect(Status const &status,
            Monitor::shared_pointer const & monitor, StructureConstPtr const & structure) = 0;
        /**
         * A monitor event has occurred.
         * The requester must call Monitor.poll to get data.
         * @param monitor The monitor.
         */
        virtual void monitorEvent(Monitor::shared_pointer const &monitor) = 0;
        /**
         * The data source is no longer available.
         * @param monitor The monitor.
         */
        virtual void unlisten(Monitor::shared_pointer const &monitor) = 0;
    };

}}
#endif  /* MONITOR_H */
