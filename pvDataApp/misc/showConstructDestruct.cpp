/* showConstructDestruct.cpp */
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

#include "noDefaultMethods.h"
#include "lock.h"
#include "pvType.h"
#include "linkedList.h"
#include "showConstructDestruct.h"

namespace epics { namespace pvData { 

static ShowConstructDestruct *pShowConstructDestruct = 0;
static Mutex *globalMutex = 0;
typedef LinkedListNode<ConstructDestructCallback> ListNode;
typedef LinkedList<ConstructDestructCallback> List;
static List *list;

ConstructDestructCallback::ConstructDestructCallback(
    String name,
    getTotal construct,
    getTotal destruct,
    getTotal reference)
: name(name), construct(construct), destruct(destruct) ,reference(reference)
{
    getShowConstructDestruct()->registerCallback(this);
}

ConstructDestructCallback::~ConstructDestructCallback() {}

String ConstructDestructCallback::getConstructName()
{
    return name;
}

int64 ConstructDestructCallback::getTotalConstruct()
{
    return construct();
}

int64 ConstructDestructCallback:: getTotalDestruct()
{
    return destruct();
}

int64 ConstructDestructCallback::getTotalReferenceCount()
{
    if(reference==0) return 0;
    return reference();
}


ShowConstructDestruct::ShowConstructDestruct() {}

void ShowConstructDestruct::constuctDestructTotals(FILE *fd)
{
    getShowConstructDestruct(); // make it initialize
    Lock xx(globalMutex);
    ListNode *node = list->getHead();
    while(node!=0) {
        ConstructDestructCallback *callback = node->getObject();
        String name = callback->getConstructName();
        int64 reference = callback->getTotalReferenceCount();
        int64 construct = callback->getTotalConstruct();
        int64 destruct = callback->getTotalDestruct();
        fprintf(fd,"%s: totalConstruct %lli totalDestruct %lli",
            name.c_str(),construct,destruct);
        if(reference>0) fprintf(fd," totalReference %lli",reference);
        fprintf(fd,"\n");
        node = list->getNext(node);
    }
}

void ShowConstructDestruct::registerCallback(ConstructDestructCallback *callback)
{
    static ConstructDestructCallback *listCallback = 0;
    static ConstructDestructCallback *listNodeCallback = 0;
   Lock xx(globalMutex);
   ListNode *listNode = 0;
   if(list==0) {
       if(callback->getConstructName().compare("linkedListNode")==0) {
           listNodeCallback = callback;
       } else if(callback->getConstructName().compare("linkedList")==0) {
           listCallback = callback;
       } else {
           throw std::logic_error(String("ShowConstructDestruct::registerCallback"));
       }
       return;
   }
   if(listCallback!=0) {
       if(listNodeCallback==0) {
           throw std::logic_error(String(
               "ShowConstructDestruct::registerCallback expected listNodeCallback!=0"));
       }
       listNode = new ListNode(listNodeCallback);
       list->addTail(listNode);
       listNode = new ListNode(listCallback);
       list->addTail(listNode);
       listCallback = 0;
       listNodeCallback = 0;
   }
   listNode = new ListNode(callback);
   list->addTail(listNode);
}

ShowConstructDestruct * getShowConstructDestruct()
{
    static Mutex mutex = Mutex();
    Lock xx(&mutex);
    if(pShowConstructDestruct==0) {
        globalMutex = new Mutex();
        pShowConstructDestruct = new ShowConstructDestruct();
        list = new List();
    } 
    return pShowConstructDestruct;
}

}}
