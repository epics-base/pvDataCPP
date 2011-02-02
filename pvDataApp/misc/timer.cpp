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

class TimerNodePvt;

typedef LinkedListNode<TimerNodePvt> TimerListNode;
typedef LinkedList<TimerNodePvt> TimerList;

class TimerNodePvt {
public:
    TimerNode *timerNode;
    TimerCallback *callback;
    TimerListNode *timerListNode;
    TimeStamp timeToRun;
    TimerPvt *timerPvt;
    double period;
    TimerNodePvt(TimerNode *timerNode,TimerCallback *callback);
    ~TimerNodePvt();
};

TimerNodePvt::TimerNodePvt(TimerNode *timerNode,TimerCallback *callback)
: timerNode(timerNode),callback(callback),
  timerListNode(new TimerListNode(this)),timeToRun(TimeStamp()),
  timerPvt(0), period(0.0)
{}

TimerNodePvt::~TimerNodePvt()
{
    delete timerListNode;
}

struct TimerPvt : public Runnable{
public:
    TimerPvt(String threadName,ThreadPriority priority);
    ~TimerPvt();
    virtual void run();
public: // only used by this source module
    TimerList *timerList;
    Mutex mutex;
    Event *waitForWork;
    Event *waitForDone;
    volatile bool alive;
    Thread *thread;
};

TimerPvt::TimerPvt(String threadName,ThreadPriority priority)
: timerList(new TimerList()),
  mutex(Mutex()),
  waitForWork(new Event(false)),
  waitForDone(new Event(false)),
  alive(true),
  thread(new Thread(threadName,priority,this))
{}

TimerPvt::~TimerPvt()
{
    delete thread;
    delete waitForDone;
    delete waitForWork;
    delete timerList;
}

static void addElement(TimerPvt *timer,TimerNodePvt *node)
{
    TimerList *timerList = timer->timerList;
    TimerListNode *nextNode = timerList->getHead();
    if(nextNode==0) {
        timerList->addTail(node->timerListNode);
        return;
    }
    while(true) {
        TimerNodePvt *timerListNode = nextNode->getObject();
        if((node->timeToRun)<(timerListNode->timeToRun)) {
            timerList->insertBefore(timerListNode->timerListNode,node->timerListNode);
            return;
        } 
        nextNode = timerList->getNext(timerListNode->timerListNode);
        if(nextNode==0) {
            timerList->addTail(node->timerListNode);
            return;
        }
    }
}


TimerNode::TimerNode(TimerCallback *callback)
: pImpl(new TimerNodePvt(this,callback))
{
    PVDATA_REFCOUNT_MONITOR_CONSTRUCT(timerNode);
}


TimerNode::~TimerNode()
{
    cancel();
    delete pImpl;
    PVDATA_REFCOUNT_MONITOR_DESTRUCT(timerNode);
}

void TimerNode::cancel()
{
    TimerPvt *timerPvt = pImpl->timerPvt;
    if(timerPvt==0) return;
    Lock xx(&timerPvt->mutex);
    if(pImpl->timerPvt==0) return;
    pImpl->timerPvt->timerList->remove(pImpl);
    pImpl->timerPvt = 0;
}

bool TimerNode::isScheduled()
{
    TimerPvt *pvt = pImpl->timerPvt;
    if(pvt==0) return false;
    Lock xx(&pvt->mutex);
    return pImpl->timerListNode->isOnList();
}


void TimerPvt::run()
{
    TimeStamp currentTime;
    while(alive) {
         currentTime.getCurrent();
         TimeStamp *timeToRun = 0;
         double period = 0.0;
         TimerNodePvt *nodeToCall = 0;
         {
             Lock xx(&mutex);
             TimerListNode *timerListNode = timerList->getHead();
             if(timerListNode!=0) {
                 TimerNodePvt *timerNodePvt = timerListNode->getObject();
                 timeToRun = &timerNodePvt->timeToRun;
                 double diff = TimeStamp::diff(
                     *timeToRun,currentTime);
                 if(diff<=0.0) {
                     nodeToCall = timerNodePvt;
                     timerList->removeHead();
                     period = timerNodePvt->period;
                     if(period>0.0) {
                         timerNodePvt->timeToRun += period;
                         addElement(this,timerNodePvt);
                     } else {
                         timerNodePvt->timerPvt = 0;
                     }
                     timerListNode = timerList->getHead();
                     if(timerListNode!=0) {
                         timerNodePvt = timerListNode->getObject();
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
            waitForWork->wait();
         } else {
             double delay = TimeStamp::diff(*timeToRun,currentTime);
             waitForWork->wait(delay);
         }
    } 
    waitForDone->signal();
}

Timer::Timer(String threadName, ThreadPriority priority)
: pImpl(new TimerPvt(threadName,priority))
{
    PVDATA_REFCOUNT_MONITOR_CONSTRUCT(timer);
}

Timer::~Timer() {
    {
         Lock xx(&pImpl->mutex);
         pImpl->alive = false;
    }
    pImpl->waitForWork->signal();
    pImpl->waitForDone->wait();
    TimerList *timerList = pImpl->timerList;
    TimerListNode *node = 0;
    while((node = timerList->removeHead())!=0) {
        node->getObject()->callback->timerStopped();
    }
    delete pImpl;
    PVDATA_REFCOUNT_MONITOR_DESTRUCT(timer);
}

void Timer::scheduleAfterDelay(TimerNode *timerNode,double delay)
{
    schedulePeriodic(timerNode,delay,0.0);
}
void Timer::schedulePeriodic(TimerNode *timerNode,double delay,double period)
{
    TimerNodePvt *timerNodePvt = timerNode->pImpl;
    if(timerNodePvt->timerListNode->isOnList()) {
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
        Lock xx(&pImpl->mutex);
        timerNodePvt->timerPvt = pImpl;
        addElement(pImpl,timerNodePvt);
        TimerNodePvt *first = pImpl->timerList->getHead()->getObject();
        if(first==timerNodePvt) isFirst = true;
    }
    if(isFirst) pImpl->waitForWork->signal();
}

}}
