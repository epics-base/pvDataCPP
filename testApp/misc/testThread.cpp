/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/*
 * testThread.cpp
 *
 *  Created on: 2010.11
 *      Author: Marty Kraimer
 */

#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>
#include <list>

#include <epicsAssert.h>

#include "event.h"
#include "thread.h"
#include "executor.h"
#include "showConstructDestruct.h"
#include "timeFunction.h"

using namespace epics::pvData;


class Basic : public Command {
public:
    Basic(Executor *executor);
    ~Basic();
    void run();
    virtual void command();
private:
    Executor *executor;
    ExecutorNode *executorNode;
    Event *wait;
};

Basic::Basic(Executor *executor)
: executor(executor),
  executorNode(executor->createNode(this)),
  wait(new Event())
{
}

Basic::~Basic() {
    delete wait;
}

void Basic::run()
{
    executor->execute(executorNode);
    bool result = wait->wait();
    if(result==false) printf("basic::run wait returned false\n");
}

void Basic::command()
{
    wait->signal();
}


static void testBasic(FILE *fd) {
    Executor *executor = new Executor(String("basic"),middlePriority);
    Basic *basic = new Basic(executor);
    basic->run();
    delete basic; 
    String buf("");
    Thread::showThreads(&buf);
    fprintf(fd,"threads\n%s\n",buf.c_str());
    delete executor;
}

class MyFunc : public TimeFunctionRequester {
public:
    MyFunc(Basic *basic)
    : basic(basic)
    {}
    virtual void function()
    {
        basic->run();
    }
private:
    Basic *basic;
};

static void testThreadContext(FILE *fd,FILE *auxFd) {
    Executor *executor = new Executor(String("basic"),middlePriority);
    Basic *basic = new Basic(executor);
    MyFunc myFunc(basic);
    TimeFunction timeFunction(&myFunc);
    double perCall = timeFunction.timeCall();
    perCall *= 1e6;
    fprintf(auxFd,"time per call %f microseconds\n",perCall);
    delete basic;
    delete executor;
}

int main(int argc, char *argv[]) {
     char *fileName = 0;
    if(argc>1) fileName = argv[1];
    FILE * fd = stdout;
    if(fileName!=0 && fileName[0]!=0) {
        fd = fopen(fileName,"w+");
    }
    char *auxFileName = 0;
    if(argc>2) auxFileName = argv[2];
    FILE *auxFd = stdout;
    if(auxFileName!=0 && auxFileName[0]!=0) {
        auxFd = fopen(auxFileName,"w+");
    }
    testBasic(fd);
    testThreadContext(fd,auxFd);
    getShowConstructDestruct()->constuctDestructTotals(fd);
    return (0);
}
