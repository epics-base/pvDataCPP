/* queue.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <vector>
#include <tr1/memory>
#include <cstddef>
#include <stdexcept>
#ifndef QUEUE_H
#define QUEUE_H
namespace epics { namespace pvData { 

template <typename T>
class Queue
{
public:
    typedef std::tr1::shared_ptr<T> queueElementPtr;
    Queue(int size);
    Queue(std::vector<queueElementPtr> elements);
    ~Queue(){}
    void clear();
    int capacity(){return size;}
    int getNumberFree(){return numberFree;}
    int getNumberUsed(){return numberUsed;}
    T * getFree();
    void setUsed(T *element);
    T * getUsed();
    void releaseUsed(T *element);
private:
    std::vector<queueElementPtr> elements;
    int size;
    int numberFree;
    int numberUsed;
    int nextGetFree;
    int nextSetUsed;
    int nextGetUsed;
    int nextReleaseUsed;
};

template <typename T>
Queue<T>::Queue(int size)
: elements(size),
  size(size),
  numberFree(size),
  numberUsed(0),
  nextGetFree(0),
  nextSetUsed(0),
  nextGetUsed(0),
  nextReleaseUsed(0)
{
    for(int i=0; i<size; i++) {
        elements[i] = std::tr1::shared_ptr<T>(new T());
    }
}

template <typename T>
Queue<T>::Queue(std::vector<queueElementPtr> elements)
: elements(elements),
  size(elements.size()),
  numberFree(size),
  numberUsed(0),
  nextGetFree(0),
  nextSetUsed(0),
  nextGetUsed(0),
  nextReleaseUsed(0)
{ }

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
T * Queue<T>::getFree()
{
    if(numberFree==0) return NULL;
    numberFree--;
    std::tr1::shared_ptr<T> queueElement = elements[nextGetFree++];
    if(nextGetFree>=size) nextGetFree = 0;
    return queueElement.get();
}

template <typename T>
void Queue<T>::setUsed(T *element)
{
   if(element!=elements[nextSetUsed++].get()) {
        throw std::logic_error("not correct queueElement");
    }
    numberUsed++;
    if(nextSetUsed>=size) nextSetUsed = 0;
}

template <typename T>
T * Queue<T>::getUsed()
{
    if(numberUsed==0) return 0;
    std::tr1::shared_ptr<T> queueElement = elements[nextGetUsed++];
    if(nextGetUsed>=size) nextGetUsed = 0;
    return queueElement.get();
}

template <typename T>
void Queue<T>::releaseUsed( T *element)
{
    if(element!=elements[nextReleaseUsed++].get()) {
        throw std::logic_error(
           "not queueElement returned by last call to getUsed");
    }
    if(nextReleaseUsed>=size) nextReleaseUsed = 0;
    numberUsed--;
    numberFree++;

}


}}
#endif  /* QUEUE_H */



