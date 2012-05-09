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

#include <pv/lock.h>
#include <pv/timeStamp.h>
#include <pv/queue.h>
#include <pv/event.h>
#include <pv/thread.h>
#include <pv/executor.h>


using namespace epics::pvData;

struct Data {
    int a;
    int b;
};

typedef std::tr1::shared_ptr<Data> DataPtr;
typedef std::vector<DataPtr> DataPtrArray;

static const int numElements = 5;
typedef Queue<Data> DataQueue;

class Sink;
typedef std::tr1::shared_ptr<Sink> SinkPtr;

class Sink : public Runnable {
public:
    static SinkPtr create(DataQueue &queue,FILE *auxfd);
    Sink(DataQueue &queue,FILE *auxfd);
    ~Sink();
    void stop();
    void look();
    virtual void run();
private:
    DataQueue &queue;
    FILE *auxfd;
    bool isStopped;
    Event *wait;
    Event *stopped;
    Event *waitReturn;
    Event *waitEmpty;
    Thread *thread;
};

SinkPtr Sink::create(DataQueue &queue,FILE *auxfd)
{
    return SinkPtr(new Sink(queue,auxfd));
}

Sink::Sink(DataQueue &queue,FILE *auxfd)
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
            DataPtr data = queue.getUsed();
            if(data.get()==NULL) {
                 waitEmpty->signal();
                 break;
            }
            fprintf(auxfd,"  sink a %d b %d\n",data->a,data->b);
            queue.releaseUsed(data);
        }
    }
    stopped->signal();
}

static void testBasic(FILE * fd,FILE *auxfd ) {
    DataPtrArray dataArray;
    dataArray.reserve(numElements);
    for(int i=0; i<numElements; i++)  {
        dataArray.push_back(DataPtr(new Data()));
    }
    DataQueue queue(dataArray);
    DataPtr data = queue.getFree();
    int value = 0;
    while(data.get()!=NULL) {
         data->a = value;
         data->b = value*10;
         value++;
         queue.setUsed(data);
         data = queue.getFree();
    }
    SinkPtr sink = SinkPtr(new Sink(queue,auxfd));
    queue.clear();
    while(true) {
        data = queue.getFree();
        if(data.get()==NULL) break;
        fprintf(auxfd,"source a %d b %d\n",data->a,data->b);
        queue.setUsed(data);
    }
    sink->look();
    // now alternate 
    for(int i=0; i<numElements; i++) {
        data = queue.getFree();
        assert(data.get()!=NULL);
        fprintf(auxfd,"source a %d b %d\n",data->a,data->b);
        queue.setUsed(data);
        sink->look();
    }
    sink->stop();
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
    return (0);
}
 
