/* event.h */
#ifndef EVENT_H
#define EVENT_H
#include <memory>
#include <vector>
#include <epicsMutex.h>
#include <epicsEvent.h>
#include "noDefaultMethods.h"
#include "pvType.h"
#include "showConstructDestruct.h"

namespace epics { namespace pvData { 

enum EventWaitStatus {
    eventWaitOK,
    eventWaitTimeout,
    eventWaitError
};

enum EventInitialState {
    eventEmpty,
    eventFull
};
    

class Event : private NoDefaultMethods {
public:
    ~Event();
    Event(EventInitialState initial);
    static ConstructDestructCallback *getConstructDestructCallback();
    void signal();
    bool wait (); /* blocks until full */
    bool wait ( double timeOut ); /* false if empty at time out */
    bool tryWait (); /* false if empty */
private:
    epicsEventId id;
};

}}
#endif  /* EVENT_H */
