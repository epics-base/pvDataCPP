/* timer.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
 
#ifdef _WIN32
#define NOMINMAX
#endif

#include <stdexcept>

#define epicsExportSharedSymbols
#include <pv/convert.h>
#include <pv/timer.h>

namespace epics { namespace pvData { 

TimerCallback::TimerCallback()
: period(0.0),
  onList(false)
{
}

Timer::Timer(String threadName,ThreadPriority priority)
: waitForWork(false),
  waitForDone(false),
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
            if(prevNode.get()!=NULL) {
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
            if(prevNode.get()!=NULL) {
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
    throw std::logic_error(String(""));
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
             if(timerCallback.get()!=NULL) {
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
         if(nodeToCall.get()!=NULL) {
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
    waitForDone.signal();
}

Timer::~Timer() {
    {
         Lock xx(mutex);
         alive = false;
    }
    waitForWork.signal();
    waitForDone.wait();
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
        throw std::logic_error(String("already queued"));
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

void Timer::toString(StringBuilder builder)
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
         char buffer[50];
         sprintf(buffer,"timeToRun %f period %f\n",diff,period);
         *builder += buffer;
         nodeToCall = nodeToCall->next;
     }
}

}}
