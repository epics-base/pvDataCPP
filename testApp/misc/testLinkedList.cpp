/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/*
 * testLinkedList.cpp
 *
 *  Created on: 2010.11
 *      Author: Marty Kraimer
 */

#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>
#include <list>

#include <epicsAssert.h>
#include <epicsExit.h>

#include "lock.h"
#include "timeStamp.h"
#include "linkedList.h"
#include "CDRMonitor.h"


using namespace epics::pvData;

static int numNodes = 5;
class Basic;
typedef LinkedListNode<Basic> BasicListNode;
typedef LinkedList<Basic> BasicList;

class Basic {
public:
    Basic(int i): index(i),node(*this) {}
    ~Basic() { }
    int index;
    BasicListNode node;
};
    
static void testBasic(FILE * fd ) {
    LinkedList<Basic> basicList;
    Basic *basics[numNodes];
    for(int i=0; i<numNodes; i++) {
        basics[i] = new Basic(i);
        basicList.addTail(basics[i]->node);
        assert(basicList.getLength()==i+1);
    }
    BasicListNode *basicNode = basicList.getHead();
    fprintf(fd,"basic addTail");
    while(basicNode!=0) {
        fprintf(fd," %d",basicNode->getObject().index);
        basicNode = basicList.getNext(*basicNode);
    }
    assert(basicList.isEmpty()==false);
    basicNode = basicList.getTail();
    while(basicNode!=0) {
        fprintf(fd," %d",basicNode->getObject().index);
        assert(basicNode->isOnList());
        basicNode = basicList.getPrev(*basicNode);
    }
    fprintf(fd,"\n"); 
    for(int i=0; i<numNodes; i++) {
        basicNode = basicList.getHead();
        assert(basicNode!=0);
        Basic &basic = basicNode->getObject();
        assert(basic.index==i);
        assert(basics[i]->node.isOnList()==true);
        basicList.remove(basics[i]->node);
        assert(basics[i]->node.isOnList()==false);
        int length = basicList.getLength();
        assert(length==(numNodes-i-1));
    }
    assert(basicList.isEmpty());
    for(int i=numNodes-1; i>=0; i--) {
        basicList.addHead(basics[i]->node);
        assert(basicList.getLength()==numNodes-i);
    }
    basicNode = basicList.getHead();
    fprintf(fd,"basic addHead");
    while(basicNode!=0) {
        fprintf(fd," %d",basicNode->getObject().index);
        basicNode = basicList.getNext(*basicNode);
    }
    fprintf(fd,"\n"); 
    for(int i=0; i<numNodes; i++) {
        basicNode = basicList.getHead();
        assert(basicNode!=0);
        Basic &basic = basicNode->getObject();
        assert(basic.index==i);
        basicList.removeHead();
        assert(basic.node.isOnList()==false);
        int length = basicList.getLength();
        assert(length==(numNodes-i-1));
    }
    assert(basicList.isEmpty());
    basicList.addTail(basics[0]->node);
    basicNode = basicList.getTail();
    assert(basicNode->getObject().index==0);
    for(int i=1;i<numNodes;i++) {
        basicList.insertAfter(*basicNode,basics[i]->node);
        basicNode = basicList.getTail();
        assert(basicList.getLength()==i+1);
    }
    fprintf(fd,"basic addTail insertAfter");
    basicNode = basicList.getHead();
    while(basicNode!=0) {
        fprintf(fd," %d",basicNode->getObject().index);
        basicNode = basicList.getNext(*basicNode);
    }
    fprintf(fd,"\n"); 
    for(int i=numNodes-1; i>=0; i--) {
        Basic &basic = basicList.getTail()->getObject();
        assert(basic.index==i);
        basicList.removeTail();
        assert(basic.node.isOnList()==false);
        int length = basicList.getLength();
        assert(length==i);
    }
    assert(basicList.isEmpty());
    basicList.addHead(basics[numNodes-1]->node);
    basicNode = basicList.getHead();
    assert(basicNode->getObject().index==4);
    for(int i=numNodes-2; i>=0; i--) {
        basicList.insertBefore(*basicNode,basics[i]->node);
        basicNode = basicList.getHead();
        assert(basicList.getLength()==numNodes-i);
    }
    fprintf(fd,"basic addTail insertBefore");
    basicNode = basicList.getHead();
    while(basicNode!=0) {
        fprintf(fd," %d",basicNode->getObject().index);
        basicNode = basicList.getNext(*basicNode);
    }
    fprintf(fd,"\n"); 
    for(int i=numNodes-1; i>=0; i--) {
        Basic &basic = basicList.getTail()->getObject();
        assert(basic.index==i);
        basicList.remove(basic.node);
        assert(basic.node.isOnList()==false);
        int length = basicList.getLength();
        assert(length==i);
    }
    assert(basicList.isEmpty());
    for(int i=0; i<numNodes; i++) delete basics[i];
}

static void testQueue(FILE * fd ) {
    LinkedList<Basic> basicList;
    Basic *basics[numNodes];
    for(int i=0; i<numNodes; i++) {
        basics[i] = new Basic(i);
    }
    fprintf(fd,"\nQueue test\n");
    for(int i=0;i<numNodes;i++) {
        basicList.addTail(basics[i]->node);
        assert(basicList.getLength()==i+1);
    }
    BasicListNode *basicNode = basicList.removeHead();
    while(basicNode!=0) basicNode = basicList.removeHead();
    for(int i=0;i<numNodes;i++) basicList.addTail(basics[i]->node);
    basicNode = basicList.removeHead();
    fprintf(fd,"queue");
    while(basicNode!=0) {
        fprintf(fd," %d",basicNode->getObject().index);
        basicNode = basicList.removeHead();
    }
    fprintf(fd,"\n");
    assert(basicList.isEmpty());
    for(int i=0; i<numNodes; i++) delete basics[i];
}

static void testStack(FILE * fd ) {
    LinkedList<Basic> basicList;
    Basic *basics[numNodes];
    for(int i=0; i<numNodes; i++) {
        basics[i] = new Basic(i);
    }
    fprintf(fd,"\nStack test\n");
    for(int i=0;i<numNodes;i++) {
        basicList.addHead(basics[i]->node);
        assert(basicList.getLength()==i+1);
    }
    BasicListNode *basicNode = basicList.removeHead();
    while(basicNode!=0) basicNode = basicList.removeHead();
    for(int i=0;i<numNodes;i++) basicList.addHead(basics[i]->node);
    basicNode = basicList.removeHead();
    fprintf(fd,"stack");
    while(basicNode!=0) {
        fprintf(fd," %d",basicNode->getObject().index);
        basicNode = basicList.removeHead();
    }
    fprintf(fd,"\n");
    assert(basicList.isEmpty());
    for(int i=0; i<numNodes; i++) delete basics[i];
}

static void testList(FILE * fd ) {
    LinkedList<Basic> basicList;
    Basic *basics[numNodes];
    for(int i=0; i<numNodes; i++) {
        basics[i] = new Basic(i);
    }
    fprintf(fd,"\ntestList\n");
    for(int i=0;i<numNodes;i++) basicList.addTail(basics[i]->node);
    fprintf(fd,"list");
    BasicListNode *basicNode = basicList.removeHead();
    while(basicNode!=0) {
        fprintf(fd," %d",basicNode->getObject().index);
        basicNode = basicList.removeHead();
    }
    fprintf(fd,"\n");
    assert(basicList.isEmpty());
    for(int i=0; i<numNodes; i++) delete basics[i];
}

static void testRandomInsertRemove(FILE * fd ) {
    LinkedList<Basic> basicList;
    Basic *basics[numNodes];
    for(int i=0; i<numNodes; i++) {
        basics[i] = new Basic(i);
    }
    fprintf(fd,"\nRandom insert/remove test\n");
    basicList.addHead(basics[4]->node);
    basicList.insertAfter(basics[4]->node,basics[3]->node);
    basicList.insertAfter(basics[3]->node,basics[2]->node);
    basicList.addTail(basics[1]->node);
    basicList.addTail(basics[0]->node);
    BasicListNode *basicNode = basicList.removeHead();
    fprintf(fd,"stack");
    while(basicNode!=0) {
        fprintf(fd," %d",basicNode->getObject().index);
        basicNode = basicList.removeHead();
    }
    fprintf(fd,"\n");
    assert(basicList.isEmpty());
    for(int i=0; i<numNodes; i++) delete basics[i];
}

static void testOrderedQueue(FILE * fd ) {
    LinkedList<Basic> basicList;
    Basic *basics[numNodes];
    for(int i=0; i<numNodes; i++) {
        basics[i] = new Basic(i);
    }
    BasicListNode *basicNode = 0;
    fprintf(fd,"\nOrdered Queue test\n");
    basicList.addHead(basics[2]->node);
    for(int i=0;i<numNodes;i++) {
        if(basics[i]->node.isOnList()) continue;
        basicNode = basicList.getHead();
        while(basicNode!=0) {
            if(basicNode->getObject().index>=basics[i]->index) {
                basicList.insertBefore(*basicNode,basics[i]->node);
                break;
            }
            basicNode = basicList.getNext(*basicNode);
        }
        if(basics[i]->node.isOnList()) continue;
        basicList.addTail(basics[i]->node);
    }
    fprintf(fd,"list");
    basicNode = basicList.removeHead();
    while(basicNode!=0) {
        fprintf(fd," %d",basicNode->getObject().index);
        basicNode = basicList.removeHead();
    }
    fprintf(fd,"\n");
    assert(basicList.isEmpty());
    for(int i=0; i<numNodes; i++) delete basics[i];
}

static void testTime(FILE *auxFd) {
    TimeStamp startTime;
    TimeStamp endTime;
    int numNodes = 1000;

    LinkedList<Basic> basicList;
    Basic *basics[numNodes];
    for(int i=0; i<numNodes; i++) {
        basics[i] = new Basic(i);
    }
    fprintf(auxFd,"\nTime test\n");
    int ntimes = 1000;
    startTime.getCurrent();
    for(int i=0; i<ntimes; i++) {
        for(int j=0;j<numNodes;j++) basicList.addTail(basics[j]->node);
        BasicListNode *basicNode = basicList.removeHead();
        while(basicNode!=0) basicNode = basicList.removeHead();
    }
    endTime.getCurrent();
    double diff = TimeStamp::diff(endTime,startTime);
    diff *= 1000.0;
    fprintf(auxFd,"diff %f milliSeconds\n",diff);
    diff = diff/1000.0; // convert from milliseconds to seconds
    diff = diff/ntimes; // seconds per outer loop
    diff = diff*1e6; // converty to microseconds
    fprintf(auxFd,"time per iteration %f microseconds\n",diff);
    diff = diff/(numNodes*2); // convert to per addTail/removeHead
    fprintf(auxFd,"time per addTail/removeHead %f microseconds\n",diff);
    assert(basicList.isEmpty());
    for(int i=0; i<numNodes; i++) delete basics[i];
}

static void testTimeLocked(FILE *auxFd) {
    TimeStamp startTime;
    TimeStamp endTime;
    Mutex mutex;
    int numNodes = 1000;

    LinkedList<Basic> basicList;
    Basic *basics[numNodes];
    for(int i=0; i<numNodes; i++) {
        basics[i] = new Basic(i);
    }
    fprintf(auxFd,"\nTime test locked\n");
    int ntimes = 1000;
    startTime.getCurrent();
    for(int i=0; i<ntimes; i++) {
        for(int j=0;j<numNodes;j++) {
            Lock xx(mutex);
            basicList.addTail(basics[j]->node);
        }
        BasicListNode *basicNode = 0;
        {
            Lock xx(mutex);
            basicNode = basicList.removeHead();
        }
        while(basicNode!=0) {
            Lock xx(mutex);
           basicNode = basicList.removeHead();
        }
    }
    endTime.getCurrent();
    double diff = TimeStamp::diff(endTime,startTime);
    diff *= 1000.0;
    fprintf(auxFd,"diff %f milliSeconds\n",diff);
    diff = diff/1000.0; // convert from milliseconds to seconds
    diff = diff/ntimes; // seconds per outer loop
    diff = diff*1e6; // converty to microseconds
    fprintf(auxFd,"time per iteration %f microseconds\n",diff);
    diff = diff/(numNodes*2); // convert to per addTail/removeHead
    fprintf(auxFd,"time per addTail/removeHead %f microseconds\n",diff);
    assert(basicList.isEmpty());
    for(int i=0; i<numNodes; i++) delete basics[i];
}

typedef std::list<Basic *> stdList;
static void testStdListTime(FILE *auxFd) {
    TimeStamp startTime;
    TimeStamp endTime;
    int numNodes = 1000;

    stdList basicList;
    Basic *basics[numNodes];
    for(int i=0; i<numNodes; i++) {
        basics[i] = new Basic(i);
    }
    fprintf(auxFd,"\nTime std::list test\n");
    int ntimes = 1000;
    startTime.getCurrent();
    for(int i=0; i<ntimes; i++) {
        for(int j=0;j<numNodes;j++) basicList.push_back(basics[j]);
        while(basicList.size()>0) {
            basicList.begin();
            basicList.pop_front();
        }
    }
    endTime.getCurrent();
    double diff = TimeStamp::diff(endTime,startTime);
    diff *= 1000.0;
    fprintf(auxFd,"diff %f milliSeconds\n",diff);
    diff = diff/1000.0; // convert from milliseconds to seconds
    diff = diff/ntimes; // seconds per outer loop
    diff = diff*1e6; // converty to microseconds
    fprintf(auxFd,"time per iteration %f microseconds\n",diff);
    diff = diff/(numNodes*2); // convert to per addTail/removeHead
    fprintf(auxFd,"time per addTail/removeHead %f microseconds\n",diff);
    for(int i=0; i<numNodes; i++) delete basics[i];
}

static void testStdListTimeLocked(FILE *auxFd) {
    TimeStamp startTime;
    TimeStamp endTime;
    int numNodes = 1000;
    Mutex mutex;

    stdList basicList;
    Basic *basics[numNodes];
    for(int i=0; i<numNodes; i++) {
        basics[i] = new Basic(i);
    }
    fprintf(auxFd,"\nTime std::list test locked\n");
    int ntimes = 1000;
    startTime.getCurrent();
    for(int i=0; i<ntimes; i++) {
        for(int j=0;j<numNodes;j++) {
            Lock xx(mutex);
            basicList.push_back(basics[j]);
        }
        while(basicList.size()>0) {
            Lock xx(mutex);
            basicList.begin();
            basicList.pop_front();
        }
    }
    endTime.getCurrent();
    double diff = TimeStamp::diff(endTime,startTime);
    diff *= 1000.0;
    fprintf(auxFd,"diff %f milliSeconds\n",diff);
    diff = diff/1000.0; // convert from milliseconds to seconds
    diff = diff/ntimes; // seconds per outer loop
    diff = diff*1e6; // converty to microseconds
    fprintf(auxFd,"time per iteration %f microseconds\n",diff);
    diff = diff/(numNodes*2); // convert to per addTail/removeHead
    fprintf(auxFd,"time per addTail/removeHead %f microseconds\n",diff);
    for(int i=0; i<numNodes; i++) delete basics[i];
}

int main(int argc, char *argv[]) {
     char *fileName = 0;
    if(argc>1) fileName = argv[1];
    FILE * fd = stdout;
    if(fileName!=0 && fileName[0]!=0) {
        fd = fopen(fileName,"w+");
    }
    char *auxFileName = 0;
    if(argc>2) auxFileName = argv[2];
    FILE *auxFd = stdout;
    if(auxFileName!=0 && auxFileName[0]!=0) {
        auxFd = fopen(auxFileName,"w+");
    }
    testBasic(fd);
    testQueue(fd);
    testStack(fd);
    testList(fd);
    testRandomInsertRemove(fd);
    testOrderedQueue(fd);
    testTime(auxFd);
    testTimeLocked(auxFd);
    testStdListTime(auxFd);
    testStdListTimeLocked(auxFd);
    epicsExitCallAtExits();
    CDRMonitor::get().show(fd);
    return (0);
}
 
