/* monitorQueue.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/* Marty Kraimer 2011.03 */
#include <pv/bitSet.h>

#include <pv/monitorQueue.h>


namespace epics { namespace pvData { 

MonitorElementArray MonitorQueue::createMonitorElements(
    StructureConstPtr & elementStructure,int number)
{
    PVDataCreatePtr pvDataCreate = getPVDataCreate();
    MonitorElementArray elementArray(number);
    for(int i=0; i<number; i++) {
         PVStructurePtr pvStructurePtr
             = pvDataCreate->createPVStructure(elementStructure);
         MonitorElementPtr  monitorElement(new MonitorElement(pvStructurePtr));
         elementArray[i] = monitorElement;
    }
    return elementArray;
}

MonitorQueue::MonitorQueue(MonitorElementArray monitorElementArray)
: elementArray(monitorElementArray),
  queue(elementArray)//,
  //number(monitorElementArray.size())
{
     number = monitorElementArray.size();
    if(number<2) {
         throw std::logic_error(String("queueSize must be >=2"));
    }
}

MonitorQueue::~MonitorQueue()
{
}

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
    return number;
}

MonitorElementPtr & MonitorQueue::getFree()
{
    MonitorElementPtr * queueElement = queue->getFree();
    if(queueElement==0) return nullElement;
    return *queueElement;
}

void MonitorQueue::setUsed(MonitorElement::shared_pointer const & element)
{
    MonitorElementImpl *impl = static_cast<MonitorElementImpl *>(element.get());
    queue->setUsed(impl->getQueueElement());
}

MonitorElement::shared_pointer const & MonitorQueue::getUsed()
{
    MonitorQueueElement *queueElement = queue->getUsed();
    if(queueElement==0) return nullElement;
    return *queueElement->getObject();
}

void MonitorQueue::releaseUsed(MonitorElement::shared_pointer const & element)
{
    MonitorElementImpl *impl = static_cast<MonitorElementImpl *>(element.get());
    queue->releaseUsed(impl->getQueueElement());
}

}}

