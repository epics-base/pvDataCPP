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
        fprintf(out, "Action %s\n", waited?"true":"false");
    }
};

static void testThreadRun(FILE *fd) {
    // show that we can control thread start and stop
    Action ax(fd);
    {
        Thread tr("Action", lowPriority, &ax);
        bool w=ax.begin.wait();
        fprintf(fd, "main %s\n", w?"true":"false");
        fprintf(fd, "Action is %s\n", ax.actuallyRan?"true":"false");
        ax.end.signal();
    }
    fprintf(fd, "Action is %s\n", ax.actuallyRan?"true":"false");
}

class Basic :
     public Command,
     public std::tr1::enable_shared_from_this<Basic>
{
public:
    POINTER_DEFINITIONS(Basic);
    Basic(ExecutorPtr const &executor);
    ~Basic();
    void run();
    virtual void command();
private:
    Basic::shared_pointer getPtrSelf()
    {
        return shared_from_this();
    }
    ExecutorPtr executor;
    Event wait;
};

typedef std::tr1::shared_ptr<Basic> BasicPtr;

Basic::Basic(ExecutorPtr const &executor)
: executor(executor)
{
}

Basic::~Basic() {
}

void Basic::run()
{
    executor->execute(getPtrSelf());
    bool result = wait.wait();
    if(result==false) printf("basic::run wait returned false\n");
}

void Basic::command()
{
    wait.signal();
}


static void testBasic(FILE *fd) {
    ExecutorPtr executor( new Executor(String("basic"),middlePriority));
    BasicPtr basic( new Basic(executor));
    basic->run();
}

class MyFunc : public TimeFunctionRequester {
public:
    POINTER_DEFINITIONS(MyFunc);
    MyFunc(BasicPtr const &basic);
    virtual void function();
private:
    BasicPtr basic;
};

MyFunc::MyFunc(BasicPtr const &basic)
    : basic(basic)
    {}
void MyFunc::function()
{
    basic->run();
}


typedef std::tr1::shared_ptr<MyFunc> MyFuncPtr;

static void testThreadContext(FILE *fd,FILE *auxFd) {
    ExecutorPtr executor(new Executor(String("basic"),middlePriority));
    BasicPtr basic(new Basic(executor));
    MyFuncPtr myFunc(new MyFunc(basic));
    TimeFunctionPtr timeFunction(new TimeFunction(myFunc));
    double perCall = timeFunction->timeCall();
    perCall *= 1e6;
    fprintf(auxFd,"time per call %f microseconds\n",perCall);
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
    return 0;
}
