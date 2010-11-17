/* executor.h */
#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>

#include <memory>
#include <vector>
#include "linkedList.h"
#include "lock.h"
#include "thread.h"
#include "event.h"
#include "executor.h"

namespace epics { namespace pvData {

static volatile int64 totalConstruct = 0;
static volatile int64 totalDestruct = 0;
static Mutex *globalMutex = 0;

class ConstructDestructCallbackExecutor : public ConstructDestructCallback {
public:
    ConstructDestructCallbackExecutor();
    virtual String getConstructName();
    virtual int64 getTotalConstruct();
    virtual int64 getTotalDestruct();
    virtual int64 getTotalReferenceCount();
private:
    String name;
};

ConstructDestructCallbackExecutor::ConstructDestructCallbackExecutor()
: name("executor")
{
    getShowConstructDestruct()->registerCallback(this);
}

String ConstructDestructCallbackExecutor::getConstructName() {return name;}

int64 ConstructDestructCallbackExecutor::getTotalConstruct()
{
    Lock xx(globalMutex);
    return totalConstruct;
}

int64 ConstructDestructCallbackExecutor::getTotalDestruct()
{
    Lock xx(globalMutex);
    return totalDestruct;
}
int64 ConstructDestructCallbackExecutor::getTotalReferenceCount()

{
    return 0;
}


static ConstructDestructCallback *pConstructDestructCallback;

static void init() {
    static Mutex mutex = Mutex();
    Lock xx(&mutex);
    if(globalMutex==0) {
        globalMutex = new Mutex();
        pConstructDestructCallback = new ConstructDestructCallbackExecutor();
    }
}


typedef LinkedListNode<ExecutorNode> ExecutorListNode;
typedef LinkedList<ExecutorNode> ExecutorList;

class ExecutorNode {
public:
    ExecutorNode(Command *command);
    ~ExecutorNode();

    Command *command;
    ExecutorListNode *node;
    ExecutorListNode *runNode;
};

ExecutorNode::ExecutorNode(Command *command)
: command(command),
  node(new ExecutorListNode(this)),
  runNode(new ExecutorListNode(this))
{}

ExecutorNode::~ExecutorNode()
{
    delete node;
    delete runNode;
}

ConstructDestructCallback *Executor::getConstructDestructCallback()
{
    init();
    return pConstructDestructCallback;
}

class ExecutorPvt : public RunnableReady {
public:
    ExecutorPvt(String threadName,ThreadPriority priority);
    ~ExecutorPvt();
    ExecutorNode * createNode(Command *command);
    void execute(ExecutorNode *node);
    void destroy();
    virtual void run(ThreadReady *threadReady);
private:
    ExecutorList *executorList; 
    ExecutorList *runList; 
    Event *moreWork;
    Event *stopped;
    Mutex mutex;
    volatile bool alive;
    Thread *thread;
};

ExecutorPvt::ExecutorPvt(String threadName,ThreadPriority priority)
:  executorList(new ExecutorList()),
   runList(new ExecutorList()),
   moreWork(new Event(eventEmpty)),
   stopped(new Event(eventEmpty)),
   mutex(Mutex()),
   alive(true),
   thread(new Thread(threadName,priority,this))
{
   thread->start();
}

ExecutorPvt::~ExecutorPvt()
{
    ExecutorListNode *node;
    while((node=executorList->removeHead())!=0) {
        delete node->getObject();
    }
    delete stopped;
    delete moreWork;
    delete runList;
    delete executorList;
    delete thread;
}

void ExecutorPvt::run(ThreadReady *threadReady)
{
    bool firstTime = true;
    while(alive) {
        ExecutorListNode * executorListNode = 0;
        if(firstTime) {
            firstTime = false;
            threadReady->ready();
        }
        while(alive && runList->isEmpty()) {
            moreWork->wait();
        }
        if(alive) {
            Lock xx(&mutex);
            executorListNode = runList->removeHead();
        }
        if(alive && executorListNode!=0) {
             executorListNode->getObject()->command->command();
        }
    }
    stopped->signal();
}

ExecutorNode * ExecutorPvt::createNode(Command *command)
{
    Lock xx(&mutex);
    ExecutorNode *executorNode = new ExecutorNode(command);
    executorList->addTail(executorNode->node);
    return executorNode;
}

void ExecutorPvt::execute(ExecutorNode *node)
{
    Lock xx(&mutex);
    if(!alive || node->runNode->isOnList()) return;
    bool isEmpty = runList->isEmpty();
    runList->addTail(node->runNode);
    if(isEmpty) moreWork->signal();
}

void ExecutorPvt::destroy()
{
    {
        Lock xx(&mutex);
        alive = false;
    }
    moreWork->signal();
    {
        Lock xx(&mutex);
        stopped->wait();
    }
    delete this;
}

Executor::Executor(String threadName,ThreadPriority priority)
: pImpl(new ExecutorPvt(threadName,priority))
{
    init();
    Lock xx(globalMutex);
    totalConstruct++;
}

Executor::~Executor() {
    Lock xx(globalMutex);
    totalDestruct++;
}


ExecutorNode * Executor::createNode(Command*command)
{return pImpl->createNode(command);}

void Executor::execute(ExecutorNode *node) {pImpl->execute(node);}
void Executor::destroy() {
    pImpl->destroy();
    delete this;
}


}}
