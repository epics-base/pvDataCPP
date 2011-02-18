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
#include "CDRMonitor.h"

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

typedef LinkedListNode<Thread> ThreadListNode;
typedef LinkedList<Thread> ThreadList;

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

int ThreadPriorityFunc::getEpicsPriority(ThreadPriority threadPriority) {
        return epicsPriority[threadPriority];
}


class Thread::ThreadPvt {
public:
    ThreadPvt(Thread *thread,String name,
        ThreadPriority priority, Runnable*runnable);
    ~ThreadPvt();
    void ready();
    const String name;
    const ThreadPriority priority;
private:
    Runnable *runnable;
    bool isReady;
    ThreadListNode threadNode;
    Event waitDone;
    epicsThreadId id;

    static void threadMain(void*);
};

void Thread::ThreadPvt::threadMain ( void * pPvt )
{
    ThreadPvt *threadPvt = (ThreadPvt *)pPvt;
    threadPvt->runnable->run();
    threadPvt->waitDone.signal();
}

Thread::ThreadPvt::ThreadPvt(Thread *thread,String name,
    ThreadPriority priority, Runnable *runnable)
: name(name),priority(priority),
  runnable(runnable),
  isReady(false),
  threadNode(*thread),
  waitDone(),
  id(epicsThreadCreate(
        name.c_str(),
        epicsPriority[priority],
        epicsThreadGetStackSize(epicsThreadStackSmall),
        &threadMain,this))
{
    if(!id) {
        throw std::runtime_error("Unable to create thread");
    }
    epicsThreadOnce(&initOnce, &init, 0);
    assert(threadList);
    PVDATA_REFCOUNT_MONITOR_CONSTRUCT(thread);
    Lock x(listGuard);
    threadList->addTail(threadNode);
}

Thread::ThreadPvt::~ThreadPvt()
{
    bool result = waitDone.wait(2.0);
    if(!result) {
        throw std::logic_error(String("delete thread but run did not return"));
        String message("destroy thread ");
        message += name;
        message += " but run did not return";
        throw std::logic_error(message);
    }
    if(!threadNode.isOnList()) {
        String message("destroy thread ");
        message += name;
        message += " is not on threadlist";
        throw std::logic_error(message);
    }
    Lock x(listGuard);
    threadList->remove(threadNode);
    PVDATA_REFCOUNT_MONITOR_DESTRUCT(thread);
}

Thread::Thread(String name,ThreadPriority priority,Runnable *runnable)
: pImpl(new ThreadPvt(this,name,priority,runnable))
{
}

// Must be present or auto_ptr<> will not delete ThreadPvt
Thread::~Thread() {}

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
    Lock x(listGuard);
    ThreadListNode *node = threadList->getHead();    
    while(node!=0) {
        Thread &thread = node->getObject();
        *buf += thread.getName();
        *buf += " ";
        *buf += threadPriorityNames[thread.getPriority()];
        *buf += "\n";
        node = threadList->getNext(*node);
    }
}

}}
