/* timer.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdexcept>

#include "pvType.h"
#include "lock.h"
#include "noDefaultMethods.h"
#include "CDRMonitor.h"
#include "linkedList.h"
#include "thread.h"
#include "timeStamp.h"
#include "timer.h"
#include "event.h"

namespace epics { namespace pvData { 

PVDATA_REFCOUNT_MONITOR_DEFINE(timerNode);
PVDATA_REFCOUNT_MONITOR_DEFINE(timer);

typedef LinkedListNode<TimerNode::Pvt> TimerListNode;
typedef LinkedList<TimerNode::Pvt> TimerList;

class TimerNode::Pvt {
public:
    TimerNode *timerNode;
    TimerCallback *callback;
    TimerListNode timerListNode;
    TimeStamp timeToRun;
    Timer::Pvt *timerPvt;
    double period;
    Pvt(TimerNode &timerNode,TimerCallback &callback);
    ~Pvt(){}
private:
};

TimerNode::Pvt::Pvt(TimerNode &timerNode,TimerCallback &callback)
: timerNode(&timerNode),callback(&callback),
  timerListNode(*this),timeToRun(),
  timerPvt(0), period(0.0)
{}

struct Timer::Pvt : public Runnable{
public:
    Pvt(String threadName,ThreadPriority priority);
    virtual void run();
public: // only used by this source module
    TimerList timerList;
    Mutex mutex;
    Event waitForWork;
    Event waitForDone;
    volatile bool alive;
    Thread thread;
    void addElement(TimerNode::Pvt &node);
};

Timer::Pvt::Pvt(String threadName,ThreadPriority priority)
: timerList(),
  mutex(Mutex()),
  waitForWork(false),
  waitForDone(false),
  alive(true),
  thread(threadName,priority,this)
{}

void Timer::Pvt::addElement(TimerNode::Pvt &node)
{
    TimerListNode *nextNode = timerList.getHead();
    if(nextNode==0) {
        timerList.addTail(node.timerListNode);
        return;
    }
    while(true) {
        TimerNode::Pvt &timerListNode = nextNode->getObject();
        if((node.timeToRun)<(timerListNode.timeToRun)) {
            timerList.insertBefore(timerListNode.timerListNode,node.timerListNode);
            return;
        } 
        nextNode = timerList.getNext(timerListNode.timerListNode);
        if(nextNode==0) {
            timerList.addTail(node.timerListNode);
            return;
        }
    }
}


TimerNode::TimerNode(TimerCallback &callback)
: pImpl(new Pvt(*this,callback))
{
    PVDATA_REFCOUNT_MONITOR_CONSTRUCT(timerNode);
}


TimerNode::~TimerNode()
{
    cancel();
    PVDATA_REFCOUNT_MONITOR_DESTRUCT(timerNode);
}

void TimerNode::cancel()
{
    Timer::Pvt *timerPvt = pImpl->timerPvt;
    if(timerPvt==0) return;
    Lock xx(timerPvt->mutex);
    if(pImpl->timerPvt==0) return;
    pImpl->timerPvt->timerList.remove(pImpl->timerListNode);
    pImpl->timerPvt = 0;
}

bool TimerNode::isScheduled()
{
    Timer::Pvt *pvt = pImpl->timerPvt;
    if(pvt==0) return false;
    Lock xx(pvt->mutex);
    return pImpl->timerListNode.isOnList();
}


void Timer::Pvt::run()
{
    TimeStamp currentTime;
    while(alive) {
         currentTime.getCurrent();
         TimeStamp *timeToRun = 0;
         double period = 0.0;
         TimerNode::Pvt *nodeToCall = 0;
         {
             Lock xx(mutex);
             TimerListNode *timerListNode = timerList.getHead();
             if(timerListNode!=0) {
                 TimerNode::Pvt *timerNodePvt = &timerListNode->getObject();
                 timeToRun = &timerNodePvt->timeToRun;
                 double diff = TimeStamp::diff(
                     *timeToRun,currentTime);
                 if(diff<=0.0) {
                     nodeToCall = timerNodePvt;
                     timerList.removeHead();
                     period = timerNodePvt->period;
                     if(period>0.0) {
                         timerNodePvt->timeToRun += period;
                         addElement(*timerNodePvt);
                     } else {
                         timerNodePvt->timerPvt = 0;
                     }
                     timerListNode = timerList.getHead();
                     if(timerListNode!=0) {
                         timerNodePvt = &timerListNode->getObject();
                         timeToRun = &timerNodePvt->timeToRun;
                     } else {
                        timeToRun = 0;
                     }
                 }
             }
         }
         if(nodeToCall!=0) {
             nodeToCall->callback->callback();
         }
         if(!alive) break;
         if(timeToRun==0) {
            waitForWork.wait();
         } else {
             double delay = TimeStamp::diff(*timeToRun,currentTime);
             waitForWork.wait(delay);
         }
    } 
    waitForDone.signal();
}

Timer::Timer(String threadName, ThreadPriority priority)
: pImpl(new Pvt(threadName,priority))
{
    PVDATA_REFCOUNT_MONITOR_CONSTRUCT(timer);
}

Timer::~Timer() {
    {
         Lock xx(pImpl->mutex);
         pImpl->alive = false;
    }
    pImpl->waitForWork.signal();
    pImpl->waitForDone.wait();
    TimerListNode *node = 0;
    while((node = pImpl->timerList.removeHead())!=0) {
        node->getObject().callback->timerStopped();
    }
    PVDATA_REFCOUNT_MONITOR_DESTRUCT(timer);
}

void Timer::scheduleAfterDelay(TimerNode &timerNode,double delay)
{
    schedulePeriodic(timerNode,delay,0.0);
}
void Timer::schedulePeriodic(TimerNode &timerNode,double delay,double period)
{
    TimerNode::Pvt *timerNodePvt = timerNode.pImpl.get();
    if(timerNodePvt->timerListNode.isOnList()) {
        throw std::logic_error(String("already queued"));
    }
    if(!pImpl->alive) {
        timerNodePvt->callback->timerStopped();
        return;
    }
    TimeStamp *timeStamp = &timerNodePvt->timeToRun;
    timeStamp->getCurrent();
    *timeStamp += delay;
    timerNodePvt->period = period;
    bool isFirst = false;
    {
        Lock xx(pImpl->mutex);
        timerNodePvt->timerPvt = pImpl.get();
        pImpl->addElement(*timerNodePvt);
        TimerNode::Pvt *first = &pImpl->timerList.getHead()->getObject();
        if(first==timerNodePvt) isFirst = true;
    }
    if(isFirst) pImpl->waitForWork.signal();
}

}}
