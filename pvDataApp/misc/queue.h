/* queue.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#include <vector>
#include <tr1/memory>
#include <cstddef>
#include <stdexcept>
#include <pv/sharedPtr.h>
#ifndef QUEUE_H
#define QUEUE_H
namespace epics { namespace pvData { 

template <typename T>
class Queue
{
public:
    POINTER_DEFINITIONS(Queue);
    typedef std::tr1::shared_ptr<T> queueElementPtr;
    typedef std::vector<queueElementPtr> queueElementPtrArray;
    Queue(queueElementPtrArray &);
    virtual ~Queue();
    void clear();
    int capacity();
    int getNumberFree();
    int getNumberUsed();
    queueElementPtr & getFree();
    void setUsed(queueElementPtr &element);
    queueElementPtr & getUsed();
    void releaseUsed(queueElementPtr &element);
private:
    queueElementPtr nullElement;
    queueElementPtrArray elements;
    int size;
    int numberFree;
    int numberUsed;
    int nextGetFree;
    int nextSetUsed;
    int nextGetUsed;
    int nextReleaseUsed;
};

template <typename T>
Queue<T>::Queue(std::vector<queueElementPtr> &xxx)
: size(xxx.size()),
  numberFree(size),
  numberUsed(0),
  nextGetFree(0),
  nextSetUsed(0),
  nextGetUsed(0),
  nextReleaseUsed(0)
{
     elements.swap(xxx);
}

template <typename T>
Queue<T>::~Queue(){}

template <typename T>
int Queue<T>::capacity(){return size;}

template <typename T>
int Queue<T>::getNumberFree(){return numberFree;}

template <typename T>
int Queue<T>::getNumberUsed(){return numberUsed;}

template <typename T>
void Queue<T>::clear()
{
    numberFree = size;
    numberUsed = 0;
    nextGetFree = 0;
    nextSetUsed = 0;
    nextGetUsed = 0;
    nextReleaseUsed = 0;
}

template <typename T>
std::tr1::shared_ptr<T> & Queue<T>::getFree()
{
    if(numberFree==0) return nullElement;
    numberFree--;
    int ind = nextGetFree;
    std::tr1::shared_ptr<T> queueElement = elements[nextGetFree++];
    if(nextGetFree>=size) nextGetFree = 0;
    return elements[ind];
}

template <typename T>
void Queue<T>::setUsed(std::tr1::shared_ptr<T> &element)
{
   if(element!=elements[nextSetUsed++]) {
        throw std::logic_error("not correct queueElement");
    }
    numberUsed++;
    if(nextSetUsed>=size) nextSetUsed = 0;
}

template <typename T>
std::tr1::shared_ptr<T> & Queue<T>::getUsed()
{
    if(numberUsed==0) return nullElement;
    int ind = nextGetUsed;
    std::tr1::shared_ptr<T> queueElement = elements[nextGetUsed++];
    if(nextGetUsed>=size) nextGetUsed = 0;
    return elements[ind];
}

template <typename T>
void Queue<T>::releaseUsed(std::tr1::shared_ptr<T> &element)
{
    if(element!=elements[nextReleaseUsed++]) {
        throw std::logic_error(
           "not queueElement returned by last call to getUsed");
    }
    if(nextReleaseUsed>=size) nextReleaseUsed = 0;
    numberUsed--;
    numberFree++;
}


}}
#endif  /* QUEUE_H */



