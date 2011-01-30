/* thread.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>
#include <stdexcept>

#include <epicsThread.h>
#include <epicsEvent.h>
#include <epicsExit.h>
#include "lock.h"
#include "event.h"
#include "thread.h"
#include "linkedList.h"
#include "showConstructDestruct.h"

namespace epics { namespace pvData { 

static unsigned int epicsPriority[] = {
        epicsThreadPriorityLow,
        epicsThreadPriorityLow + 15,
        epicsThreadPriorityMedium - 15,
        epicsThreadPriorityMedium,
        epicsThreadPriorityMedium + 15,
        epicsThreadPriorityHigh - 15,
        epicsThreadPriorityHigh
};

unsigned int const * const ThreadPriorityFunc::getEpicsPriorities()
{
    return epicsPriority;
}


static String threadPriorityNames[] = {
    String("lowest"),String("lower"),String("low"),
    String("middle"),
    String("high"),String("higher"),String("highest")
};

class ThreadListElement;
typedef LinkedListNode<ThreadListElement> ThreadListNode;
typedef LinkedList<ThreadListElement> ThreadList;

PVDATA_REFCOUNT_MONITOR_DEFINE(thread);

static Mutex listGuard;
static ThreadList *threadList;

static void deleteStatic(void*)
{
    delete threadList;
}

static void init(void*)
{
    threadList = new ThreadList();
    epicsAtExit(&deleteStatic,0);
}

static
epicsThreadOnceId initOnce = EPICS_THREAD_ONCE_INIT;


class ThreadListElement {
public:
    ThreadListElement(Thread *thread) : thread(thread),node(new ThreadListNode(this)){}
    ~ThreadListElement(){delete node;}
    Thread *thread;
    ThreadListNode *node;
};


int ThreadPriorityFunc::getEpicsPriority(ThreadPriority threadPriority) {
        return epicsPriority[threadPriority];
}

extern  "C" void myFunc ( void * pPvt );


class ThreadPvt {
public:
    ThreadPvt(Thread *thread,String name,
        ThreadPriority priority, Runnable*runnable);
    virtual ~ThreadPvt();
    void ready();
public: // only used within this source module
    Thread *thread;
    String name;
    ThreadPriority priority;
    Runnable *runnable;
    bool isReady;
    ThreadListElement *threadListElement;
    Event *waitDone;
    epicsThreadId id;
};

extern "C" void myFunc ( void * pPvt )
{
    ThreadPvt *threadPvt = (ThreadPvt *)pPvt;
    threadPvt->runnable->run();
    threadPvt->waitDone->signal();
}

ThreadPvt::ThreadPvt(Thread *thread,String name,
    ThreadPriority priority, Runnable *runnable)
: thread(thread),name(name),priority(priority),
  runnable(runnable),
  isReady(false),
  threadListElement(new ThreadListElement(thread)),
  waitDone(new Event()),
  id(epicsThreadCreate(
        name.c_str(),
        epicsPriority[priority],
        epicsThreadGetStackSize(epicsThreadStackSmall),
        myFunc,this))
{
    epicsThreadOnce(&initOnce, &init, 0);
    PVDATA_REFCOUNT_MONITOR_CONSTRUCT(thread);
    Lock x(&listGuard);
    threadList->addTail(threadListElement->node);
}

ThreadPvt::~ThreadPvt()
{
    bool result = waitDone->wait(2.0);
    if(!result) {
        throw std::logic_error(String("delete thread but run did not return"));
        String message("destroy thread ");
        message += thread->getName();
        message += " but run did not return";
        throw std::logic_error(message);
    }
    if(!threadListElement->node->isOnList()) {
        String message("destroy thread ");
        message += thread->getName();
        message += " is not on threadlist";
        throw std::logic_error(message);
    }
    Lock x(&listGuard);
    threadList->remove(threadListElement->node);
    delete waitDone;
    delete threadListElement;
    PVDATA_REFCOUNT_MONITOR_DESTRUCT(thread);
}

Thread::Thread(String name,ThreadPriority priority,Runnable *runnable)
: pImpl(new ThreadPvt(this,name,priority,runnable))
{
}

Thread::~Thread()
{
   delete pImpl;
}

void Thread::sleep(double seconds)
{
    epicsThreadSleep(seconds);;
}

String Thread::getName()
{
    return pImpl->name;
}

ThreadPriority Thread::getPriority()
{
    return pImpl->priority;
}

void Thread::showThreads(StringBuilder buf)
{
    Lock x(&listGuard);
    ThreadListNode *node = threadList->getHead();    
    while(node!=0) {
        Thread *thread = node->getObject()->thread;
        *buf += thread->getName();
        *buf += " ";
        *buf += threadPriorityNames[thread->getPriority()];
        *buf += "\n";
        node = threadList->getNext(node);
    }
}

}}
