/* linkedList.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include "linkedListVoid.h"
namespace epics { namespace pvData { 

template <typename T>
class LinkedList;

template <typename T>
class LinkedListNode : private LinkedListVoidNode {
public:
    LinkedListNode(T *object) : LinkedListVoidNode(object){}
    ~LinkedListNode() {}
    T *getObject() { return static_cast<T *>(LinkedListVoidNode::getObject());}
    bool isOnList() {return LinkedListVoidNode::isOnList();}
    friend  class LinkedList<T>;
};

template <typename T>
class LinkedList : private LinkedListVoid {
public:
    LinkedList() : LinkedListVoid() {}
    ~LinkedList() {}
    int getLength() {return LinkedListVoid::getLength();}
    void addTail(LinkedListNode<T> *listNode)
    {
        LinkedListVoid::addTail(static_cast<LinkedListVoidNode *>(listNode));
    }
    void addHead(LinkedListNode<T> *listNode)
    {
        LinkedListVoid::addHead(static_cast<LinkedListVoidNode *>(listNode));
    }
    void insertAfter(LinkedListNode<T> *listNode,
        LinkedListNode<T> *addNode)
    {
        LinkedListVoid::insertAfter(
            static_cast<LinkedListVoidNode *>(listNode),
            static_cast<LinkedListVoidNode *>(addNode));
    }
    void insertBefore(LinkedListNode<T> *listNode,
        LinkedListNode<T> *addNode)
    {
        LinkedListVoid::insertBefore(
            static_cast<LinkedListVoidNode *>(listNode),
            static_cast<LinkedListVoidNode *>(addNode));
    }
    LinkedListNode<T> *removeTail(){
        return static_cast<LinkedListNode<T> *>(LinkedListVoid::removeTail());
    }
    LinkedListNode<T> *removeHead(){
        return static_cast<LinkedListNode<T> *>(LinkedListVoid::removeHead());
    }
    void remove(LinkedListNode<T> *listNode){
        LinkedListVoid::remove(static_cast<LinkedListVoidNode *>(listNode));
    }
    void remove(T *object){
          LinkedListVoid::remove(object);
    }
    LinkedListNode<T> *getHead(){
        return static_cast<LinkedListNode<T> *>(LinkedListVoid::getHead());
    }
    LinkedListNode<T> *getTail(){
        return static_cast<LinkedListNode<T> *>(LinkedListVoid::getTail());
    }
    LinkedListNode<T> *getNext(LinkedListNode<T> *listNode){
        return static_cast<LinkedListNode<T> *>(LinkedListVoid::getNext(
            static_cast<LinkedListVoidNode *>(listNode)));
    }
    LinkedListNode<T> *getPrev(LinkedListNode<T> *listNode){
        return static_cast<LinkedListNode<T> *>(LinkedListVoid::getPrev(
            static_cast<LinkedListVoidNode *>(listNode)));
    }
    bool isEmpty() { return LinkedListVoid::isEmpty();}
    bool contains(T *object) { return LinkedListVoid::contains(object);}
};


}}
#endif  /* LINKEDLIST_H */



