/* linkedList.h */
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
    T *getObject() { return (T *)LinkedListVoidNode::getObject();}
    bool isOnList() {return LinkedListVoidNode::isOnList();}
};

template <typename T>
class LinkedList : private LinkedListVoid {
public:
    LinkedList() : LinkedListVoid() {}
    ~LinkedList() {}
    int getLength() {return LinkedListVoid::getLength();}
    void addTail(LinkedListNode<T> *listNode)
    {
        LinkedListVoid::addTail((LinkedListVoidNode *)listNode);
    }
    void addHead(LinkedListNode<T> *listNode)
    {
        LinkedListVoid::addHead((LinkedListVoidNode *)listNode);
    }
    void insertAfter(LinkedListNode<T> *listNode,
        LinkedListNode<T> *addNode)
    {
        LinkedListVoid::insertAfter(
            (LinkedListVoidNode *)listNode,(LinkedListVoidNode *)addNode);
    }
    void insertBefore(LinkedListNode<T> *listNode,
        LinkedListNode<T> *addNode)
    {
        LinkedListVoid::insertBefore((LinkedListVoidNode *)listNode,
            (LinkedListVoidNode *)addNode);
    }
    LinkedListNode<T> *removeTail(){
        return (LinkedListNode<T>*)LinkedListVoid::removeTail();
    }
    LinkedListNode<T> *removeHead(){
        return (LinkedListNode<T>*)LinkedListVoid::removeHead();
    }
    void remove(LinkedListNode<T> *listNode){
        LinkedListVoid::remove((LinkedListVoidNode *)listNode);
    }
    void remove(T *object){
          LinkedListVoid::remove(object);
    }
    LinkedListNode<T> *getHead(){
        return (LinkedListNode<T>*)LinkedListVoid::getHead();
    }
    LinkedListNode<T> *getTail(){
        return (LinkedListNode<T>*)LinkedListVoid::getTail();
    }
    LinkedListNode<T> *getNext(LinkedListNode<T> *listNode){
        return (LinkedListNode<T>*)LinkedListVoid::getNext(
            (LinkedListVoidNode *)listNode);
    }
    LinkedListNode<T> *getPrev(LinkedListNode<T> *listNode){
        return (LinkedListNode<T>*)LinkedListVoid::getPrev(
            (LinkedListVoidNode *)listNode);
    }
    bool isEmpty() { return LinkedListVoid::isEmpty();}
    bool contains(T *object) { return LinkedListVoid::contains(object);}
};


}}
#endif  /* LINKEDLIST_H */



