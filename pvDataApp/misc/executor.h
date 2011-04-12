/* executor.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef EXECUTOR_H
#define EXECUTOR_H
#include <memory>
#include <vector>
#include "noDefaultMethods.h"
#include "pvType.h"
#include "thread.h"

namespace epics { namespace pvData { 

// This is created by Executor.createNode and passed to Executor.execute
class ExecutorNode;

class Command {
public:
    virtual ~Command(){}
    virtual void command() = 0;
};

class Executor : private NoDefaultMethods {
public:
    Executor(String threadName,ThreadPriority priority);
    ~Executor();
    ExecutorNode * createNode(Command *command);
    void execute(ExecutorNode *node);
private:
    class ExecutorPvt *pImpl;
};

}}
#endif  /* EXECUTOR_H */
