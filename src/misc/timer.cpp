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

void Timer::addElement(TimerCallbackPtr const & timerCallback)
{
    timerCallback->onList = true;
    if(head.get()==NULL) {
        head = timerCallback;
        timerCallback->next.reset();
        return;
    }
    TimerCallbackPtr nextNode(head);
    TimerCallbackPtr prevNode;
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

bool Timer::isScheduled(TimerCallbackPtr const &timerCallback)
{
    Lock xx(mutex);
    return timerCallback->onList;
}


void Timer::run()
{
    TimeStamp currentTime;
    while(true) {
         double period = 0.0;
         TimerCallbackPtr nodeToCall;
         {
             Lock xx(mutex);
             currentTime.getCurrent();
             if (!alive) break;
             TimerCallbackPtr timerCallback = head;
             if(timerCallback) {
                 double diff = TimeStamp::diff(
                     timerCallback->timeToRun,currentTime);
                 if(diff<=0.0) {
                     nodeToCall = timerCallback;
                     nodeToCall->onList = false;
                     head = head->next;
                     period = timerCallback->period;
                     if(period>0.0) {
                         timerCallback->timeToRun += period;
                         addElement(timerCallback);
                     }
                     timerCallback = head;
                 }
             }
         }
         if(nodeToCall) {
             nodeToCall->callback();
         }
         {
             Lock xx(mutex);
             if(!alive) break;
         }
         if(head.get()==NULL) {
            waitForWork.wait();
         } else {
             double delay = TimeStamp::diff(head->timeToRun,currentTime);
             waitForWork.wait(delay);
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

void Timer::dump(std::ostream& o)
{
    Lock xx(mutex);
    if(!alive) return;
    TimeStamp currentTime;
    TimerCallbackPtr nodeToCall(head);
    currentTime.getCurrent();
    while(true) {
         if(nodeToCall.get()==NULL) return;
         TimeStamp timeToRun = nodeToCall->timeToRun;
         double period = nodeToCall->period;
         double diff = TimeStamp::diff(timeToRun,currentTime);
         o << "timeToRun " << diff << " period " << period << std::endl;
         nodeToCall = nodeToCall->next;
     }
}

std::ostream& operator<<(std::ostream& o, Timer& timer)
{
    timer.dump(o);
    return o;
}

}}
