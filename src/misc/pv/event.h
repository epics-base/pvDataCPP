/* event.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#ifndef EVENT_H
#define EVENT_H

#include <memory>
#include <vector>

#ifdef epicsExportSharedSymbols
#define eventepicsExportSharedSymbols
#undef epicsExportSharedSymbols
#endif

#include <epicsEvent.h>

#ifdef eventepicsExportSharedSymbols
#define epicsExportSharedSymbols
#undef eventepicsExportSharedSymbols
#endif

#include <pv/pvType.h>
#include <pv/sharedPtr.h>

#include <shareLib.h>

namespace epics { namespace pvData { 

class Event;
typedef std::tr1::shared_ptr<Event> EventPtr;

/**
 * @brief C++ wrapper for epicsEvent from EPICS base.
 *
 */
class epicsShareClass Event {
public:
    POINTER_DEFINITIONS(Event);
    /**
     * Constructor
     */
    explicit Event(bool = false);
    /**
     * Destructor.
     */
    ~Event();
    /**
     * Signal the event i.e. ensures that the next or current call to wait completes. 
     */
    void signal();
    /**
     * wait 
     * @return (false,true) if (some error, event signaled).
     * The next wait or tryWait will clear signal.
     */
    bool wait (); /* blocks until full */
    /**
     * wait for up to timeOut seconds.
     * @param timeOut max number of seconds to wait
     * @return (false, true) if (timeout or error, event signaled).
     */
    bool wait ( double timeOut ); /* false if empty at time out */
    /**
     * See if a signal has been called.
     * @return (false, true) if (timeout or error, event signaled).
     */
    bool tryWait (); /* false if empty */
private:
    epicsEventId id;
};

}}
#endif  /* EVENT_H */
