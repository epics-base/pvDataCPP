/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
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

#include <epicsUnitTest.h>
#include <testMain.h>

#include <epicsThread.h>

#include <pv/lock.h>
#include <pv/timeStamp.h>
#include <pv/queue.h>
#include <pv/event.h>


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

class Sink : public epicsThreadRunable {
public:
    static SinkPtr create(DataQueue &queue);
    Sink(DataQueue &queue);
    ~Sink();
    void stop();
    void look();
    virtual void run();
private:
    DataQueue &queue;
    bool isStopped;
    Event *wait;
    Event *stopped;
    Event *waitReturn;
    Event *waitEmpty;
    epicsThread *thread;
};

SinkPtr Sink::create(DataQueue &queue)
{
    return SinkPtr(new Sink(queue));
}

Sink::Sink(DataQueue &queue)
: queue(queue),
  isStopped(false),
  wait(new Event()),
  stopped(new Event()),
  waitReturn(new Event()),
  waitEmpty(new Event()),
  thread(new epicsThread(*this,"sink",epicsThreadGetStackSize(epicsThreadStackSmall)))
{
   thread->start();
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
            printf("  sink a %d b %d\n",data->a,data->b);
            queue.releaseUsed(data);
        }
    }
    stopped->signal();
}

static void testBasic() {
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
    SinkPtr sink = SinkPtr(new Sink(queue));
    queue.clear();
    while(true) {
        data = queue.getFree();
        if(data.get()==NULL) break;
        printf("source a %d b %d\n",data->a,data->b);
        queue.setUsed(data);
    }
    sink->look();
    // now alternate 
    for(int i=0; i<numElements; i++) {
        data = queue.getFree();
        testOk1(data.get()!=NULL);
        printf("source a %d b %d\n",data->a,data->b);
        queue.setUsed(data);
        sink->look();
    }
    sink->stop();
    printf("PASSED\n");
}


MAIN(testQueue)
{
    testPlan(5);
    testDiag("Tests queue");
    testBasic();
    return testDone();
}
 
