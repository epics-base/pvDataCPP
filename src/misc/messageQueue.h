/* messageQueue.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
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

#include <shareLib.h>

namespace epics { namespace pvData { 

class MessageNode;
class MessageQueue;
typedef std::tr1::shared_ptr<MessageNode> MessageNodePtr;
typedef std::vector<MessageNodePtr> MessageNodePtrArray;
typedef std::tr1::shared_ptr<MessageQueue> MessageQueuePtr;

/**
 * @brief A node that can be put on a MessageQueue.
 *
 */
class epicsShareClass MessageNode {
public:
    /**
     * Constructor
     */
    MessageNode();
    /**
     * 
     * Get the message value.
     * @return The message value.
     */
    std::string getMessage() const;
    /** 
     * Get the message type.
     * @return The message type which is defined in Requester.
     */
    MessageType getMessageType() const;
private:
    std::string message;
    MessageType messageType;
    friend class MessageQueue;
};

/**
 * @brief A bounded queue for messages.
 *
 *
 */
class epicsShareClass MessageQueue : public Queue<MessageNode> {
public:
    POINTER_DEFINITIONS(MessageQueue);
    /**
     * Factory method to create a MessageQueue.
     * @param size The number of MessageNodes in the queue.
     * @return shared_ptr to MessageQueue.
     */
    static MessageQueuePtr create(int size);
    /**
     * Constructor
     * @param nodeArray an array of shared_ptr to MessageNodes,
     */
    MessageQueue(MessageNodePtrArray &nodeArray);
    /**
     * Destructor
     */
    virtual ~MessageQueue();
    /**
     * get the next MessageNode of the queue.
     * @return A shared_ptr to the MessageNode.
     * This will be a null pointer if queue is empty.
     * If get is successful then release for this MessageNode
     * must be called before another get can be issued.
     */
    MessageNodePtr &get();
    /**
     * Release the MessageNode that was returned by the previous call to get.
     */
    void release();
    /**
     * 
     * put a message into the message queue
     * @param message The message string.
     * @param messageType The message type as defined in Requester,
     * @param replaceLast If true and queue is full then replace.
     * @return (false,true) if a message (was not, was) put in queiue.
     */
    bool put(std::string message,MessageType messageType,bool replaceLast);
    /**
     * Is queue empty?
     * @return (false,true) if (is not, is) empty.
     */
    bool isEmpty() ;
    /**
     * Is queue full?
     * @return (false,true) if (is not, is) full.
     */
    bool isFull() ;
    /**
     * 
     * Clear number of times queue was overrun and return the number
     * of times the queue was overrun.
     */
    int getClearOverrun();
private:
    MessageNodePtr nullNode;
    MessageNodePtr lastGet;
    MessageNodePtr lastPut;
    uint32 overrun;
};

}}
#endif  /* MESSAGEQUEUE_H */



