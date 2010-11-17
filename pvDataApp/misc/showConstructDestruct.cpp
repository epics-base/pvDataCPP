/* showConstructDestruct.cpp */
#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

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

ShowConstructDestruct::ShowConstructDestruct() {}

void ShowConstructDestruct::constuctDestructTotals(FILE *fd)
{
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
   Lock xx(globalMutex);
   ListNode *listNode = new ListNode(callback);
   list->addTail(listNode);
}

ShowConstructDestruct * getShowConstructDestruct()
{
    static Mutex mutex = Mutex();
    Lock xx(&mutex);
    if(pShowConstructDestruct==0) {
        globalMutex = new Mutex();
        list = new List();
        pShowConstructDestruct = new ShowConstructDestruct();
    }
    return pShowConstructDestruct;
}

}}
