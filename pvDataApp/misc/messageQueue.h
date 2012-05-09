/* messageQueue.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H
#include <memory>
#include <vector>
#include <cstddef>
#include <stdexcept>

#include <pv/pvType.h>
#include <pv/requester.h>
#include <pv/queue.h>

namespace epics { namespace pvData { 

class MessageNode;
class MessageQueue;
typedef std::tr1::shared_ptr<MessageNode> MessageNodePtr;
typedef std::vector<MessageNodePtr> MessageNodePtrArray;
typedef std::tr1::shared_ptr<MessageQueue> MessageQueuePtr;

class MessageNode {
public:
    MessageNode() : messageType(infoMessage) {}
    String getMessage() const { return message;}
    MessageType getMessageType() const {return messageType;}
private:
    String message;
    MessageType messageType;
    friend class MessageQueue;
};

class MessageQueue : public Queue<MessageNode> {
public:
    POINTER_DEFINITIONS(MessageQueue);
    static MessageQueuePtr create(int size);
    MessageQueue(MessageNodePtrArray &nodeArray);
    virtual ~MessageQueue();
    MessageNodePtr &get();
    // must call release before next get
    void release();
    // return (false,true) if message (was not, was) put into queue
    bool put(String message,MessageType messageType,bool replaceLast);
    bool isEmpty() ;
    bool isFull() ;
    int getClearOverrun();
private:
    MessageNodePtr nullNode;
    MessageNodePtr lastGet;
    MessageNodePtr lastPut;
    uint32 overrun;
};

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


}}
#endif  /* MESSAGEQUEUE_H */



