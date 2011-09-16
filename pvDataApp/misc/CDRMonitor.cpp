/* CDRMonitor.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdexcept>

#include <epicsThread.h>

#include <pv/noDefaultMethods.h>
#include <pv/lock.h>
#include <pv/pvType.h>
#include <pv/linkedList.h>
#include <pv/CDRMonitor.h>

#ifdef _WIN32
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

namespace epics { namespace pvData {

static
epicsThreadOnceId monitorInit = EPICS_THREAD_ONCE_INIT;

// Must use a pointer w/ lazy init due to lack of
// initialization order guarantees
CDRMonitor* CDRMonitor::theone = 0;

CDRMonitor&
CDRMonitor::get()
{
    epicsThreadOnce(&monitorInit, &CDRMonitor::init, 0);
    assert(theone);
    return *theone;
}

void
CDRMonitor::init(void *)
{
    //BUG: No idea how to handle allocation failure at this stage.
    theone=new CDRMonitor;
}

void
CDRMonitor::destroy()
{
    if (theone)
    {
        CDRNode *node = theone->first();
        while (node)
        {
            CDRNode* tmp = node;
            node = node->next();
            delete tmp;
        }
        delete theone;
        theone = 0;
    }
}

CDRMonitor::CDRMonitor()
    :firstNode(0)
{}

CDRCount
CDRMonitor::current()
{
    CDRCount total;
    for(CDRNode *cur=first(); !!cur; cur=cur->next())
    {
        total+=cur->get();
    }
    return total;
}

void
CDRMonitor::show(FILE *fd, bool destroy)
{
    for(CDRNode *cur=first(); !!cur; cur=cur->next())
    {
        cur->show(fd);
    }
    if (destroy)
        CDRMonitor::destroy();
}

void
CDRMonitor::show(std::ostream& out, bool destroy) const
{
    for(CDRNode *cur=first(); !!cur; cur=cur->next())
    {
        cur->show(out);
    }
    if (destroy)
        CDRMonitor::destroy();
}

void
CDRNode::show(FILE *fd)
{
    Lock x(guard);
    if(!current.cons && !current.dtys && !current.refs)
        return;
    fprintf(fd,"%s:  totalConstruct %lu totalDestruct %lu",
            nodeName.c_str(), (unsigned long)current.cons,
            (unsigned long)current.dtys);
    ssize_t alive=current.cons;
    alive-=current.dtys;
    if(current.refs)
        fprintf(fd," totalReference %ld", current.refs);
    if(alive)
        fprintf(fd," ACTIVE %ld\n", (long)alive);
    else
        fprintf(fd,"\n");
}

void
CDRNode::show(std::ostream& out) const
{
    Lock x(guard);
    if(!current.cons && !current.dtys && !current.refs)
        return;
    out<<nodeName<<"  totalConstruct "<<current.cons
            <<" totalDestruct "<<current.dtys;
    ssize_t alive=current.cons;
    alive-=current.dtys;
    if(current.refs)
        out<<" totalReference "<<current.refs;
    if(alive)
        out<<" ACTIVE "<<alive;
    out<<"\n";
}

void
onceNode(void* raw)
{
    CDRNodeInstance* inst=static_cast<CDRNodeInstance*>(raw);
    inst->node=new CDRNode(inst->name);
}

}} // namespace epics::pvData

std::ostream& operator<<(std::ostream& out,const epics::pvData::CDRMonitor& mon)
{
    mon.show(out);
    return out;
}

std::ostream& operator<<(std::ostream& out,const epics::pvData::CDRNode& node)
{
    node.show(out);
    return out;
}
