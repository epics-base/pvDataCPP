/* timer.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef TIMER_H
#define TIMER_H
#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "pvType.h"
#include "thread.h"
#include "noDefaultMethods.h"
#include "showConstructDestruct.h"

namespace epics { namespace pvData { 

class TimerCallback {
public:
    virtual void callback() = 0;
    virtual void timerStopped() = 0;
};

class TimerNode : private NoDefaultMethods {
public:
    static ConstructDestructCallback *getConstructDestructCallback();
    static TimerNode *create(TimerCallback *timerCallback);
    void destroy();
    void cancel();
    bool isScheduled();
private:
    TimerNode(TimerCallback *timerCallback);
    ~TimerNode();
    class TimerNodePvt *pImpl;
    friend class Timer;
};

class Timer : private NoDefaultMethods {
public:
    static ConstructDestructCallback *getConstructDestructCallback();
    static Timer * create(String threadName, ThreadPriority priority);
    void destroy();
    void scheduleAfterDelay(TimerNode *timerNode,double delay);
    void schedulePeriodic(TimerNode *timerNode,double delay,double period);
private:
    Timer(String threadName, ThreadPriority priority);
    ~Timer();
    class TimerPvt *pImpl;
    friend class TimerNode;
};

}}
#endif  /* TIMER_H */
