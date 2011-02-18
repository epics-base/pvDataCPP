/* linkedListVoid.cpp */
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

#include "lock.h"
#include "pvType.h"
#include "linkedListVoid.h"
#include "CDRMonitor.h"

namespace epics { namespace pvData { 

static String alreadyOnList("already on list");

PVDATA_REFCOUNT_MONITOR_DEFINE(LinkedListNode);
PVDATA_REFCOUNT_MONITOR_DEFINE(LinkedList);

LinkedListVoidNode::LinkedListVoidNode(void *object)
: object(object),before(0),after(0),linkedListVoid(0)
{
    PVDATA_REFCOUNT_MONITOR_CONSTRUCT(LinkedListNode);
}

LinkedListVoidNode::LinkedListVoidNode(bool isHead)
: object(this),before(this),after(this)
{
    PVDATA_REFCOUNT_MONITOR_CONSTRUCT(LinkedListNode);
}


LinkedListVoidNode::~LinkedListVoidNode()
{
    PVDATA_REFCOUNT_MONITOR_DESTRUCT(LinkedListNode);
}

void *LinkedListVoidNode::getObject() {
    return object;
}

bool LinkedListVoidNode::isOnList()
{
    if(before==0 && after==0) return false;
    return true;
}

LinkedListVoid::LinkedListVoid()
: head(new LinkedListVoidNode(true)),length(0)
{
    PVDATA_REFCOUNT_MONITOR_CONSTRUCT(LinkedList);
}

LinkedListVoid::~LinkedListVoid()
{
    delete head;
    PVDATA_REFCOUNT_MONITOR_DESTRUCT(LinkedList);
}

int LinkedListVoid::getLength()
{
    return length;
}

void LinkedListVoid::addTail(LinkedListVoidNode &node)
{
    if(node.before!=0 || node.after!=0) {
        throw std::logic_error(alreadyOnList);
    }
    node.linkedListVoid = this;
    node.before = head->before;
    node.after = head;
    head->before->after = &node;
    head->before = &node;
    ++length;
}

void LinkedListVoid::addHead(LinkedListVoidNode &node)
{
    if(node.before!=0 || node.after!=0) {
        throw std::logic_error(alreadyOnList);
    }
    node.linkedListVoid = this;
    node.after = head->after;
    node.before = head;
    head->after->before = &node;
    head->after = &node;
    ++length;
}

void LinkedListVoid::insertAfter(LinkedListVoidNode &node,
    LinkedListVoidNode &addNode)
{
    LinkedListVoidNode *existingNode = &node;
    LinkedListVoidNode *newNode = &addNode;
    if(existingNode->after==0 || existingNode->before==0) {
        throw std::logic_error(String("listNode not on list"));
    }
    if(newNode->before!=0 || newNode->after!=0) {
        throw std::logic_error(alreadyOnList);
    }
    if(node.linkedListVoid!=this) {
        throw std::logic_error(String("node not on this list"));
    }
    newNode->linkedListVoid = this;
    newNode->after = existingNode->after;
    newNode->before = existingNode;
    existingNode->after->before = newNode;
    existingNode->after = newNode;
    ++length;
}

void LinkedListVoid::insertBefore(LinkedListVoidNode &node,
    LinkedListVoidNode &addNode)
{
    LinkedListVoidNode *existingNode = &node;
    LinkedListVoidNode *newNode = &addNode;
    if(existingNode->after==0 || existingNode->before==0) {
        throw std::logic_error(String("listNode not on list"));
    }
    if(newNode->before!=0 || newNode->after!=0) {
        throw std::logic_error(alreadyOnList);
    }
    if(node.linkedListVoid!=this) {
        throw std::logic_error(String("node not on this list"));
    }
    newNode->linkedListVoid = this;
    newNode->after = existingNode;
    newNode->before = existingNode->before;
    existingNode->before->after = newNode;
    existingNode->before = newNode;
    ++length;
}

LinkedListVoidNode *LinkedListVoid::removeTail()
{
    if(head->after==head) return 0;
    LinkedListVoidNode *node = head->before;
    remove(*head->before);
    return node;
}

LinkedListVoidNode *LinkedListVoid::removeHead()
{
    if(head->after==head) return 0;
    LinkedListVoidNode *node = head->after;
    remove(*head->after);
    return node;
}

void LinkedListVoid::remove(LinkedListVoidNode &node)
{
    if(node.before==0 || node.after==0) {
        throw std::logic_error(String("node not on list"));
    }
    if(node.linkedListVoid!=this) {
        throw std::logic_error(String("node not on this list"));
    }
    node.linkedListVoid = 0;
    LinkedListVoidNode *prev = node.before;
    LinkedListVoidNode *next = node.after;
    node.after = node.before = 0;
    prev->after = next;
    next->before = prev;
    length--;
}

LinkedListVoidNode *LinkedListVoid::getHead()
{
    if(head->after==head) return 0;
    return head->after;
}

LinkedListVoidNode *LinkedListVoid::getTail()
{
    if(head->after==head) return 0;
    return head->before;
}

LinkedListVoidNode *LinkedListVoid::getNext(LinkedListVoidNode &listNode)
{
    if(listNode.linkedListVoid!=this) {
        throw std::logic_error(String("node not on this list"));
    }
    if(listNode.after==head) return 0;
    return listNode.after;
}

LinkedListVoidNode *LinkedListVoid::getPrev(LinkedListVoidNode &listNode)
{
    if(listNode.linkedListVoid!=this) {
        throw std::logic_error(String("node not on this list"));
    }
    if(listNode.before==head) return 0;
    return listNode.before;
}

bool LinkedListVoid::isEmpty()
{
    if(head->after==head) return true;
    return false;
}

}}
