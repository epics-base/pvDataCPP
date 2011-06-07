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

#include <pv/lock.h>
#include <pv/pvType.h>
#include <pv/queueVoid.h>
#include <pv/CDRMonitor.h>

namespace epics { namespace pvData { 

PVDATA_REFCOUNT_MONITOR_DEFINE(queueElement);
PVDATA_REFCOUNT_MONITOR_DEFINE(queue);

QueueElementVoid::QueueElementVoid(ObjectPtr object)
: object(object)
{
    PVDATA_REFCOUNT_MONITOR_CONSTRUCT(queueElement);
}


QueueElementVoid::~QueueElementVoid()
{
    PVDATA_REFCOUNT_MONITOR_DESTRUCT(queueElement);
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
    PVDATA_REFCOUNT_MONITOR_CONSTRUCT(queue);
}

QueueVoid::~QueueVoid()
{
    for(int i=0; i<number; i++) {
        delete array[i];
    }
    delete[]array;
    PVDATA_REFCOUNT_MONITOR_DESTRUCT(queue);
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
        throw std::logic_error(String("not correct queueElement"));
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
