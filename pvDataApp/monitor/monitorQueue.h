/* monitorQueue.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/* Marty Kraimer 2011.03 */
#ifndef MONITORQUEUE_H
#define MONITORQUEUE_H
#include <string>
#include <stdexcept>
#include <memory>
#include <vector>

#include <pvData.h>
#include <monitor.h>
#include <queue.h>
#include <sharedPtr.h>

namespace epics { namespace pvData { 

class MonitorQueue {
public:
    MonitorQueue(MonitorElement** elements,int number);
    ~MonitorQueue();
    static MonitorElement** createElements(PVStructurePtrArray array,int number);
    void clear();
    int getNumberFree();
    int capacity();
    MonitorElement *getFree();
    void setUsed(MonitorElement * element);
    MonitorElement * getUsed();
    void releaseUsed(MonitorElement * element);
private:
    int number;
    MonitorElement ** elements;
    Queue<MonitorElement> *queue;
};

}}

#endif  /* MONITORQUEUE_H */
