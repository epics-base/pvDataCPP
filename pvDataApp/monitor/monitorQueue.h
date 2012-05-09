/* monitorQueue.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/* Marty Kraimer 2011.03 */
#ifndef MONITORQUEUE_H
#define MONITORQUEUE_H
#include <string>
#include <stdexcept>
#include <memory>
#include <vector>

#include <pv/pvData.h>
#include <pv/monitor.h>
#include <pv/queue.h>
#include <pv/sharedPtr.h>

namespace epics { namespace pvData { 

class MonitorQueue;
typedef std::tr1::shared_ptr<MonitorQueue> MonitorQueuePtr;

class MonitorQueue {
public:
    static MonitorQueuePtr create(
        StructureConstPtr & elementStructure,int  number);
    MonitorQueue(MonitorElementArray &monitorElementArray);
    ~MonitorQueue();
    void clear();
    int getNumberFree();
    int capacity();
    MonitorElementPtr & getFree();
    void setUsed(MonitorElementPtr & element);
    MonitorElementPtr & getUsed();
    void releaseUsed(MonitorElementPtr & element);
private:
    MonitorElementPtr nullElement;
    MonitorElementArray elementArray;
    Queue<MonitorElementPtr> queue;
};

MonitorQueuePtr MonitorQueue::create(i
    StructureConstPtr & elementStructure,int  number)
{
    PVDataCreatePtr pvDataCreate = getPVDataCreate();
    MonitorElementArray elementArray;
    elementArray.reserve(number);
    for(int i=0; i<number; i++) {
         PVStructurePtr pvStructurePtr
             = pvDataCreate->createPVStructure(elementStructure);
         MonitorElementPtr  monitorElement(new MonitorElement(pvStructurePtr));
         elementArray.push_back(new MonitorElement(pvStructurePtr));
    }
    return MonitorQueuePtr(new MonitorQueue(elementArray));
}

MonitorQueue::MonitorQueue(MonitorElementArray &monitorElementArray)
: queue(new Queue<MonitorElementPtr>(monitorElementArray.swap()))
  {}

void MonitorQueue::clear()
{
    queue->clear();
}

int MonitorQueue::getNumberFree()
{
    return queue->getNumberFree();
}

int MonitorQueue::capacity()
{
    return queue.capacity();
}

MonitorElementPtr & MonitorQueue::getFree()
{
    MonitorElementPtr queueElement = queue->getFree();
    if(queueElement.get()==0) return nullElement;
    return queueElement;
}

void MonitorQueue::setUsed(MonitorElementPtr & element)
{
    queue->setUsed(element);
}

MonitorElementPtr & MonitorQueue::getUsed()
{
    MonitorElementPtr queueElement = queue->getUsed();
    if(queueElement.get()==0) return nullElement;
    return queueElement;
}

void MonitorQueue::releaseUsed(MonitorElementPtr & element)
{
    queue->releaseUsed(element);
}

}}

#endif  /* MONITORQUEUE_H */
