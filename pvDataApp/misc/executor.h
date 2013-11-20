/* executor.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <memory>

#include <pv/pvType.h>
#include <pv/lock.h>
#include <pv/event.h>
#include <pv/thread.h>
#include <pv/sharedPtr.h>

#include <sharelib.h>

namespace epics { namespace pvData { 

class Command;
class Executor;
typedef std::tr1::shared_ptr<Command> CommandPtr;
typedef std::tr1::shared_ptr<Executor> ExecutorPtr;

class epicsShareClass Command {
public:
    POINTER_DEFINITIONS(Command);
    virtual ~Command(){}
    virtual void command() = 0;
private:
    CommandPtr next;
    friend class Executor;
};

class epicsShareClass Executor : public Runnable{
public:
    POINTER_DEFINITIONS(Executor);
    Executor(String threadName,ThreadPriority priority);
    ~Executor();
    void execute(CommandPtr const &node);
    virtual void run();
private:
    CommandPtr head;
    CommandPtr tail;
    epics::pvData::Mutex mutex;
    epics::pvData::Event moreWork;
    epics::pvData::Event stopped;
    epics::pvData::Thread thread;
};

}}
#endif  /* EXECUTOR_H */
