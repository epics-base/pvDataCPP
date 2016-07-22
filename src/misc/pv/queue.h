/* queue.h */
/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */
/**
 *  @author mrk
 */
#ifndef QUEUE_H
#define QUEUE_H

#include <vector>
#include <cstddef>
#include <stdexcept>

#include <pv/sharedPtr.h>

namespace epics { namespace pvData { 

/**
 * @brief Template class for a bounded queue.
 *
 * An instance can make a queueElement be any class desired
 * but must create a std::vector of shared_ptr to queueElements.
 */
template <typename T>
class Queue
{
public:
    POINTER_DEFINITIONS(Queue);
    typedef std::tr1::shared_ptr<T> queueElementPtr;
    typedef std::vector<queueElementPtr> queueElementPtrArray;
    /**
     * Constructor
     * @param elementArray The vector of shared_ptr to queue elements.
     */
    Queue(queueElementPtrArray & elementArray);
    /**
     * Destructor
     */
    virtual ~Queue();
    /**
     * Clear the queue.
     */
    void clear();
    /** 
     * get the capacity of the queue, i. e. number of queue elements,
     * @return The capacity.
     */
    int capacity();
    /**
     * Get the number of free elements in the queue.
     * @return The number.
     */
    int getNumberFree();
    /**
     * Get the number of used elements in the queue.
     * This is the number that have been setUsed but not released.
     * @return The number.
     */
    int getNumberUsed();
    /**
     * Get the next free element.
     * @return a shared_ptr to the queue element.
     * This is null if queue was full.
     */
    queueElementPtr & getFree();
    /**
     * Set the element returned by getFree as used.
     * Until this is called getUsed will not return it.
     * @param element The element. It must be the element returned
     * by the most recent call to getUsed.
     */
    void setUsed(queueElementPtr const &element);
    /**
     * Get the oldest used element;
     * @return a shared_ptr to the queue element.
     * This is null if no used element is available.`
     */
    queueElementPtr & getUsed();
    /**
     * Release the element obtained by the most recent call to getUsed.
     * @param element The element.
     */
    void releaseUsed(queueElementPtr const &element);
private:
    queueElementPtr nullElement;
    queueElementPtrArray elements;
    // TODO use size_t instead
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
: size(static_cast<int>(xxx.size())),
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
void Queue<T>::setUsed(std::tr1::shared_ptr<T> const &element)
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
void Queue<T>::releaseUsed(std::tr1::shared_ptr<T> const &element)
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



