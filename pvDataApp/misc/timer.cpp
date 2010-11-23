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
#include "showConstructDestruct.h"
#include "linkedList.h"
#include "thread.h"
#include "timeStamp.h"
#include "timer.h"
#include "event.h"

namespace epics { namespace pvData { 


static volatile int64 totalNodeConstruct = 0;
static volatile int64 totalNodeDestruct = 0;
static volatile int64 totalTimerConstruct = 0;
static volatile int64 totalTimerDestruct = 0;
static Mutex *globalMutex = 0;

static int64 getTotalTimerNodeConstruct()
{
    Lock xx(globalMutex);
    return totalNodeConstruct;
}

static int64 getTotalTimerNodeDestruct()
{
    Lock xx(globalMutex);
    return totalNodeDestruct;
}

static int64 getTotalTimerConstruct()
{
    Lock xx(globalMutex);
    return totalTimerConstruct;
}

static int64 getTotalTimerDestruct()
{
    Lock xx(globalMutex);
    return totalTimerDestruct;
}

static ConstructDestructCallback *pCDCallbackTimerNode;
static ConstructDestructCallback *pCDCallbackTimer;

static void init()
{
     static Mutex mutex = Mutex();
     Lock xx(&mutex);
     if(globalMutex==0) {
        globalMutex = new Mutex();
        pCDCallbackTimerNode = new ConstructDestructCallback(
            "timerNode",
            getTotalTimerNodeConstruct,getTotalTimerNodeDestruct,0);

        pCDCallbackTimer = new ConstructDestructCallback(
            "timer",
            getTotalTimerConstruct,getTotalTimerDestruct,0);
     }
}

ConstructDestructCallback * TimerNode::getConstructDestructCallback()
{
    init();
    return pCDCallbackTimerNode;
}

ConstructDestructCallback * Timer::getConstructDestructCallback()
{
    init();
    return pCDCallbackTimer;
}

class TimerNodePvt;

typedef LinkedListNode<TimerNodePvt> ListNode;
typedef LinkedList<TimerNodePvt> List;

class TimerNodePvt {
public:
    TimerNode *timerNode;
    TimerCallback *callback;
    ListNode *listNode;
    TimeStamp timeToRun;
    TimerPvt *timerPvt;
    double period;
    TimerNodePvt(TimerNode *timerNode,TimerCallback *callback);
    ~TimerNodePvt();
};

TimerNodePvt::TimerNodePvt(TimerNode *timerNode,TimerCallback *callback)
: timerNode(timerNode),callback(callback),
  listNode(new ListNode(this)),timeToRun(TimeStamp()),
  timerPvt(0), period(0.0)
{}

TimerNodePvt::~TimerNodePvt()
{
    delete listNode;
}

struct TimerPvt : public RunnableReady {
public:
    TimerPvt(String threadName,ThreadPriority priority);
    ~TimerPvt();
    virtual void run(ThreadReady *threadReady);
public: // only used by this source module
    List *list;
    Mutex mutex;
    Event *waitForWork;
    Event *waitForDone;
    Thread *thread;
    volatile bool alive;
};

TimerPvt::TimerPvt(String threadName,ThreadPriority priority)
: list(new List()),
  mutex(Mutex()),
  waitForWork(new Event(eventEmpty)),
  waitForDone(new Event(eventEmpty)),
  thread(new Thread(threadName,priority,this)),
  alive(true)
{
    thread->start();
}

TimerPvt::~TimerPvt()
{
    delete thread;
    delete waitForDone;
    delete waitForWork;
    delete list;
}

static void addElement(TimerPvt *timer,TimerNodePvt *node)
{
    List *list = timer->list;
    ListNode *nextNode = list->getHead();
    if(nextNode==0) {
        list->addTail(node->listNode);
        return;
    }
    while(true) {
        TimerNodePvt *listNode = nextNode->getObject();
        if((node->timeToRun)<(listNode->timeToRun)) {
            list->insertBefore(listNode->listNode,node->listNode);
            return;
        } 
        nextNode = list->getNext(listNode->listNode);
        if(nextNode==0) {
            list->addTail(node->listNode);
            return;
        }
    }
}


TimerNode::TimerNode(TimerCallback *callback)
: pImpl(new TimerNodePvt(this,callback))
{
    init();
    Lock xx(globalMutex);
    totalNodeConstruct++;
}

TimerNode *TimerNode::create(TimerCallback *callback)
{
    return new TimerNode(callback);
}

TimerNode::~TimerNode()
{
    delete pImpl;
    Lock xx(globalMutex);
    totalNodeDestruct++;
}

void TimerNode::destroy()
{
   cancel();
   delete this;
}

void TimerNode::cancel()
{
    TimerPvt *timerPvt = pImpl->timerPvt;
    if(timerPvt==0) return;
    Lock xx(&timerPvt->mutex);
    if(pImpl->timerPvt==0) return;
    pImpl->timerPvt->list->remove(pImpl);
    pImpl->timerPvt = 0;
}

bool TimerNode::isScheduled()
{
    TimerPvt *pvt = pImpl->timerPvt;
    if(pvt==0) return false;
    Lock xx(&pvt->mutex);
    return pImpl->listNode->isOnList();
}


void TimerPvt::run(ThreadReady *threadReady)
{
    threadReady->ready();
    TimeStamp currentTime;
    while(alive) {
         currentTime.getCurrent();
         TimeStamp *timeToRun = 0;
         double period = 0.0;
         TimerNodePvt *nodeToCall = 0;
         {
             Lock xx(&mutex);
             ListNode *listNode = list->getHead();
             if(listNode!=0) {
                 TimerNodePvt *timerNodePvt = listNode->getObject();
                 timeToRun = &timerNodePvt->timeToRun;
                 double diff = TimeStamp::diff(
                     *timeToRun,currentTime);
                 if(diff<=0.0) {
                     nodeToCall = timerNodePvt;
                     list->removeHead();
                     period = timerNodePvt->period;
                     if(period>0) {
                         timerNodePvt->timeToRun += period;
                         addElement(this,timerNodePvt);
                     } else {
                         timerNodePvt->timerPvt = 0;
                     }
                     listNode = list->getHead();
                     if(listNode!=0) {
                         timerNodePvt = listNode->getObject();
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
    init();
    Lock xx(globalMutex);
    totalTimerConstruct++;
}

Timer * Timer::create(String threadName, ThreadPriority priority)
{
    return new Timer(threadName,priority);
}

Timer::~Timer() {
    delete pImpl;
    Lock xx(globalMutex);
    totalTimerDestruct++;
}

void Timer::destroy()
{
    {
         Lock xx(&pImpl->mutex);
         pImpl->alive = false;
         pImpl->waitForWork->signal();
         pImpl->waitForDone->wait();
    }
    List *list = pImpl->list;
    ListNode *node = 0;
    while((node = list->removeHead())!=0) {
        node->getObject()->callback->timerStopped();
    }
    delete this;
}

void Timer::scheduleAfterDelay(TimerNode *timerNode,double delay)
{
    schedulePeriodic(timerNode,delay,0.0);
}
void Timer::schedulePeriodic(TimerNode *timerNode,double delay,double period)
{
    TimerNodePvt *timerNodePvt = timerNode->pImpl;
    if(timerNodePvt->listNode->isOnList()) {
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
        TimerNodePvt *first = pImpl->list->getHead()->getObject();
        if(first==timerNodePvt) isFirst = true;
    }
    if(isFirst) pImpl->waitForWork->signal();
}

}}
