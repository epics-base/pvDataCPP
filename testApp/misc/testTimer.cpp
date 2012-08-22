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

#include <epicsAssert.h>
#include <epicsExit.h>
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
    MyCallback(String name,FILE *fd,FILE *auxfd,EventPtr const & wait)
    : name(name),
      fd(fd),
      auxfd(auxfd),
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
        fprintf(fd,"timerStopped %s\n",name.c_str());
    }
    TimeStamp &getTimeStamp() { return timeStamp;}
private:
    String name;
    FILE *fd;
    FILE *auxfd;
    EventPtr wait;
    TimeStamp timeStamp;
};

static void testBasic(FILE *fd, FILE *auxfd)
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
    MyCallbackPtr callbackOne(new MyCallback(one,fd,auxfd,eventOne));
    MyCallbackPtr callbackTwo(new MyCallback(two,fd,auxfd,eventTwo));
    MyCallbackPtr callbackThree(new MyCallback(three,fd,auxfd,eventThree));
    for(int n=0; n<ntimes; n++) {
        currentTimeStamp.getCurrent();
        assert(!timer->isScheduled(callbackOne));
        assert(!timer->isScheduled(callbackTwo));
        assert(!timer->isScheduled(callbackThree));
        timer->scheduleAfterDelay(callbackOne,oneDelay);
        timer->scheduleAfterDelay(callbackTwo,twoDelay);
        timer->scheduleAfterDelay(callbackThree,threeDelay);
        if(oneDelay>.1) assert(timer->isScheduled(callbackOne));
        if(twoDelay>.1) assert(timer->isScheduled(callbackTwo));
        if(threeDelay>.1) assert(timer->isScheduled(callbackThree));
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
            fprintf(auxfd,"one requested %f  actual %f delta %f\n",
                 oneDelay,diff,delta);
        }
        if(delta<0.0) delta = -delta;
        assert(delta<.1);
        diff = TimeStamp::diff(
            callbackTwo->getTimeStamp(),currentTimeStamp);
        delta = diff - twoDelay;
        if(debug) {
            fprintf(auxfd,"two requested %f  actual %f delta %f\n",
                twoDelay,diff,delta);
        }
        if(delta<0.0) delta = -delta;
        assert(delta<.1);
        diff = TimeStamp::diff(
            callbackThree->getTimeStamp(),currentTimeStamp);
        delta = diff - threeDelay;
        if(debug) {
            fprintf(auxfd,"three requested %f  actual %f delta %f\n",
                threeDelay,diff,delta);
        }
        if(delta<0.0) delta = -delta;
        assert(delta<.1);
    }
    fprintf(fd,"testBasic PASSED\n");
}

static void testCancel(FILE *fd, FILE *auxfd)
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
    MyCallbackPtr callbackOne(new MyCallback(one,fd,auxfd,eventOne));
    MyCallbackPtr callbackTwo(new MyCallback(two,fd,auxfd,eventTwo));
    MyCallbackPtr callbackThree(new MyCallback(three,fd,auxfd,eventThree));
    for(int n=0; n<ntimes; n++) {
        currentTimeStamp.getCurrent();
        assert(!timer->isScheduled(callbackOne));
        assert(!timer->isScheduled(callbackTwo));
        assert(!timer->isScheduled(callbackThree));
        timer->scheduleAfterDelay(callbackOne,oneDelay);
        timer->scheduleAfterDelay(callbackTwo,twoDelay);
        timer->scheduleAfterDelay(callbackThree,threeDelay);
        timer->cancel(callbackTwo);
        if(oneDelay>.1) assert(timer->isScheduled(callbackOne));
        assert(!timer->isScheduled(callbackTwo));
        if(threeDelay>.1) assert(timer->isScheduled(callbackThree));
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
            fprintf(auxfd,"one requested %f  actual %f delta %f\n",
                 oneDelay,diff,delta);
        }
        if(delta<0.0) delta = -delta;
        assert(delta<.1);
        diff = TimeStamp::diff(
            callbackThree->getTimeStamp(),currentTimeStamp);
        delta = diff - threeDelay;
        if(debug) {
            fprintf(auxfd,"three requested %f  actual %f delta %f\n",
                threeDelay,diff,delta);
        }
        if(delta<0.0) delta = -delta;
        assert(delta<.1);
    }
    fprintf(fd,"testCancel PASSED\n");
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
    FILE *auxfd = stdout;
    if(auxFileName!=0 && auxFileName[0]!=0) {
        auxfd = fopen(auxFileName,"w+");
    }
    oneDelay = .4;
    twoDelay = .2;
    threeDelay = .1;
    fprintf(fd,"oneDelay %f twoDelay %f threeDelay %f\n",
         oneDelay,twoDelay,threeDelay);
    testBasic(fd,auxfd);
    testCancel(fd,auxfd);
    oneDelay = .1;
    twoDelay = .2;
    threeDelay = .4;
    fprintf(fd,"oneDelay %f twoDelay %f threeDelay %f\n",
         oneDelay,twoDelay,threeDelay);
    testBasic(fd,auxfd);
    testCancel(fd,auxfd);
    oneDelay = .1;
    twoDelay = .4;
    threeDelay = .2;
    fprintf(fd,"oneDelay %f twoDelay %f threeDelay %f\n",
         oneDelay,twoDelay,threeDelay);
    testBasic(fd,auxfd);
    testCancel(fd,auxfd);
    oneDelay = .0;
    twoDelay = .0;
    threeDelay = .0;
    fprintf(fd,"oneDelay %f twoDelay %f threeDelay %f\n",
         oneDelay,twoDelay,threeDelay);
    testBasic(fd,auxfd);
    testCancel(fd,auxfd);
    return (0);
}
