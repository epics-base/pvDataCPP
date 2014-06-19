/* executor.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>

#define epicsExportSharedSymbols
#include <pv/executor.h>

using std::string;

namespace epics { namespace pvData {

// special instance to stop the executor thread
class ExecutorShutdown : public Command {
    virtual void command();
};

void ExecutorShutdown::command()
{
}

static
std::tr1::shared_ptr<Command> shutdown(new ExecutorShutdown());


Executor::Executor(string const & threadName,ThreadPriority priority)
:  thread(threadName,priority,this)
{
} 

Executor::~Executor()
{
    execute(shutdown);
    stopped.wait();
    // The thread signals 'stopped' while still holding
    // the lock.  By taking it we wait for the run() function
    // to actually return
    Lock xx(mutex);
    head.reset();
    tail.reset();
}

void Executor::run()
{
    Lock xx(mutex);
    while(true) {
        while(head.get()==NULL) {
            xx.unlock();
            moreWork.wait();
            xx.lock();
        }
        CommandPtr command = head;
        head = command->next;
        if(command.get()==NULL) continue;
        if(command.get()==shutdown.get()) break;
        xx.unlock();
        try {
            command->command();
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

void Executor::execute(CommandPtr const & command)
{
    Lock xx(mutex);
    command->next.reset();
    if(head.get()==NULL) {
        head = command;
        moreWork.signal();
        return;
    }
    CommandPtr tail = head;
    while(tail->next!=NULL) tail = tail->next;
    tail->next = command;   
}

}}
