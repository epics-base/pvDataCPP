/* queueVoid.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>
#include <stdexcept>

#include "lock.h"
#include "pvType.h"
#include "queueVoid.h"

namespace epics { namespace pvData { 

static volatile int64 totalElementConstruct = 0;
static volatile int64 totalElementDestruct = 0;
static volatile int64 totalQueueConstruct = 0;
static volatile int64 totalQueueDestruct = 0;
static Mutex *globalMutex = 0;

static int64 getTotalNodeConstruct()
{
    Lock xx(globalMutex);
    return totalElementConstruct;
}

static int64 getTotalNodeDestruct()
{
    Lock xx(globalMutex);
    return totalElementDestruct;
}

static int64 getTotalListConstruct()
{
    Lock xx(globalMutex);
    return totalQueueConstruct;
}

static int64 getTotalListDestruct()
{
    Lock xx(globalMutex);
    return totalQueueDestruct;
}

static ConstructDestructCallback *pCDCallbackQueueNode;
static ConstructDestructCallback *pCDCallbackQueue;

static void initPvt()
{
     static Mutex mutex = Mutex();
     Lock xx(&mutex);
     if(globalMutex==0) {
        globalMutex = new Mutex();
        pCDCallbackQueueNode = new ConstructDestructCallback(
            "queueElement",
            getTotalNodeConstruct,getTotalNodeDestruct,0);
        
        pCDCallbackQueue = new ConstructDestructCallback(
            "queue",
            getTotalListConstruct,getTotalListDestruct,0);
     }
}


QueueElementVoid::QueueElementVoid(ObjectPtr object)
: object(object)
{
    initPvt();
    Lock xx(globalMutex);
    totalElementConstruct++;
}


QueueElementVoid::~QueueElementVoid()
{
    Lock xx(globalMutex);
    totalElementDestruct++;
}

ConstructDestructCallback *QueueElementVoid::getConstructDestructCallback()
{
    initPvt();
    return pCDCallbackQueueNode;
}

ObjectPtr QueueElementVoid::getObject() {
    return object;
}



QueueVoid::QueueVoid(ObjectPtr object[],int number)
: array(new QueueElementVoidPtr[number]),number(number),
  numberFree(number),numberUsed(0),
  nextGetFree(0),nextSetUsed(),
  nextGetUsed(0),nextReleaseUsed(0)
{
    for(int i=0; i<number; i++) {
        array[i] = new QueueElementVoid(object[i]);
    }
    initPvt();
    Lock xx(globalMutex);
    totalQueueConstruct++;
}

QueueVoid::~QueueVoid()
{
    for(int i=0; i<number; i++) {
        delete array[i];
    }
    delete[]array;
    Lock xx(globalMutex);
    totalQueueDestruct++;
}

ConstructDestructCallback *QueueVoid::getConstructDestructCallback()
{
    initPvt();
    return pCDCallbackQueue;
}

void QueueVoid::clear()
{
    numberFree = number;
    numberUsed = 0;
    nextGetFree = 0;
    nextSetUsed = 0;
    nextGetUsed = 0;
    nextReleaseUsed = 0;
}

int QueueVoid::getNumberFree()
{
    return numberFree;
}

int QueueVoid::capacity()
{
    return number;
}

QueueElementVoid * QueueVoid::getFree()
{
    if(numberFree==0) return 0;
    numberFree--;
    QueueElementVoid *queueElement = array[nextGetFree++];
    if(nextGetFree>=number) nextGetFree = 0;
    return queueElement;
}

void QueueVoid::setUsed(QueueElementVoid *queueElement)
{
    if(queueElement!=array[nextSetUsed++]) {
        throw std::logic_error(String("not correcect queueElement"));
    }
    numberUsed++;
    if(nextSetUsed>=number) nextSetUsed = 0;
}

QueueElementVoid * QueueVoid::getUsed()
{
    if(numberUsed==0) return 0;
    QueueElementVoid *queueElement = array[nextGetUsed++];
    if(nextGetUsed>=number) nextGetUsed = 0;
    return queueElement;
}

void QueueVoid::releaseUsed(QueueElementVoid *queueElement)
{
    if(queueElement!=array[nextReleaseUsed++]) {
        throw std::logic_error(String(
           "not queueElement returned by last call to getUsed"));
    }
    if(nextReleaseUsed>=number) nextReleaseUsed = 0;
    numberUsed--;
    numberFree++;
}

}}
