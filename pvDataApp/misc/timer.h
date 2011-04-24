/* timer.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef TIMER_H
#define TIMER_H
#include <memory>
#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "pvType.h"
#include "thread.h"
#include "noDefaultMethods.h"
#include "sharedPtr.h"

namespace epics { namespace pvData { 

class Timer;

class TimerCallback {
public:
    virtual ~TimerCallback(){}
    virtual void callback() = 0;
    virtual void timerStopped() = 0;
};

class TimerNode {
public:
    TimerNode(TimerCallback &timerCallback);
    ~TimerNode();
    void cancel();
    bool isScheduled();
    class Pvt;
private:
    std::auto_ptr<Pvt> pImpl;
    friend class Timer;
};

class Timer : private NoDefaultMethods {
public:
    typedef std::tr1::shared_ptr<Timer> shared_pointer;
    typedef std::tr1::shared_ptr<const Timer> const_shared_pointer;

    Timer(String threadName, ThreadPriority priority);
    ~Timer();
    void scheduleAfterDelay(TimerNode &timerNode,double delay);
    void schedulePeriodic(TimerNode &timerNode,double delay,double period);

    class Pvt;
private:
    std::auto_ptr<Pvt> pImpl;
};

}}
#endif  /* TIMER_H */
