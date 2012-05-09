/* messageQueue.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <string>
#include <pv/messageQueue.h>
namespace epics { namespace pvData { 

MessageNode::MessageNode()
: messageType(infoMessage)
{}

String MessageNode::getMessage() const
{
    return message;
}

MessageType MessageNode::getMessageType() const
{
    return messageType;
}

MessageQueuePtr MessageQueue::create(int size)
{
    MessageNodePtrArray nodeArray;
    nodeArray.reserve(size);
    for(int i=0; i<size; i++) {
        nodeArray.push_back(
            MessageNodePtr(new MessageNode()));
    }
    return std::tr1::shared_ptr<MessageQueue>(new MessageQueue(nodeArray));
}

MessageQueue::MessageQueue(MessageNodePtrArray &data)
: Queue<MessageNode>(data),
  overrun(0)
{ }

MessageQueue::~MessageQueue()
{
}

MessageNodePtr &MessageQueue::get() {
    if(getNumberUsed()==0) return nullNode;
    lastGet = getUsed();
    return lastGet;
}

void MessageQueue::release() {
    if(lastGet.get()==NULL) return;
    releaseUsed(lastGet);
    lastGet.reset();
}
bool MessageQueue::put(String message,MessageType messageType,bool replaceLast)
{
    MessageNodePtr node = getFree();
    if(node.get()!= NULL) {
        node->message = message;
        node->messageType = messageType;
        lastPut = node;
        setUsed(node);
        return true;
    }
    overrun++;
    if(replaceLast) {
        node = lastPut;
        node->message = message;
        node->messageType = messageType;
        return true;
    }
    return false;
}

bool MessageQueue::isEmpty()
{
    int free = getNumberFree();
    if(free==capacity()) return true;
    return false;
}

bool MessageQueue::isFull()
{
    if(getNumberFree()==0) return true;
    return false;
}

int MessageQueue::getClearOverrun()
{
    int num = overrun;
    overrun = 0;
    return num;
}

MessageQueuePtr createMessageQueue(int size)
{
    MessageNodePtrArray nodeArray;
    nodeArray.reserve(size);
    for(int i=0; i<size; i++) {
        nodeArray.push_back(
            MessageNodePtr(new MessageNode()));
    }
    return std::tr1::shared_ptr<MessageQueue>(new MessageQueue(nodeArray));
}


}}
