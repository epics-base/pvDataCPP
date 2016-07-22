/* executor.h */
/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
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

#include <shareLib.h>

namespace epics { namespace pvData { 

class Command;
class Executor;
typedef std::tr1::shared_ptr<Command> CommandPtr;
typedef std::tr1::shared_ptr<Executor> ExecutorPtr;

/**
 * @brief A command to be called by Executor
 *
 */
class epicsShareClass Command {
public:
    POINTER_DEFINITIONS(Command);
    /**
     * 
     * Destructor
     */
    virtual ~Command(){}
    /**
     * 
     * The command that is executed.
     */
    virtual void command() = 0;
private:
    CommandPtr next;
    friend class Executor;
};

/**
 * @brief A class that executes commands.
 *
 */
class epicsShareClass Executor : public Runnable{
public:
    POINTER_DEFINITIONS(Executor);
    /**
     * Constructor
     *
     * @param threadName name for the executor thread.
     * @param priority The thread priority.
     */
    Executor(std::string const & threadName,ThreadPriority priority);
    /**
     * Destructor
     */
    ~Executor();
    /**
     * 
     * Request to execute a command.
     * @param command A shared pointer to the command instance.
     */
    void execute(CommandPtr const &command);
    /**
     * 
     * The thread run method.
     */
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
