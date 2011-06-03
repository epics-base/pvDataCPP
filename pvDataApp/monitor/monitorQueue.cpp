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

typedef QueueElement<MonitorElement::shared_pointer> MonitorQueueElement;

class MonitorElementImpl : public MonitorElement {
public:
    MonitorElementImpl(PVStructure::shared_pointer pvStructure);
    ~MonitorElementImpl(){}
    virtual PVStructure::shared_pointer const &  getPVStructure();
    virtual BitSet::shared_pointer const & getChangedBitSet();
    virtual BitSet::shared_pointer const & getOverrunBitSet();
    void setQueueElement(MonitorQueueElement *queueElement);
    MonitorQueueElement *getQueueElement();
private:
    PVStructure::shared_pointer pvStructure;
    BitSet::shared_pointer changedBitSet;
    BitSet::shared_pointer overrunBitSet;
    MonitorQueueElement *queueElement;
};

MonitorElementImpl::MonitorElementImpl(PVStructure::shared_pointer pvStructure)
: pvStructure(pvStructure),
  changedBitSet(BitSet::shared_pointer(
      new BitSet(pvStructure->getNumberFields()))),
  overrunBitSet(BitSet::shared_pointer(
      new BitSet(pvStructure->getNumberFields()))),
  queueElement(0)
{}

PVStructure::shared_pointer const & MonitorElementImpl::getPVStructure()
{
    return pvStructure;
}

BitSet::shared_pointer const & MonitorElementImpl::getChangedBitSet()
{
    return changedBitSet;
}

BitSet::shared_pointer const & MonitorElementImpl::getOverrunBitSet()
{
    return overrunBitSet;
}

void MonitorElementImpl::setQueueElement(MonitorQueueElement *queueElement)
{
    this->queueElement = queueElement;
}

MonitorQueueElement *MonitorElementImpl::getQueueElement()
{
    return queueElement;
}

MonitorQueue::MonitorQueue(PVStructureSharedPointerPtrArray structures,int number)
: number(number),
  structures(structures),
  queue(0),
  queueElements(new MonitorElement::shared_pointer*[number]),
  nullElement(MonitorElement::shared_pointer())
{
    if(number<2) {
         throw std::logic_error(String("queueSize must be >=2"));
    }
    
    for(int i=0; i<number; i++) {
        queueElements[i] = new MonitorElement::shared_pointer(
               new MonitorElementImpl(*structures[i]));
    }
    queue = new Queue<MonitorElement::shared_pointer>(queueElements,number);
    MonitorQueueElement *queueElement;
    for(int i=0; i<number;i++) {
        queueElement = queue->getFree();
        MonitorElementImpl * element = static_cast<MonitorElementImpl *>(
            queueElement->getObject()->get());
        element->setQueueElement(queueElement);
        queue->setUsed(queueElement);
        queue->releaseUsed(queueElement);
    }
}

MonitorQueue::~MonitorQueue()
{
    delete queue;
    for(int i=0; i<number; i++) {
          delete queueElements[i];
    }
    delete[] queueElements;
    for(int i=0; i<number; i++) delete structures[i];
    delete[] structures;
}

PVStructureSharedPointerPtrArray MonitorQueue::createStructures(
    PVStructurePtrArray array,int number)
{
    PVStructureSharedPointerPtrArray elements =
        new PVStructureSharedPointerPtr[number];
    for(int i=0; i<number; i++){
        elements[i] =  new PVStructure::shared_pointer(array[i]);
    }
    delete[] array;
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

MonitorElement::shared_pointer const & MonitorQueue::getFree()
{
    MonitorQueueElement *queueElement = queue->getFree();
    if(queueElement==0) return nullElement;
    return *queueElement->getObject();
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

