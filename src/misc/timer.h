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

/**
 * @brief Class that must be implemented by code that makes Timer requests.
 *
 */
class epicsShareClass TimerCallback {
public:
    POINTER_DEFINITIONS(TimerCallback);
    /**
     * Constructor
     */
    TimerCallback();
    /**
     * Destructor
     */
    virtual ~TimerCallback(){}
    /**
     * The method that is called when a timer expires.
     */
    virtual void callback() = 0;
    /**
     * The timer has stopped.
     */
    virtual void timerStopped() = 0;
private:
    TimerCallbackPtr next;
    TimeStamp timeToRun;
    double period;
    bool onList;
    friend class Timer;
};

/**
 * @brief Support for delayed or periodic callback execution.
 *
 */
class epicsShareClass Timer : public Runnable {
public:
    POINTER_DEFINITIONS(Timer);
    /**
     * Constructor
     * @param threadName name for the timer thread.
     * @param priority thread priority
     */
    Timer(std::string threadName, ThreadPriority priority);
    /**
     * Destructor
     */
    virtual ~Timer();
    /**
     * The thread run method. This is called automatically.
     */
    virtual void run();
    /**
     * schedule a callback after a delay.
     * @param timerCallback the timerCallback instance.
     * @param delay number of seconds before calling callback.
     */
    void scheduleAfterDelay(
        TimerCallbackPtr const &timerCallback,
        double delay);
    /**
     * schedule a periodic callback.`
     * @param timerCallback the timerCallback instance.
     * @param delay number of seconds before first callback.
     * @param period time in seconds between each callback.
     */
    void schedulePeriodic(
        TimerCallbackPtr const &timerCallback,
        double delay,
        double period);
    /**
     * cancel a callback.
     * @param timerCallback the timerCallback to cancel.
     */
    void cancel(TimerCallbackPtr const &timerCallback);
    /**
     * Is the callback scheduled to be called?
     * @param timerCallback the timerCallback.
     * @return (false,true) if (not, is) scheduled.
     */
    bool isScheduled(TimerCallbackPtr const &timerCallback);
    /**
     * show the elements in the timer queue.
     * @parm o The output stream for the output
     */
    void dump(std::ostream& o);

private:
    void addElement(TimerCallbackPtr const &timerCallback);
    TimerCallbackPtr head;
    Mutex mutex;
    Event waitForWork;
    Event waitForDone;
    bool alive;
    Thread thread;
};

epicsShareExtern std::ostream& operator<<(std::ostream& o, Timer& timer);

}}
#endif  /* TIMER_H */
