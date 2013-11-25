/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/*
 * testTimer.cpp
 *
 *  Created on: 2010.11
 *      Author: Marty Kraimer
 */

#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>

#include <epicsUnitTest.h>
#include <testMain.h>

#include <pv/timeStamp.h>
#include <pv/event.h>
#include <pv/timer.h>
#include <pv/thread.h>

using namespace epics::pvData;

static TimeStamp currentTimeStamp;
static double oneDelay = 4.0;
static double twoDelay = 2.0;
static double threeDelay = 1.0;
static int ntimes = 3;
static bool debug = false;

class MyCallback;
typedef std::tr1::shared_ptr<MyCallback> MyCallbackPtr;

class MyCallback : public TimerCallback {
public:
    POINTER_DEFINITIONS(MyCallback);
    MyCallback(String name,EventPtr const & wait)
    : name(name),
      wait(wait)
    {
    }
    ~MyCallback()
    {
    }
    virtual void callback()
    {
        timeStamp.getCurrent();
        wait->signal();
    }
    virtual void timerStopped()
    {
        printf("timerStopped %s\n",name.c_str());
    }
    TimeStamp &getTimeStamp() { return timeStamp;}
private:
    String name;
    EventPtr wait;
    TimeStamp timeStamp;
};

static void testBasic()
{
    if(debug) {
        printf("\n\ntestBasic oneDelay %lf twoDelay %lf threeDaley %lf\n",
            oneDelay,twoDelay,threeDelay);
    }
    String one("one");
    String two("two");
    String three("three");
    EventPtr eventOne(new Event());
    EventPtr eventTwo(new Event());
    EventPtr eventThree(new Event());
    TimerPtr timer(new Timer(String("timer"),middlePriority));
    MyCallbackPtr callbackOne(new MyCallback(one,eventOne));
    MyCallbackPtr callbackTwo(new MyCallback(two,eventTwo));
    MyCallbackPtr callbackThree(new MyCallback(three,eventThree));
    for(int n=0; n<ntimes; n++) {
        currentTimeStamp.getCurrent();
        testOk1(!timer->isScheduled(callbackOne));
        testOk1(!timer->isScheduled(callbackTwo));
        testOk1(!timer->isScheduled(callbackThree));
        timer->scheduleAfterDelay(callbackOne,oneDelay);
        timer->scheduleAfterDelay(callbackTwo,twoDelay);
        timer->scheduleAfterDelay(callbackThree,threeDelay);
        if(oneDelay>.1) testOk1(timer->isScheduled(callbackOne));
        if(twoDelay>.1) testOk1(timer->isScheduled(callbackTwo));
        if(threeDelay>.1) testOk1(timer->isScheduled(callbackThree));
        if(debug) {
            String builder;
            timer->toString(&builder);
            printf("timerQueue\n%s",builder.c_str());
        }
        eventOne->wait();
        eventTwo->wait();
        eventThree->wait();
        double diff;
        double delta;
        diff = TimeStamp::diff(
            callbackOne->getTimeStamp(),currentTimeStamp);
        delta = diff - oneDelay;
        if(debug) {
            printf("one requested %f  actual %f delta %f\n",
                 oneDelay,diff,delta);
        }
        if(delta<0.0) delta = -delta;
        testOk1(delta<.1);
        diff = TimeStamp::diff(
            callbackTwo->getTimeStamp(),currentTimeStamp);
        delta = diff - twoDelay;
        if(debug) {
            printf("two requested %f  actual %f delta %f\n",
                twoDelay,diff,delta);
        }
        if(delta<0.0) delta = -delta;
        testOk1(delta<.1);
        diff = TimeStamp::diff(
            callbackThree->getTimeStamp(),currentTimeStamp);
        delta = diff - threeDelay;
        if(debug) {
            printf("three requested %f  actual %f delta %f\n",
                threeDelay,diff,delta);
        }
        if(delta<0.0) delta = -delta;
        testOk1(delta<.1);
    }
    printf("testBasic PASSED\n");
}

static void testCancel()
{
    if(debug) {
        printf("\n\ntestCancel oneDelay %lf twoDelay %lf threeDaley %lf\n",
            oneDelay,twoDelay,threeDelay);
    }
    String one("one");
    String two("two");
    String three("three");
    EventPtr eventOne(new Event());
    EventPtr eventTwo(new Event());
    EventPtr eventThree(new Event());
    TimerPtr timer(new Timer(String("timer"),middlePriority));
    MyCallbackPtr callbackOne(new MyCallback(one,eventOne));
    MyCallbackPtr callbackTwo(new MyCallback(two,eventTwo));
    MyCallbackPtr callbackThree(new MyCallback(three,eventThree));
    for(int n=0; n<ntimes; n++) {
        currentTimeStamp.getCurrent();
        testOk1(!timer->isScheduled(callbackOne));
        testOk1(!timer->isScheduled(callbackTwo));
        testOk1(!timer->isScheduled(callbackThree));
        timer->scheduleAfterDelay(callbackOne,oneDelay);
        timer->scheduleAfterDelay(callbackTwo,twoDelay);
        timer->scheduleAfterDelay(callbackThree,threeDelay);
        timer->cancel(callbackTwo);
        if(oneDelay>.1) testOk1(timer->isScheduled(callbackOne));
        testOk1(!timer->isScheduled(callbackTwo));
        if(threeDelay>.1) testOk1(timer->isScheduled(callbackThree));
        if(debug) {
            String builder;
            timer->toString(&builder);
            printf("timerQueue\n%s",builder.c_str());
        }
        eventOne->wait();
        eventThree->wait();
        double diff;
        double delta;
        diff = TimeStamp::diff(
            callbackOne->getTimeStamp(),currentTimeStamp);
        delta = diff - oneDelay;
        if(debug) {
            printf("one requested %f  actual %f delta %f\n",
                 oneDelay,diff,delta);
        }
        if(delta<0.0) delta = -delta;
        testOk1(delta<.1);
        diff = TimeStamp::diff(
            callbackThree->getTimeStamp(),currentTimeStamp);
        delta = diff - threeDelay;
        if(debug) {
            printf("three requested %f  actual %f delta %f\n",
                threeDelay,diff,delta);
        }
        if(delta<0.0) delta = -delta;
        testOk1(delta<.1);
    }
    printf("testCancel PASSED\n");
}

MAIN(testTimer)
{
    testPlan(171);
    testDiag("Tests timer");
    oneDelay = .4;
    twoDelay = .2;
    threeDelay = .1;
    printf("oneDelay %f twoDelay %f threeDelay %f\n",
         oneDelay,twoDelay,threeDelay);
    testBasic();
    testCancel();
    oneDelay = .1;
    twoDelay = .2;
    threeDelay = .4;
    printf("oneDelay %f twoDelay %f threeDelay %f\n",
         oneDelay,twoDelay,threeDelay);
    testBasic();
    testCancel();
    oneDelay = .1;
    twoDelay = .4;
    threeDelay = .2;
    printf("oneDelay %f twoDelay %f threeDelay %f\n",
         oneDelay,twoDelay,threeDelay);
    testBasic();
    testCancel();
    oneDelay = .0;
    twoDelay = .0;
    threeDelay = .0;
    printf("oneDelay %f twoDelay %f threeDelay %f\n",
         oneDelay,twoDelay,threeDelay);
    testBasic();
    testCancel();
    return testDone();
}
