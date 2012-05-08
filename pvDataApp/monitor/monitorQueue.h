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

#include <pv/pvData.h>
#include <pv/monitor.h>
#include <pv/queue.h>
#include <pv/sharedPtr.h>

namespace epics { namespace pvData { 

typedef PVStructure::shared_pointer* PVStructureSharedPointerPtr;
typedef PVStructureSharedPointerPtr* PVStructureSharedPointerPtrArray;

class MonitorQueue {
public:
    MonitorQueue(PVStructureSharedPointerPtrArray structures,int number);
    ~MonitorQueue();
    static PVStructureSharedPointerPtrArray createStructures(
        PVStructurePtrArray array,int number);
    void clear();
    int getNumberFree();
    int capacity();
    MonitorElementPtr & getFree();
    void setUsed(MonitorElementPtr & element);
    MonitorElementPtr getUsed();
    void releaseUsed(MonitorElementPtr & element);
private:
    int number;
    PVStructureSharedPointerPtrArray structures;
    Queue<MonitorElementPtr> *queue;
    MonitorElementPtr **queueElements;
    MonitorElementPtr nullElement;
};

}}

#endif  /* MONITORQUEUE_H */
