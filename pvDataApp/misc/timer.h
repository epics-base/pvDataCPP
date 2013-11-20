/* timer.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#ifndef TIMER_H
#define TIMER_H
#include <memory>
#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include <pv/pvType.h>
#include <pv/thread.h>
#include <pv/timeStamp.h>
#include <pv/event.h>
#include <pv/lock.h>
#include <pv/sharedPtr.h>

#include <shareLib.h>

namespace epics { namespace pvData { 

class TimerCallback;
class Timer;
typedef std::tr1::shared_ptr<TimerCallback> TimerCallbackPtr;
typedef std::tr1::shared_ptr<Timer> TimerPtr;

class epicsShareClass TimerCallback {
public:
    POINTER_DEFINITIONS(TimerCallback);
    TimerCallback();
    virtual ~TimerCallback(){}
    virtual void callback() = 0;
    virtual void timerStopped() = 0;
private:
    TimerCallbackPtr next;
    TimeStamp timeToRun;
    double period;
    bool onList;
    friend class Timer;
};

class epicsShareClass Timer : public Runnable {
public:
    POINTER_DEFINITIONS(Timer);
    Timer(String threadName, ThreadPriority priority);
    virtual ~Timer();
    virtual void run();
    void scheduleAfterDelay(
        TimerCallbackPtr const &timerCallback,
        double delay);
    void schedulePeriodic(
        TimerCallbackPtr const &timerCallback,
        double delay,
        double period);
    void cancel(TimerCallbackPtr const &timerCallback);
    bool isScheduled(TimerCallbackPtr const &timerCallback);
    void toString(StringBuilder builder);
private:
    void addElement(TimerCallbackPtr const &timerCallback);
    TimerCallbackPtr head;
    Mutex mutex;
    Event waitForWork;
    Event waitForDone;
    bool alive;
    Thread thread;
};

}}
#endif  /* TIMER_H */
