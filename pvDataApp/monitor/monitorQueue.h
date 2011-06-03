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
    MonitorElement::shared_pointer const & getFree();
    void setUsed(MonitorElement::shared_pointer const & element);
    MonitorElement::shared_pointer const & getUsed();
    void releaseUsed(MonitorElement::shared_pointer const & element);
private:
    int number;
    PVStructureSharedPointerPtrArray structures;
    Queue<MonitorElement::shared_pointer> *queue;
    MonitorElement::shared_pointer **queueElements;
    MonitorElement::shared_pointer nullElement;
};

}}

#endif  /* MONITORQUEUE_H */
