/* messageQueue.cpp */
/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */
/**
 *  @author mrk
 */
#include <string>

#define epicsExportSharedSymbols
#include <pv/messageQueue.h>

using std::string;

namespace epics { namespace pvData { 

MessageNode::MessageNode()
: messageType(infoMessage)
{}

string MessageNode::getMessage() const
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
bool MessageQueue::put(string message,MessageType messageType,bool replaceLast)
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
