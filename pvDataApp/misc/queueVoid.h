/* queueVoid.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include "showConstructDestruct.h"
#ifndef QUEUEVOID_H
#define QUEUEVOID_H
namespace epics { namespace pvData { 

class QueueVoid;
class QueueElementVoid;

typedef void * ObjectPtr;
typedef QueueElementVoid * QueueElementVoidPtr;
typedef QueueElementVoidPtr * QueueElementVoidPtrArray;

class QueueElementVoid {
public:
    static ConstructDestructCallback *getConstructDestructCallback();
protected:
    ObjectPtr getObject();
    QueueElementVoid(ObjectPtr object);
    ~QueueElementVoid();
    ObjectPtr object;
    friend  class QueueVoid;
};


class QueueVoid {
public:
    static ConstructDestructCallback *getConstructDestructCallback();
protected:
    QueueVoid(ObjectPtr array[],int number);
    ~QueueVoid();
    void clear();
    int getNumberFree();
    int capacity();
    QueueElementVoidPtr getFree();
    void setUsed(QueueElementVoid *queueElement);
    QueueElementVoid *getUsed();
    void releaseUsed(QueueElementVoid *queueElement);
private:
    friend class QueueElementVoid;
    QueueElementVoidPtrArray array;
    int number;
    int numberFree;
    int numberUsed;
    int nextGetFree;
    int nextSetUsed;
    int nextGetUsed;
    int nextReleaseUsed;
};


}}
#endif  /* QUEUEVOID_H */



