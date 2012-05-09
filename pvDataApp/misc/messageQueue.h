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
    MessageNode();
    String getMessage() const;
    MessageType getMessageType() const;
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

}}
#endif  /* MESSAGEQUEUE_H */



