/* queue.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef QUEUE_H
#define QUEUE_H
#include "queueVoid.h"
namespace epics { namespace pvData { 

template <typename T>
class Queue;

template <typename T>
class QueueElement;

template <typename T>
class QueueElement : private QueueElementVoid {
public:
    T *getObject() { return static_cast<T *>(QueueElementVoid::getObject());}
protected:
    QueueElement(T *object) : QueueElementVoid(static_cast<void *>(object)){}
    ~QueueElement() {}
    friend  class Queue<T>;
};

template <typename T>
class Queue : private QueueVoid {
public:
    Queue(T *array[],int number)
    : QueueVoid((ObjectPtr*)array,number)
    //: QueueVoid(static_cast<ObjectPtr*>(array),number)
    {}
    ~Queue() {}
    void clear() {QueueVoid::clear();}
    int getNumberFree() {return QueueVoid::getNumberFree();}
    int capacity() {return QueueVoid::capacity();}
    QueueElement<T> *getFree() {
        return static_cast<QueueElement<T> *>(QueueVoid::getFree());}
    void setUsed(QueueElement<T> *queueElement) {
        QueueVoid::setUsed(static_cast<QueueElementVoid *>(queueElement));}
    QueueElement<T> *getUsed() {
        return static_cast<QueueElement<T> *>(QueueVoid::getUsed());}
    void releaseUsed(QueueElement<T> *queueElement) {
        QueueVoid::releaseUsed(static_cast<QueueElementVoid *>(queueElement));}
};


}}
#endif  /* QUEUE_H */



