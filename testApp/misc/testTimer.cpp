/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
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
#include "timeStamp.h"
#include "event.h"
#include "timer.h"
#include "thread.h"
#include "showConstructDestruct.h"

using namespace epics::pvData;

static TimeStamp currentTimeStamp;
static double oneDelay = 4.0;
static double twoDelay = 2.0;

class MyCallback : public TimerCallback {
public:
    MyCallback(String name,FILE *fd,FILE *auxfd,Event *wait)
    : name(name),fd(fd),auxfd(auxfd),wait(wait),
      timerNode(new TimerNode(this)),timeStamp(TimeStamp())
    {
    }
    ~MyCallback()
    {
        delete timerNode;
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
    TimerNode *getTimerNode() { return timerNode;}
    TimeStamp &getTimeStamp() { return timeStamp;}
private:
    String name;
    FILE *fd;
    FILE *auxfd;
    Event *wait;
    TimerNode *timerNode;
    TimeStamp timeStamp;
};

static void testBasic(FILE *fd, FILE *auxfd)
{
    String one("one");
    String two("two");
    Event *eventOne = new Event();
    Event *eventTwo = new Event();
    Timer *timer = new Timer(String("timer"),middlePriority);
    MyCallback *callbackOne = new MyCallback(
        one,fd,auxfd,eventOne);
    MyCallback *callbackTwo = new MyCallback(
        two,fd,auxfd,eventTwo);
    currentTimeStamp.getCurrent();
    timer->scheduleAfterDelay(callbackOne->getTimerNode(),oneDelay);
    timer->scheduleAfterDelay(callbackTwo->getTimerNode(),twoDelay);
    eventOne->wait();
    eventTwo->wait();
    double diff;
    diff = TimeStamp::diff(
        callbackOne->getTimeStamp(),currentTimeStamp);
    fprintf(auxfd,"one requested %f  diff %f seconds\n",oneDelay,diff);
    diff = TimeStamp::diff(
        callbackTwo->getTimeStamp(),currentTimeStamp);
    fprintf(auxfd,"two requested %f  diff %f seconds\n",twoDelay,diff);
    delete timer;
    delete callbackTwo;
    delete callbackOne;
    delete eventTwo;
    delete eventOne;
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
    testBasic(fd,auxfd);
    oneDelay = .2;
    twoDelay = .4;
    testBasic(fd,auxfd);
    oneDelay = .0;
    twoDelay = .0;
    testBasic(fd,auxfd);
    getShowConstructDestruct()->constuctDestructTotals(fd);
    return (0);
}
