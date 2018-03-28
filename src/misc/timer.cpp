/* timer.cpp */
/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */
/**
 *  @author mrk
 */
 
#if defined(_WIN32) && !defined(NOMINMAX)
#define NOMINMAX
#endif

#include <stdexcept>
#include <string>
#include <iostream>

#include <epicsThread.h>
#include <epicsGuard.h>

#define epicsExportSharedSymbols
#include <pv/timer.h>

using std::string;

namespace epics { namespace pvData { 

TimerCallback::TimerCallback()
: period(0.0),
  onList(false)
{
}

Timer::Timer(string threadName,ThreadPriority priority)
: waitForWork(false),
  alive(true),
  thread(threadName,priority,this)
{}

// call with mutex held
void Timer::addElement(TimerCallbackPtr const & timerCallback)
{
    assert(!timerCallback->onList);
    assert(!timerCallback->next);

    timerCallback->onList = true;
    if(!head) {
        head = timerCallback;
        timerCallback->next.reset();
        return;
    }

    TimerCallbackPtr nextNode(head), prevNode;

    while(true) {
        if(timerCallback->timeToRun < nextNode->timeToRun) {
            if(prevNode) {
                prevNode->next = timerCallback;
            } else {
                head = timerCallback;
            }
            timerCallback->next = nextNode;
            return;
        }
        if(nextNode->next.get()==NULL) {
            nextNode->next = timerCallback;
            timerCallback->next.reset();
            return;
        }
        prevNode = nextNode;
        nextNode = nextNode->next;
    }
}


void Timer::cancel(TimerCallbackPtr const &timerCallback)
{
    Lock xx(mutex);
    if(!timerCallback->onList) return;
    TimerCallbackPtr nextNode(head);
    TimerCallbackPtr prevNode;
    while(true) {
        if(nextNode.get()==timerCallback.get()) {
            if(prevNode) {
                prevNode->next = timerCallback->next;
            } else {
                head = timerCallback->next;
            }
            timerCallback->next.reset();
            timerCallback->onList = false;
            return;
        }
        prevNode = nextNode;
        nextNode = nextNode->next;
    }
    throw std::logic_error(string(""));
}

bool Timer::isScheduled(TimerCallbackPtr const &timerCallback) const
{
    Lock xx(mutex);
    return timerCallback->onList;
}


void Timer::run()
{
    epicsGuard<epicsMutex> G(mutex);

    while(alive) {

         TimerCallbackPtr next;

         epicsTime currentTime(epicsTime::getCurrent());

         double delay = -1;

         if(head) {
             // there may be work to be done

             delay = head->timeToRun - currentTime;

             if(delay <= 0.0) {
                 // head timer has expired

                 // we take head, move head = head->next
                 next.swap(head);
                 head.swap(next->next);

                 next->onList = false;

                 // re-schedule periodic
                 if(next->period > 0.0) {
                     next->timeToRun += next->period;
                     addElement(next);
                 }

                 if(head) {
                     delay = head->timeToRun - currentTime;
                 }
             }
         };

         bool hasHead = !!head;

         {
             epicsGuardRelease<epicsMutex> U(G);

             if(next) {
                 next->callback();
             }

             if(hasHead) {
                 waitForWork.wait(delay);
             } else {
                 waitForWork.wait();
             }
         }
    } 
}

Timer::~Timer() {
    {
         Lock xx(mutex);
         alive = false;
    }
    waitForWork.signal();
    thread.exitWait();
    TimerCallbackPtr timerCallback;
    while(true) {
        timerCallback = head;
        if(head.get()==NULL) break;
        head->timerStopped();
        head = timerCallback->next;
        timerCallback->next.reset();
        timerCallback->onList = false;
    }
}

void Timer::scheduleAfterDelay(
    TimerCallbackPtr const &timerCallback,
    double delay)
{
    schedulePeriodic(timerCallback,delay,0.0);
}

void Timer::schedulePeriodic(
    TimerCallbackPtr const &timerCallback,
    double delay,
    double period)
{
    if(isScheduled(timerCallback)) {
        throw std::logic_error(string("already queued"));
    }
    {
        Lock xx(mutex);
        if(!alive) {
            timerCallback->timerStopped();
            return;
        }
    }
    TimeStamp timeStamp;
    timeStamp.getCurrent();
    timeStamp += delay;
    timerCallback->timeToRun.getCurrent();
    timerCallback->timeToRun += delay;
    timerCallback->period = period;
    bool isFirst = false;
    {
        Lock xx(mutex);
        addElement(timerCallback);
        if(timerCallback.get()==head.get()) isFirst = true;
    }
    if(isFirst) waitForWork.signal();
}

void Timer::dump(std::ostream& o) const
{
    Lock xx(mutex);
    if(!alive) return;
    epicsTime currentTime(epicsTime::getCurrent());
    TimerCallbackPtr nodeToCall(head);

    while(nodeToCall) {
        o << "timeToRun " << (nodeToCall->timeToRun - currentTime)
          << " period " << nodeToCall->period << "\n";
        nodeToCall = nodeToCall->next;
    }
}

std::ostream& operator<<(std::ostream& o, const Timer& timer)
{
    timer.dump(o);
    return o;
}

}}
