/* messageQueue.cpp */
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

#include "pvType.h"
#include "lock.h"
#include "requester.h"
#include "noDefaultMethods.h"
#include "CDRMonitor.h"
#include "queue.h"
#include "messageQueue.h"

namespace epics { namespace pvData { 

PVDATA_REFCOUNT_MONITOR_DEFINE(messageQueue);

typedef MessageNode * MessageNodePtr;
typedef QueueElement<MessageNode> MessageElement;
typedef MessageElement *MessageElementPtr;
typedef Queue<MessageNode> MessageNodeQueue;


MessageNode::MessageNode()
: message(String("")),messageType(infoMessage){}

MessageNode::~MessageNode() {
}

String MessageNode::getMessage() const { return message;};

MessageType MessageNode::getMessageType() const { return messageType;}


class MessageQueuePvt {
public:
    MessageNodePtr *messageNodeArray;
    MessageNodeQueue *queue;
    MessageNodePtr lastPut;
    MessageElementPtr lastGet;
    int size;
    int overrun;
};
    
MessageQueue::MessageQueue(int size)
: pImpl(new MessageQueuePvt)
{
    PVDATA_REFCOUNT_MONITOR_CONSTRUCT(messageQueue);
    pImpl->size = size;
    pImpl->overrun = 0;
    pImpl->lastPut = 0;
    pImpl->lastGet = 0;
    pImpl->messageNodeArray = new MessageNodePtr[size];
    for(int i=0; i<size; i++) {
        pImpl->messageNodeArray[i] = new MessageNode();
    }
    pImpl->queue = new MessageNodeQueue(pImpl->messageNodeArray,size);
}
    
MessageQueue::~MessageQueue()
{
    delete pImpl->queue;
    for(int i=0; i< pImpl->size; i++) {
        delete pImpl->messageNodeArray[i];
    }
    delete[] pImpl->messageNodeArray;
    PVDATA_REFCOUNT_MONITOR_DESTRUCT(messageQueue);
}

MessageNode *MessageQueue::get() {
    if(pImpl->lastGet!=0) {
        throw std::logic_error(
            String("MessageQueue::get() but did not release last"));
    }
    MessageElementPtr element = pImpl->queue->getUsed();
    if(element==0) return 0;
    pImpl->lastGet = element;
    return element->getObject();
}

void MessageQueue::release() {
    if(pImpl->lastGet==0) return;
    pImpl->queue->releaseUsed(pImpl->lastGet);
    pImpl->lastGet = 0;
}

bool MessageQueue::put(String message,MessageType messageType,bool replaceLast)
{
    MessageElementPtr element = pImpl->queue->getFree();
    if(element!=0) {
        MessageNodePtr node = element->getObject();
        node->message = message;
        node->messageType = messageType;
        pImpl->lastPut = node;
        pImpl->queue->setUsed(element);
        return true;
    }
    pImpl->overrun++;
    if(replaceLast) {
        MessageNodePtr node = pImpl->lastPut;
        node->message = message;
        node->messageType = messageType;
    }
    return false;
}

bool MessageQueue::isEmpty() const
{
    int free = pImpl->queue->getNumberFree();
    if(free==pImpl->size) return true;
    return false;
}

bool MessageQueue::isFull() const
{
    if(pImpl->queue->getNumberFree()==0) return true;
    return false;
}

int MessageQueue::getClearOverrun()
{
    int num = pImpl->overrun;
    pImpl->overrun = 0;
    return num;
}

}}
