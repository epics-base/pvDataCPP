/* executor.cpp */
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

#include <memory>
#include <vector>
#include "linkedList.h"
#include "lock.h"
#include "thread.h"
#include "event.h"
#include "executor.h"
#include "CDRMonitor.h"

namespace epics { namespace pvData {

PVDATA_REFCOUNT_MONITOR_DEFINE(executor);

typedef LinkedListNode<ExecutorNode> ExecutorListNode;
typedef LinkedList<ExecutorNode> ExecutorList;

class ExecutorNode {
public:
    ExecutorNode(Command *command);

    Command *command;
    ExecutorListNode node;
    ExecutorListNode runNode;
};

ExecutorNode::ExecutorNode(Command *command)
: command(command),
  node(this),
  runNode(this)
{}

class ExecutorPvt : public Runnable{
public:
    ExecutorPvt(String threadName,ThreadPriority priority);
    ~ExecutorPvt();
    ExecutorNode * createNode(Command *command);
    void execute(ExecutorNode *node);
    virtual void run();
private:
    ExecutorList executorList;
    ExecutorList runList;
    Event moreWork;
    Event stopped;
    Mutex mutex;
    volatile bool alive;
    Thread thread;
};

ExecutorPvt::ExecutorPvt(String threadName,ThreadPriority priority)
:  executorList(),
   runList(),
   moreWork(),
   stopped(),
   mutex(),
   alive(true),
   thread(threadName,priority,this)
{} 

ExecutorPvt::~ExecutorPvt()
{
    {
        Lock xx(&mutex);
        alive = false;
    }
    moreWork.signal();
    {
        Lock xx(&mutex);
        stopped.wait();
    }
    ExecutorListNode *node;
    while((node=executorList.removeHead())!=0) {
        delete node->getObject();
    }
}

void ExecutorPvt::run()
{
    while(alive) {
        ExecutorListNode * executorListNode = 0;
        while(alive && runList.isEmpty()) {
            moreWork.wait();
        }
        if(alive) {
            Lock xx(&mutex);
            executorListNode = runList.removeHead();
        }
        if(alive && executorListNode!=0) {
             executorListNode->getObject()->command->command();
        }
    }
    stopped.signal();
}

ExecutorNode * ExecutorPvt::createNode(Command *command)
{
    Lock xx(&mutex);
    ExecutorNode *executorNode = new ExecutorNode(command);
    executorList.addTail(&executorNode->node);
    return executorNode;
}

void ExecutorPvt::execute(ExecutorNode *node)
{
    Lock xx(&mutex);
    if(!alive || node->runNode.isOnList()) return;
    bool isEmpty = runList.isEmpty();
    runList.addTail(&node->runNode);
    if(isEmpty) moreWork.signal();
}

Executor::Executor(String threadName,ThreadPriority priority)
: pImpl(new ExecutorPvt(threadName,priority))
{
    PVDATA_REFCOUNT_MONITOR_CONSTRUCT(executor);
}

Executor::~Executor() {
    delete pImpl;
    PVDATA_REFCOUNT_MONITOR_DESTRUCT(executor);
}

ExecutorNode * Executor::createNode(Command*command)
{return pImpl->createNode(command);}

void Executor::execute(ExecutorNode *node) {pImpl->execute(node);}

}}
