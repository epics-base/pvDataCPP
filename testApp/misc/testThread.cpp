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

#include <epicsUnitTest.h>
#include <testMain.h>


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
    Action(): actuallyRan(false) {}
    virtual void run() {
        printf("Action waiting\n");
        begin.signal();
        bool waited=end.wait();
        actuallyRan=true;
        printf("Action %s\n", waited?"true":"false");
    }
};

static void testThreadRun() {
    // show that we can control thread start and stop
    ActionPtr ax(new Action());
    {
        ThreadPtr tr(new Thread(actionName,lowPriority,ax.get()));
        bool w=ax->begin.wait();
        printf( "main %s\n", w?"true":"false");
        printf( "Action is %s\n", ax->actuallyRan?"true":"false");
        ax->end.signal();
    }
    testOk1(ax->actuallyRan==true);
    printf( "Action is %s\n", ax->actuallyRan?"true":"false");
    printf("testThreadRun PASSED\n");
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
        testOk1(result==true);
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

static void testBasic() {
    ExecutorPtr executor(new Executor(String("basic"),middlePriority));
    BasicPtr basic( new Basic(executor));
    basic->run();
    printf("testBasic PASSED\n");
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

static void testThreadContext() {
    ExecutorPtr executor(new Executor(String("basic"),middlePriority));
    BasicPtr basic(new Basic(executor));
    MyFuncPtr myFunc(new MyFunc(basic));
    TimeFunctionPtr timeFunction(new TimeFunction(myFunc));
    double perCall = timeFunction->timeCall();
    perCall *= 1e6;
    printf("time per call %f microseconds\n",perCall);
    printf("testThreadContext PASSED\n");
}
#endif

MAIN(testThread)
{
    testPlan(2);
    testDiag("Tests thread");
    testThreadRun();
    testBasic();
#ifdef TESTTHREADCONTEXT
    testThreadContext();
#endif
    return testDone();
}
