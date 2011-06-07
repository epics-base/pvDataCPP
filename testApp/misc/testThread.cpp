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
#include <epicsExit.h>

#include <pv/event.h>
#include <pv/thread.h>
#include <pv/executor.h>
#include <pv/CDRMonitor.h>
#include <pv/timeFunction.h>

using namespace epics::pvData;

class Action : public Runnable {
public:
    FILE *out;
    bool actuallyRan;
    Event begin, end;
    Action(FILE* fp): out(fp), actuallyRan(false) {}
    virtual void run() {
        fprintf(out, "Action waiting\n");
        begin.signal();
        bool waited=end.wait();
        actuallyRan=true;
        fprintf(out, "Action1 %s\n", waited?"true":"false");
    }
};

static void testThreadRun(FILE *fd) {
    // show that we can control thread start and stop
    Action ax(fd);
    {
        Thread tr("Action", lowPriority, &ax);
        bool w=ax.begin.wait();
        fprintf(fd, "main1 %s\n", w?"true":"false");
        fprintf(fd, "Action is %s\n", ax.actuallyRan?"true":"false");
        ax.end.signal();
    }
    fprintf(fd, "Action is %s\n", ax.actuallyRan?"true":"false");
}

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
    testThreadRun(fd);
    testBasic(fd);
    testThreadContext(fd,auxFd);
    epicsExitCallAtExits();
    CDRMonitor::get().show(fd);
    return (0);
}
