/* messageQueue.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H
#include <pv/pvType.h>
#include <pv/requester.h>
#include <pv/noDefaultMethods.h>

namespace epics { namespace pvData { 

class MessageNode {
public:
    String getMessage() const;
    MessageType getMessageType() const;
    void setMessageNull();
private:
    MessageNode();
    ~MessageNode();
    friend class MessageQueue;
    String message;
    MessageType messageType;
};

class MessageQueue : private NoDefaultMethods {
public:
    MessageQueue(int size);
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
    class MessageQueuePvt *pImpl;
};

}}
#endif  /* MESSAGEQUEUE_H */



