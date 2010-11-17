/* thread.cpp */
#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>

#include <epicsThread.h>
#include <epicsEvent.h>
#include "lock.h"
#include "event.h"
#include "thread.h"
#include "linkedList.h"

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

static volatile int64 totalConstruct = 0;
static volatile int64 totalDestruct = 0;
static Mutex *globalMutex = 0;
static void addThread(Thread *thread);
static void removeThread(Thread *thread);
static ThreadList *list;

static int64 getTotalConstruct()
{
    Lock xx(globalMutex);
    return totalConstruct;
}

static int64 getTotalDestruct()
{
    Lock xx(globalMutex);
    return totalDestruct;
}

static ConstructDestructCallback *pConstructDestructCallback;

static void init()
{
     static Mutex mutex = Mutex();
     Lock xx(&mutex);
     if(globalMutex==0) {
        globalMutex = new Mutex();
        list = new ThreadList();
        pConstructDestructCallback = new ConstructDestructCallback(
            String("thread"),
            getTotalConstruct,getTotalDestruct,0);
     }
}


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


class Runnable : public ThreadReady {
public:
    Runnable(Thread *thread,String name,
        ThreadPriority priority, RunnableReady *runnable);
    virtual ~Runnable();
    Thread *start();
    void ready();
public: // only used within this source module
    Thread *thread;
    String name;
    ThreadPriority priority;
    RunnableReady *runnable;
    Event waitStart;
    bool isReady;
    epicsThreadId id;
};

extern "C" void myFunc ( void * pPvt )
{
    Runnable *runnable = (Runnable *)pPvt;
    runnable->waitStart.signal();
    addThread(runnable->thread);
    runnable->runnable->run(runnable);
    removeThread(runnable->thread);
}

Runnable::Runnable(Thread *thread,String name,
    ThreadPriority priority, RunnableReady *runnable)
: thread(thread),name(name),priority(priority),
  runnable(runnable),
  waitStart(eventEmpty),
  isReady(false),
  id(epicsThreadCreate(
        name.c_str(),
        epicsPriority[priority],
        epicsThreadGetStackSize(epicsThreadStackSmall),
        myFunc,this))
{
    init();
    Lock xx(globalMutex);
    totalConstruct++;
}

Runnable::~Runnable()
{
    Lock xx(globalMutex);
    totalDestruct++;
}


Thread * Runnable::start()
{
    if(!waitStart.wait(10.0)) {
        fprintf(stderr,"thread %s did not call ready\n",thread->getName().c_str());
    }
    return thread;
}


void Runnable::ready()
{
    waitStart.signal();
}

Thread::Thread(String name,ThreadPriority priority,RunnableReady *runnableReady)
: pImpl(new Runnable(this,name,priority,runnableReady))
{
}

Thread::~Thread()
{
   delete pImpl;
}

ConstructDestructCallback *Thread::getConstructDestructCallback()
{
    init();
    return pConstructDestructCallback;
}

void Thread::start()
{
    pImpl->start();
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
    init();
    Lock xx(globalMutex);
    ThreadListNode *node = list->getHead();    
    while(node!=0) {
        Thread *thread = node->getObject()->thread;
        *buf += thread->getName();
        *buf += " ";
        *buf += threadPriorityNames[thread->getPriority()];
        *buf += "\n";
        node = list->getNext(node);
    }
}

void addThread(Thread *thread)
{
    Lock xx(globalMutex);
    ThreadListElement *element = new ThreadListElement(thread);
    list->addTail(element->node);
}

void removeThread(Thread *thread)
{
    Lock xx(globalMutex);
    ThreadListNode *node = list->getHead();
    while(node!=0) {
        if(node->getObject()->thread==thread) {
             list->remove(node);
             delete node;
             return;
        }
        node = list->getNext(node);
    }
    fprintf(stderr,"removeThread but thread %s did not in list\n",
        thread->getName().c_str());
}

}}
