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
#include <pv/noDefaultMethods.h>

namespace epics { namespace pvData { 

class MessageNode;
class MessageQueue;
typedef std::tr1::shared_ptr<MessageNode> MessageNodePtr;
typedef std::tr1::shared_ptr<MessageQueue> MessageQueuePtr;

class MessageNode {
public:
    String getMessage() const { return message;}
    MessageType getMessageType() const {return messageType;}
    void setMessageNull() {message=String();}
private:
    MessageNode();
    ~MessageNode();
    friend class MessageQueue;
    String message;
    MessageType messageType;
};

class MessageQueue : private NoDefaultMethods {
public:
    static MessageQueuePtr create(int size);
    ~MessageQueue();
    MessageNode *get();
    // must call release before next get
    void release();
    // return (false,true) if message (was not, was) put into queue
    bool put(String message,MessageType messageType,bool replaceLast);
    bool isEmpty() const;
    bool isFull() const;
    int getClearOverrun();
private:
    MessageQueue(std::vector<std::tr1::shared_ptr<MessageNode> > data);

    Queue<MessageNode> queue;
    MessageNode *lastPut;
    MessagreNode *lastGet;
    int size;
    int overrun;
};

MessageQueuePtr MessageQueue::create(int size)
{
    std::vector<std::tr1::shared_ptr<MessageNode> > dataArray;
    dataArray.reserve(size);
    for(int i=0; i<size; i++) {
        dataArray.push_back(
            std::tr1::shared_ptr<MessageNode>(new MessageNode()));
    }
    return std::tr1::shared_ptr<MessageQueue>(new MessageQueue(dataArray));
}

MessageQueue::MessageQueue(std::vector<std::tr1::shared_ptr<MessageNode> > data)
: queue(data),
  lastPut(NULL),
  lastGet(NULL),
  size(data.size()),
  overrun(0)
{ }

MessageNode *MessageQueue::get() {
    if(lastGet!=NULL) {
        throw std::logic_error(
            String("MessageQueue::get() but did not release last"));
    }
    MessageNode node = queue.getUsed();
    if(node==NULL) return NULL;
    lastGet = node;
    return node;
}

void MessageQueue::release() {
    if(lastGet==NULL) return;
    queue.releaseUsed(lastGet);
    lastGet = NULL;
}

bool MessageQueue::put(String message,MessageType messageType,bool replaceLast)
{
    MessageNode node = queue.getFree();
    if(node!= NULL) {
        node->message = message;
        node->messageType = messageType;
        lastPut = node;
        queue.setUsed(node);
        return true;
    }
    overrun++;
    if(replaceLast) {
        node = lastPut;
        node->message = message;
        node->messageType = messageType;
    }
    return false;
}

bool MessageQueue::isEmpty() const
{
    int free = queue.getNumberFree();
    if(free==size) return true;
    return false;
}

bool MessageQueue::isFull() const
{
    if(queue.getNumberFree()==0) return true;
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



