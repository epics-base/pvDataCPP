/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
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

static String actionName("action");

class Action;
typedef std::tr1::shared_ptr<Action> ActionPtr;

class Action : public Runnable {
public:
    virtual ~Action() {}
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
    ActionPtr ax(new Action(fd));
    {
        ThreadPtr tr(new Thread(actionName,lowPriority,ax.get()));
        bool w=ax->begin.wait();
        fprintf(fd, "main %s\n", w?"true":"false");
        fprintf(fd, "Action is %s\n", ax->actuallyRan?"true":"false");
        ax->end.signal();
    }
    fprintf(fd, "Action is %s\n", ax->actuallyRan?"true":"false");
    fprintf(fd,"testThreadRun PASSED\n");
}

class Basic;
typedef std::tr1::shared_ptr<Basic> BasicPtr;

class Basic :
     public Command,
     public std::tr1::enable_shared_from_this<Basic>
{
public:
    POINTER_DEFINITIONS(Basic);
    Basic(ExecutorPtr const &executor)
    : executor(executor) {}
    ~Basic()
    {
    }
    void run()
    {
        executor->execute(getPtrSelf());
        bool result = wait.wait();
        if(result==false) printf("basic::run wait returned false\n");
    }
    virtual void command()
    {
        wait.signal();
    }
private:
    Basic::shared_pointer getPtrSelf()
    {
        return shared_from_this();
    }
    ExecutorPtr executor;
    Event wait;
};

typedef std::tr1::shared_ptr<Basic> BasicPtr;

static void testBasic(FILE *fd) {
    ExecutorPtr executor(new Executor(String("basic"),middlePriority));
    BasicPtr basic( new Basic(executor));
    basic->run();
    fprintf(fd,"testBasic PASSED\n");
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

#ifdef TESTTHREADCONTEXT

static void testThreadContext(FILE *fd,FILE *auxFd) {
    ExecutorPtr executor(new Executor(String("basic"),middlePriority));
    BasicPtr basic(new Basic(executor));
    MyFuncPtr myFunc(new MyFunc(basic));
    TimeFunctionPtr timeFunction(new TimeFunction(myFunc));
    double perCall = timeFunction->timeCall();
    perCall *= 1e6;
    fprintf(auxFd,"time per call %f microseconds\n",perCall);
    fprintf(fd,"testThreadContext PASSED\n");
}
#endif

int main(int argc, char *argv[]) {
     char *fileName = 0;
    if(argc>1) fileName = argv[1];
    FILE * fd = stdout;
    if(fileName!=0 && fileName[0]!=0) {
        fd = fopen(fileName,"w+");
    }
    testThreadRun(fd);
    testBasic(fd);
#ifdef TESTTHREADCONTEXT
    char *auxFileName = 0;
    if(argc>2) auxFileName = argv[2];
    FILE *auxFd = stdout;
    if(auxFileName!=0 && auxFileName[0]!=0) {
        auxFd = fopen(auxFileName,"w+");
    }
    testThreadContext(fd,auxFd);
#endif
    return 0;
}
