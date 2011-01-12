/* event.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef EVENT_H
#define EVENT_H
#include <memory>
#include <vector>
#include <epicsEvent.h>
#include "noDefaultMethods.h"
#include "pvType.h"

namespace epics { namespace pvData { 

class Event : private NoDefaultMethods {
public:
    explicit Event(bool = false);
    ~Event();
    void signal();
    bool wait (); /* blocks until full */
    bool wait ( double timeOut ); /* false if empty at time out */
    bool tryWait (); /* false if empty */
private:
    epicsEventId id;
};

}}
#endif  /* EVENT_H */
