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
static Mutex globalMutex;
static bool notInited = true;
typedef LinkedListNode<ConstructDestructCallback> ListNode;
typedef LinkedList<ConstructDestructCallback> List;
static List *list;

/* list callbacks are special because showConstructDestruct creates a list
   Thus list can be null when list calls registerCallback
   The list callbacks are not put on the list but handled separately
 */
static ConstructDestructCallback *listCallback = 0;
static ConstructDestructCallback *listNodeCallback = 0;

ConstructDestructCallback::ConstructDestructCallback(
    String name,
    getTotalFunc construct,
    getTotalFunc destruct,
    getTotalFunc reference,
    deleteStaticFunc deleteFunc)
: name(name), construct(construct), destruct(destruct) ,reference(reference),
  deleteFunc(deleteFunc)
{ }

ConstructDestructCallback::~ConstructDestructCallback() {}

String ConstructDestructCallback::getConstructName()
{
    return name;
}

int64 ConstructDestructCallback::getTotalConstruct()
{
    if(construct==0) return 0;
    return construct();
}

int64 ConstructDestructCallback:: getTotalDestruct()
{
    if(destruct==0) return 0;
    return destruct();
}

int64 ConstructDestructCallback::getTotalReferenceCount()
{
    if(reference==0) return 0;
    return reference();
}

void ConstructDestructCallback::deleteStatic()
{
    if(deleteFunc==0) return;
    deleteFunc();
}

ShowConstructDestruct::ShowConstructDestruct() {}

ShowConstructDestruct::~ShowConstructDestruct() {
    delete listCallback;
    delete listNodeCallback;
    listCallback = 0;
    listNodeCallback = 0;
}

void ShowConstructDestruct::registerCallback(
    String name,
    getTotalFunc construct,
    getTotalFunc destruct,
    getTotalFunc reference,
        deleteStaticFunc deleteFunc)
{
    getShowConstructDestruct(); // make it initialize
    Lock xx(&globalMutex);
    if(name.compare("linkedList")==0) {
        listCallback = new ConstructDestructCallback(
            name,construct,destruct,reference,deleteFunc);
        return;
    } else if(name.compare("linkedListNode")==0) {
        listNodeCallback = new ConstructDestructCallback(
            name,construct,destruct,reference,deleteFunc);
        return;
    }
    if(list==0) {
        throw std::logic_error(String(
            "ShowConstructDestruct::registerCallback"));
    }
    ConstructDestructCallback *callback = new ConstructDestructCallback(
        name,construct,destruct,reference,deleteFunc);
    ListNode *listNode = new ListNode(callback);
    list->addTail(listNode);
}
static void showOne(ConstructDestructCallback *callback,FILE *fd)
{
    String name = callback->getConstructName();
    int64 reference = callback->getTotalReferenceCount();
    int64 construct = callback->getTotalConstruct();
    int64 destruct = callback->getTotalDestruct();
    if(reference==0&&construct==0&&destruct==0) return;
    fprintf(fd,"%s: ", name.c_str());
    if(construct>0 || destruct>0) {
        fprintf(fd," totalConstruct %lli totalDestruct %lli",
        construct,destruct);
    }
    if(reference>0) fprintf(fd," totalReference %lli",reference);
    int64 diff = construct - destruct;
    if(diff!=0) fprintf(fd," ACTIVE %lli",diff);
    fprintf(fd,"\n");
}

ConstructDestructCallback* ShowConstructDestruct::getConstructDestructCallback(
    String name)
{
    if(name.compare(listNodeCallback->getConstructName())==0) {
        return listNodeCallback;
    }
    if(name.compare(listCallback->getConstructName())==0) {
        return listCallback;
    }
    Lock xx(&globalMutex);
    ListNode *node = list->getHead();
    while(node!=0) {
        ConstructDestructCallback *callback = node->getObject();
        if(name.compare(callback->getConstructName())==0) {
            return callback;
        }
        node = list->getNext(node);
    }
    return 0;
}

void ShowConstructDestruct::constuctDestructTotals(FILE *fd)
{
    getShowConstructDestruct(); // make it initialize
    Lock xx(&globalMutex);
    ListNode *node = list->getHead();
    while(node!=0) {
        ConstructDestructCallback *callback = node->getObject();
        showOne(callback,fd);
        node = list->getNext(node);
    }
    showOne(listNodeCallback,fd);
    showOne(listCallback,fd);
}

void ShowConstructDestruct::showDeleteStaticExit(FILE *fd)
{
    getShowConstructDestruct(); // make it initialize
    {
        Lock xx(&globalMutex);
        ListNode *node = list->getHead();
        while(node!=0) {
            ConstructDestructCallback *callback = node->getObject();
            if(callback->deleteFunc!=0) callback->deleteFunc();
            node = list->getNext(node);
        }
        node = list->getHead();
        while(node!=0) {
            ConstructDestructCallback *callback = node->getObject();
            showOne(callback,fd);
            list->removeHead();
            delete callback;
            delete node;
            node = list->getHead();
        }
        delete list;
        if(listNodeCallback->deleteFunc!=0) listNodeCallback->deleteFunc();
        if(listCallback->deleteFunc!=0) listCallback->deleteFunc();
        showOne(listNodeCallback,fd);
        showOne(listCallback,fd);
        delete pShowConstructDestruct;
        pShowConstructDestruct = 0;
    }
    exit( 0);
}

ShowConstructDestruct * getShowConstructDestruct()
{
    static Mutex mutex;
    Lock xx(&mutex);
    if(notInited) {
        notInited = false;
        pShowConstructDestruct = new ShowConstructDestruct();
        List *listTemp;
        listTemp = new List();
        list = listTemp;
    } 
    return pShowConstructDestruct;
}

}}
