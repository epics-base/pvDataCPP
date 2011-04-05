/* monitorQueue.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/* Marty Kraimer 2011.03 */
#include <bitSet.h>

#include "monitorQueue.h"


namespace epics { namespace pvData { 

typedef QueueElement<MonitorElement> MonitorQueueElement;

class MonitorElementImpl : public MonitorElement {
public:
    MonitorElementImpl(PVStructure *pvStructure);
    ~MonitorElementImpl(){}
    virtual PVStructure* getPVStructure();
    virtual BitSet* getChangedBitSet();
    virtual BitSet* getOverrunBitSet();
    void setQueueElement(MonitorQueueElement *queueElement);
    MonitorQueueElement *getQueueElement();
private:
    PVStructure *pvStructure;
    std::auto_ptr<BitSet> changedBitSet;
    std::auto_ptr<BitSet> overrunBitSet;
    MonitorQueueElement *queueElement;
};

MonitorElementImpl::MonitorElementImpl(PVStructure *pvStructure)
: pvStructure(pvStructure),
  changedBitSet(std::auto_ptr<BitSet>(new BitSet(pvStructure->getNumberFields()))),
  overrunBitSet(std::auto_ptr<BitSet>(new BitSet(pvStructure->getNumberFields()))),
  queueElement(0)
{}

PVStructure* MonitorElementImpl::getPVStructure()
{
    return pvStructure;
}

BitSet* MonitorElementImpl::getChangedBitSet()
{
    return changedBitSet.get();
}

BitSet* MonitorElementImpl::getOverrunBitSet()
{
    return overrunBitSet.get();
}

void MonitorElementImpl::setQueueElement(MonitorQueueElement *queueElement)
{
    this->queueElement = queueElement;
}

MonitorQueueElement *MonitorElementImpl::getQueueElement()
{
    return queueElement;
}

MonitorQueue::MonitorQueue(MonitorElement** elements,int number)
: number(number),
  elements(elements),
  queue(new Queue<MonitorElement>(elements,number))
{
    if(number<2) {
         throw std::logic_error(String("queueSize must be >=2"));
    }
    MonitorQueueElement  *queueElement;
    for(int i=0; i<number;i++) {
        queueElement = queue->getFree();
        MonitorElementImpl *temp = 
            static_cast<MonitorElementImpl*>(queueElement->getObject());
        temp->setQueueElement(queueElement);
        queue->setUsed(queueElement);
        queue->releaseUsed(queueElement);
    }
}

MonitorQueue::~MonitorQueue()
{
    delete queue;
    for(int i=0; i<number; i++) {
        delete elements[i];
    }
    delete[] elements;
}

MonitorElement** MonitorQueue::createElements(PVStructurePtrArray array,int number)
{
    MonitorElement** elements = new MonitorElement*[number];
    for(int i=0; i<number; i++) {
        elements[i] = new MonitorElementImpl(array[i]);
    }
    return elements;
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

MonitorElement * MonitorQueue::getFree()
{
    MonitorQueueElement *queueElement = queue->getFree();
    if(queueElement==0) return 0;
    return queueElement->getObject();
}

void MonitorQueue::setUsed(MonitorElement * element)
{
    MonitorElementImpl *temp = static_cast<MonitorElementImpl*>(element);
    MonitorQueueElement *queueElement = temp->getQueueElement();
    queue->setUsed(temp->getQueueElement());
}

MonitorElement * MonitorQueue::getUsed()
{
    MonitorQueueElement *queueElement = queue->getUsed();
    if(queueElement==0) return 0;
    return queueElement->getObject();
}

void MonitorQueue::releaseUsed(MonitorElement * element)
{
    MonitorElementImpl *temp = static_cast<MonitorElementImpl*>(element);
    queue->releaseUsed(temp->getQueueElement());
}


}}

