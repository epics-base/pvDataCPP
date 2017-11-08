/* executor.cpp */
/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */
/**
 *  @author mrk
 */
#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>

#include <epicsEvent.h>
#include <epicsMutex.h>
#include <epicsThread.h>

// Suppress deprecation warnings for the implementation
#include <compilerDependencies.h>
#undef EPICS_DEPRECATED
#define EPICS_DEPRECATED

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
        while(!head.get()) {
            xx.unlock();
            moreWork.wait();
            xx.lock();
        }
        CommandPtr command = head;
        head = command->next;
        if(!command.get()) continue;
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
    if(!head.get()) {
        head = command;
        moreWork.signal();
        return;
    }
    CommandPtr tail = head;
    while(tail->next) tail = tail->next;
    tail->next = command;   
}

}}
