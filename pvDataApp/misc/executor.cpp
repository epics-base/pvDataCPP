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
#include <pv/linkedList.h>
#include <pv/lock.h>
#include <pv/thread.h>
#include <pv/event.h>
#include <pv/executor.h>
#include <pv/CDRMonitor.h>

namespace epics { namespace pvData {

// special instance to stop the executor thread
static
class ExecutorShutdown : public Command {
    virtual void command(){};
} executorShutdown;

static
Command *shutdown=&executorShutdown;

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
  node(*this),
  runNode(*this)
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
    Thread thread;
};

ExecutorPvt::ExecutorPvt(String threadName,ThreadPriority priority)
:  executorList(),
   runList(),
   moreWork(),
   stopped(),
   mutex(),
   thread(threadName,priority,this)
{} 

ExecutorPvt::~ExecutorPvt()
{
    ExecutorNode shutdownNode(shutdown);

    execute(&shutdownNode);
    stopped.wait();

    // The thread signals 'stopped' while still holding
    // the lock.  By taking it we wait for the run() function
    // to actually return
    Lock xx(mutex);

    ExecutorListNode *node;
    while((node=executorList.removeHead())!=0) {
        delete &node->getObject();
    }
}

void ExecutorPvt::run()
{
    Lock xx(mutex);
    while(true) {
        ExecutorListNode * executorListNode = 0;
        while(runList.isEmpty()) {
            xx.unlock();
            moreWork.wait();
            xx.lock();
        }
        executorListNode = runList.removeHead();

        if(!executorListNode) continue;
        Command *cmd=executorListNode->getObject().command;

        if(cmd==shutdown) break;

        xx.unlock();
        try {
            executorListNode->getObject().command->command();
        }catch(std::exception& e){
            //TODO: feed into logging mechanism
            fprintf(stderr, "Executor: Unhandled exception: %s",e.what());
        }catch(...){
            fprintf(stderr, "Executor: Unhandled exception");
        }

        xx.lock();
    }

    stopped.signal();
}

ExecutorNode * ExecutorPvt::createNode(Command *command)
{
    Lock xx(mutex);
    ExecutorNode *executorNode = new ExecutorNode(command);
    executorList.addTail(executorNode->node);
    return executorNode;
}

void ExecutorPvt::execute(ExecutorNode *node)
{
    Lock xx(mutex);
    if(node->runNode.isOnList()) return;
    bool isEmpty = runList.isEmpty();
    runList.addTail(node->runNode);
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
