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
    MonitorQueue(PVStructure::shared_pointer* structures,int number);
    ~MonitorQueue();
    static PVStructure::shared_pointer* createStructures(
        PVStructurePtrArray array,int number);
    void clear();
    int getNumberFree();
    int capacity();
    MonitorElement::shared_pointer getFree();
    void setUsed(MonitorElement::shared_pointer element);
    MonitorElement::shared_pointer getUsed();
    void releaseUsed(MonitorElement::shared_pointer element);
private:
    int number;
    PVStructure::shared_pointer* structures;
    Queue<MonitorElement::shared_pointer> *queue;
};

}}

#endif  /* MONITORQUEUE_H */
