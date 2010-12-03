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

typedef QueueElementVoid * QueueElementVoidPtr;
typedef QueueElementVoidPtr * QueueElementVoidPtrArray;

class QueueElementVoid {
public:
    ~QueueElementVoid();
    static ConstructDestructCallback *getConstructDestructCallback();
    void *getObject();
protected:
    QueueElementVoid(void *object);
private:
    void *object;
    friend  class QueueVoid;
};

typedef  class QueueElementVoid * QueueElementVoidArray;

class QueueVoid {
public:
    ~QueueVoid();
    static ConstructDestructCallback *getConstructDestructCallback();
    void clear();
    int getNumberFree();
    int capacity();
    QueueElementVoid *getFree();
    void setUsed(QueueElementVoid *queueElement);
    QueueElementVoid *getUsed();
    void releaseUsed(QueueElementVoid *queueElement);
protected:
    QueueVoid(QueueElementVoidPtrArray array,int number);
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



