/* CDRMonitor.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef CDRMONITOR_H
#define CDRMONITOR_H
#include <ostream>
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>

#include <epicsThread.h>
#include "noDefaultMethods.h"
#include "lock.h"
#include "pvType.h"

namespace epics { namespace pvData { 

//! Used to pass around snapshots
struct CDRCount { // default copy and assignment are ok
    size_t cons, dtys;
    long refs;
    CDRCount():cons(0),dtys(0),refs(0){}
    CDRCount& operator+=(const CDRCount& o)
        {cons+=o.cons; dtys+=o.dtys; refs+=o.refs; return *this;}
    CDRCount& operator=(size_t count) // reset counters
        {cons=count; dtys=count; refs=count; return *this;}
};

class CDRNode;

//! @brief Global registrar for CDRNodes
class CDRMonitor : private NoDefaultMethods {
public:
    static CDRMonitor& get();

    CDRNode* addNode(CDRNode& next)
    {
        CDRNode *ret=firstNode;
        firstNode=&next;
        return ret;
    }

    CDRCount current(); //!< current global count

    CDRNode* first() const{return firstNode;}

    void show(FILE*);
    void show(std::ostream&) const;
private:
    // Private ctor for singleton
    CDRMonitor();
    CDRNode *firstNode;

    static CDRMonitor *theone;
    static void init(void*);
};

//! Counters for Construction, Destruction, and References of one class
class CDRNode : private NoDefaultMethods {
public:
    CDRNode(const String& name)
        :nodeName(name)
        ,current()
        ,guard()
        ,nextNode(CDRMonitor::get().addNode(*this))
    {}
    void construct(){Lock x(guard); current.cons++;}
    void destruct(){Lock x(guard); current.dtys++;}
    void incRef(){Lock x(guard); current.refs++;}
    void decRef(){Lock x(guard); current.refs--;}

    CDRNode* next() const{return nextNode;}

    CDRCount get() const{Lock x(guard); return current;}

    void show(FILE*);
    void show(std::ostream&) const;
private:
    const String nodeName;
    CDRCount current;
    mutable Mutex guard;
    CDRNode * const nextNode;
};

struct CDRNodeInstance
{
    CDRNode *node;
    epicsThreadOnceId once;
    const char* const name;
};

void onceNode(void* raw);

static inline
CDRNode*
getNode(CDRNodeInstance *inst)
{
    epicsThreadOnce(&inst->once,&onceNode,
                    static_cast<void*>(inst));
    return inst->node;
}

#ifndef NDEBUG

#define PVDATA_REFCOUNT_MONITOR_DEFINE(NAME) \
static CDRNodeInstance NAME ## _node={0,EPICS_THREAD_ONCE_INIT,#NAME}

#define PVDATA_REFCOUNT_MONITOR_DESTRUCT(NAME) \
            getNode(&NAME ## _node)->destruct()

#define PVDATA_REFCOUNT_MONITOR_CONSTRUCT(NAME) \
            getNode(&NAME ## _node)->construct()

#define PVDATA_REFCOUNT_MONITOR_INCREF(NAME) \
            getNode(&NAME ## _node)->incRef()
#define PVDATA_REFCOUNT_MONITOR_DECREF(NAME) \
            getNode(&NAME ## _node)->decRef()


#else

#define PVDATA_REFCOUNT_MONITOR_DEFINE(NAME)
#define PVDATA_REFCOUNT_MONITOR_DESTRUCT(NAME)
#define PVDATA_REFCOUNT_MONITOR_CONSTRUCT(NAME)
#define PVDATA_REFCOUNT_MONITOR_INCREF(NAME)
#define PVDATA_REFCOUNT_MONITOR_DECREF(NAME)

#endif

}}

std::ostream& operator<<(std::ostream&,const epics::pvData::CDRMonitor&);
std::ostream& operator<<(std::ostream&,const epics::pvData::CDRNode&);

#endif  /* CDRMONITOR_H */
