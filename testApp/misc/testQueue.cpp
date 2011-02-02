/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/*
 * testQueue.cpp
 *
 *  Created on: 2010.12
 *      Author: Marty Kraimer
 */

#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>

#include <epicsAssert.h>
#include <epicsExit.h>

#include "lock.h"
#include "timeStamp.h"
#include "queue.h"
#include "CDRMonitor.h"
#include "event.h"
#include "thread.h"
#include "executor.h"


using namespace epics::pvData;

struct Data {
    int a;
    int b;
};


static int numElements = 5;
typedef QueueElement<Data> DataElement;
typedef Queue<Data> DataQueue;

class Sink : public Runnable {
public:
    Sink(DataQueue *queue,FILE *auxfd);
    ~Sink();
    void stop();
    void look();
    virtual void run();
private:
    DataQueue *queue;
    FILE *auxfd;
    bool isStopped;
    Event *wait;
    Event *stopped;
    Event *waitReturn;
    Event *waitEmpty;
    Thread *thread;
};

Sink::Sink(DataQueue *queue,FILE *auxfd)
: queue(queue),
  auxfd(auxfd),
  isStopped(false),
  wait(new Event()),
  stopped(new Event()),
  waitReturn(new Event()),
  waitEmpty(new Event()),
  thread(new Thread(String("sink"),middlePriority,this))
{
}

Sink::~Sink() {
    delete thread;
    delete waitEmpty;
    delete waitReturn;
    delete stopped;
    delete wait;
}

void  Sink::stop()
{
    isStopped = true;
    wait->signal();
    stopped->wait();
}

void  Sink::look() 
{
    wait->signal();
    waitEmpty->wait();
}

void Sink::run()
{
    while(!isStopped) {
        wait->wait();
        if(isStopped) break;
        while(true) {
            DataElement *element = queue->getUsed();
            if(element==0) {
                 waitEmpty->signal();
                 break;
            }
            Data *data = element->getObject();
            fprintf(auxfd,"  sink a %d b %d\n",data->a,data->b);
            queue->releaseUsed(element);
        }
    }
    stopped->signal();
}

static void testBasic(FILE * fd,FILE *auxfd ) {
    Data *dataArray[numElements];
    for(int i=0; i<numElements; i++) {
        dataArray[i] = new Data();
        dataArray[i]->a = i;
        dataArray[i]->b = i*10;
    }
    DataQueue *queue = new DataQueue(dataArray,numElements);
    Sink *sink = new Sink(queue,auxfd);
    while(true) {
        DataElement *element = queue->getFree();
        if(element==0) break;
        Data *data = element->getObject();
        fprintf(auxfd,"source a %d b %d\n",data->a,data->b);
        queue->setUsed(element);
    }
    sink->look();
    // now alternate 
    for(int i=0; i<numElements; i++) {
        DataElement *element = queue->getFree();
        assert(element!=0);
        Data *data = element->getObject();
        fprintf(auxfd,"source a %d b %d\n",data->a,data->b);
        queue->setUsed(element);
        sink->look();
    }
    sink->stop();
    delete sink;
    delete queue;
    for(int i=0; i<numElements; i++) delete dataArray[i];
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
    testBasic(fd,auxfd);
    epicsExitCallAtExits();
    CDRMonitor::get().show(fd);
    return (0);
}
 
