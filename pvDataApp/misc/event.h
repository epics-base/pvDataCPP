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
#include <epicsEvent.h>

#include <pv/pvType.h>
#include <pv/sharedPtr.h>

#include <shareLib.h>

namespace epics { namespace pvData { 

class Event;
typedef std::tr1::shared_ptr<Event> EventPtr;

class epicsShareClass Event {
public:
    POINTER_DEFINITIONS(Event);
    explicit Event(bool = false);
    ~Event();
    void signal();
    bool wait (); /* blocks until full */
    bool wait ( double timeOut ); /* false if empty at time out */
    bool tryWait (); /* false if empty */
private:
    epicsEventId id;
    String alreadyOn;
};

}}
#endif  /* EVENT_H */
