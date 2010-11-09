/*
 *  * testLinkedList.cpp
 *   *
 *    *  Created on: 2010.11
 *     *      Author: Marty Kraimer
 *      */

#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>
#include <list>

#include <epicsTime.h>
#include <epicsAssert.h>

#include "lock.h"
#include "linkedList.h"
#include "pvIntrospect.h"
#include "pvData.h"


using namespace epics::pvData;

static int numNodes = 5;
class Basic;
typedef LinkedListNode<Basic> BasicListNode;
typedef LinkedList<Basic> BasicList;

class Basic {
public:
    Basic(int i): index(i),node(new BasicListNode(this)) {}
    ~Basic() { delete node;}
    int index;
    BasicListNode*node;
};
    
static void testBasic(FILE * fd ) {
    LinkedList<Basic> *basicList = new BasicList();
    Basic *basics[numNodes];
    for(int i=0; i<numNodes; i++) {
        basics[i] = new Basic(i);
        basicList->addTail(basics[i]->node);
        assert(basicList->getLength()==i+1);
    }
    BasicListNode *basicNode = basicList->getHead();
    fprintf(fd,"basic addTail");
    while(basicNode!=0) {
        fprintf(fd," %d",basicNode->getObject()->index);
        basicNode = basicList->getNext(basicNode);
    }
    assert(basicList->isEmpty()==false);
    basicNode = basicList->getTail();
    while(basicNode!=0) {
        fprintf(fd," %d",basicNode->getObject()->index);
        assert(basicList->contains(basicNode->getObject()));
        basicNode = basicList->getPrev(basicNode);
    }
    fprintf(fd,"\n"); 
    for(int i=0; i<numNodes; i++) {
        Basic *basic = basicList->getHead()->getObject();
        assert(basic->index==i);
        assert(basics[i]->node->isOnList()==true);
        basicList->remove(basics[i]);
        assert(basics[i]->node->isOnList()==false);
        int length = basicList->getLength();
        assert(length==(numNodes-i-1));
    }
    assert(basicList->isEmpty());
    for(int i=numNodes-1; i>=0; i--) {
        basicList->addHead(basics[i]->node);
        assert(basicList->getLength()==numNodes-i);
    }
    basicNode = basicList->getHead();
    fprintf(fd,"basic addHead");
    while(basicNode!=0) {
        fprintf(fd," %d",basicNode->getObject()->index);
        basicNode = basicList->getNext(basicNode);
    }
    fprintf(fd,"\n"); 
    for(int i=0; i<numNodes; i++) {
        Basic *basic = basicList->getHead()->getObject();
        assert(basic->index==i);
        basicList->removeHead();
        assert(basic->node->isOnList()==false);
        int length = basicList->getLength();
        assert(length==(numNodes-i-1));
    }
    assert(basicList->isEmpty());
    basicList->addTail(basics[0]->node);
    basicNode = basicList->getTail();
    assert(basicNode->getObject()->index==0);
    for(int i=1;i<numNodes;i++) {
        basicList->insertAfter(basicNode,basics[i]->node);
        basicNode = basicList->getTail();
        assert(basicList->getLength()==i+1);
    }
    fprintf(fd,"basic addTail insertAfter");
    basicNode = basicList->getHead();
    while(basicNode!=0) {
        fprintf(fd," %d",basicNode->getObject()->index);
        basicNode = basicList->getNext(basicNode);
    }
    fprintf(fd,"\n"); 
    for(int i=numNodes-1; i>=0; i--) {
        Basic *basic = basicList->getTail()->getObject();
        assert(basic->index==i);
        basicList->removeTail();
        assert(basic->node->isOnList()==false);
        int length = basicList->getLength();
        assert(length==i);
    }
    assert(basicList->isEmpty());
    basicList->addHead(basics[numNodes-1]->node);
    basicNode = basicList->getHead();
    assert(basicNode->getObject()->index==4);
    for(int i=numNodes-2; i>=0; i--) {
        basicList->insertBefore(basicNode,basics[i]->node);
        basicNode = basicList->getHead();
        assert(basicList->getLength()==numNodes-i);
    }
    fprintf(fd,"basic addTail insertBefore");
    basicNode = basicList->getHead();
    while(basicNode!=0) {
        fprintf(fd," %d",basicNode->getObject()->index);
        basicNode = basicList->getNext(basicNode);
    }
    fprintf(fd,"\n"); 
    for(int i=numNodes-1; i>=0; i--) {
        Basic *basic = basicList->getTail()->getObject();
        assert(basic->index==i);
        basicList->remove(basic);
        assert(basic->node->isOnList()==false);
        int length = basicList->getLength();
        assert(length==i);
    }
    assert(basicList->isEmpty());
    delete basicList;
    for(int i=0; i<numNodes; i++) delete basics[i];
}

static void testQueue(FILE * fd ) {
    LinkedList<Basic> *basicList = new BasicList();
    Basic *basics[numNodes];
    for(int i=0; i<numNodes; i++) {
        basics[i] = new Basic(i);
    }
    fprintf(fd,"\nQueue test\n");
    for(int i=0;i<numNodes;i++) {
        basicList->addTail(basics[i]->node);
        assert(basicList->getLength()==i+1);
    }
    BasicListNode *basicNode = basicList->removeHead();
    while(basicNode!=0) basicNode = basicList->removeHead();
    for(int i=0;i<numNodes;i++) basicList->addTail(basics[i]->node);
    basicNode = basicList->removeHead();
    fprintf(fd,"queue");
    while(basicNode!=0) {
        fprintf(fd," %d",basicNode->getObject()->index);
        basicNode = basicList->removeHead();
    }
    fprintf(fd,"\n");
    assert(basicList->isEmpty());
    delete basicList;
    for(int i=0; i<numNodes; i++) delete basics[i];
}

static void testStack(FILE * fd ) {
    LinkedList<Basic> *basicList = new BasicList();
    Basic *basics[numNodes];
    for(int i=0; i<numNodes; i++) {
        basics[i] = new Basic(i);
    }
    fprintf(fd,"\nStack test\n");
    for(int i=0;i<numNodes;i++) {
        basicList->addHead(basics[i]->node);
        assert(basicList->getLength()==i+1);
    }
    BasicListNode *basicNode = basicList->removeHead();
    while(basicNode!=0) basicNode = basicList->removeHead();
    for(int i=0;i<numNodes;i++) basicList->addHead(basics[i]->node);
    basicNode = basicList->removeHead();
    fprintf(fd,"stack");
    while(basicNode!=0) {
        fprintf(fd," %d",basicNode->getObject()->index);
        basicNode = basicList->removeHead();
    }
    fprintf(fd,"\n");
    assert(basicList->isEmpty());
    delete basicList;
    for(int i=0; i<numNodes; i++) delete basics[i];
}

static void testList(FILE * fd ) {
    LinkedList<Basic> *basicList = new BasicList();
    Basic *basics[numNodes];
    for(int i=0; i<numNodes; i++) {
        basics[i] = new Basic(i);
    }
    fprintf(fd,"\ntestList\n");
    for(int i=0;i<numNodes;i++) basicList->addTail(basics[i]->node);
    fprintf(fd,"list");
    BasicListNode *basicNode = basicList->removeHead();
    while(basicNode!=0) {
        fprintf(fd," %d",basicNode->getObject()->index);
        basicNode = basicList->removeHead();
    }
    fprintf(fd,"\n");
    assert(basicList->isEmpty());
    delete basicList;
    for(int i=0; i<numNodes; i++) delete basics[i];
}

static void testRandomInsertRemove(FILE * fd ) {
    LinkedList<Basic> *basicList = new BasicList();
    Basic *basics[numNodes];
    for(int i=0; i<numNodes; i++) {
        basics[i] = new Basic(i);
    }
    fprintf(fd,"\nRandom insert/remove test\n");
    basicList->addHead(basics[4]->node);
    basicList->insertAfter(basics[4]->node,basics[3]->node);
    basicList->insertAfter(basics[3]->node,basics[2]->node);
    basicList->addTail(basics[1]->node);
    basicList->addTail(basics[0]->node);
    BasicListNode *basicNode = basicList->removeHead();
    fprintf(fd,"stack");
    while(basicNode!=0) {
        fprintf(fd," %d",basicNode->getObject()->index);
        basicNode = basicList->removeHead();
    }
    fprintf(fd,"\n");
    assert(basicList->isEmpty());
    delete basicList;
    for(int i=0; i<numNodes; i++) delete basics[i];
}

static void testOrderedQueue(FILE * fd ) {
    LinkedList<Basic> *basicList = new BasicList();
    Basic *basics[numNodes];
    for(int i=0; i<numNodes; i++) {
        basics[i] = new Basic(i);
    }
    BasicListNode *basicNode = 0;
    fprintf(fd,"\nOrdered Queue test\n");
    basicList->addHead(basics[2]->node);
    for(int i=0;i<numNodes;i++) {
        if(basicList->contains(basics[i]->node->getObject())) continue;
        basicNode = basicList->getHead();
        while(basicNode!=0) {
            if(basicNode->getObject()->index>=basics[i]->index) {
                basicList->insertBefore(basicNode,basics[i]->node);
                break;
            }
            basicNode = basicList->getNext(basicNode);
        }
        if(basicList->contains(basics[i]->node->getObject())) continue;
        basicList->addTail(basics[i]->node);
    }
    fprintf(fd,"list");
    basicNode = basicList->removeHead();
    while(basicNode!=0) {
        fprintf(fd," %d",basicNode->getObject()->index);
        basicNode = basicList->removeHead();
    }
    fprintf(fd,"\n");
    assert(basicList->isEmpty());
    delete basicList;
    for(int i=0; i<numNodes; i++) delete basics[i];
}

static void testTime() {
    epicsTimeStamp  startTime;
    epicsTimeStamp  endTime;
    int numNodes = 1000;

    LinkedList<Basic> *basicList = new BasicList();
    Basic *basics[numNodes];
    for(int i=0; i<numNodes; i++) {
        basics[i] = new Basic(i);
    }
    printf("\nTime test\n");
    int ntimes = 1000;
    epicsTimeGetCurrent(&startTime);
    for(int i=0; i<ntimes; i++) {
        for(int j=0;j<numNodes;j++) basicList->addTail(basics[j]->node);
        BasicListNode *basicNode = basicList->removeHead();
        while(basicNode!=0) basicNode = basicList->removeHead();
    }
    epicsTimeGetCurrent(&endTime);
    double diff = epicsTimeDiffInSeconds(&endTime,&startTime);
    diff *= 1000.0;
    printf("diff %f milliSeconds\n",diff);
    diff = diff/1000.0; // convert from milliseconds to seconds
    diff = diff/ntimes; // seconds per outer loop
    diff = diff*1e6; // converty to microseconds
    printf("time per iteration %f microseconds\n",diff);
    diff = diff/(numNodes*2); // convert to per addTail/removeHead
    printf("time per addTail/removeHead %f microseconds\n",diff);
    assert(basicList->isEmpty());
    delete basicList;
    for(int i=0; i<numNodes; i++) delete basics[i];
}

static void testTimeLocked() {
    epicsTimeStamp  startTime;
    epicsTimeStamp  endTime;
    Mutex *mutex = new Mutex();
    int numNodes = 1000;

    LinkedList<Basic> *basicList = new BasicList();
    Basic *basics[numNodes];
    for(int i=0; i<numNodes; i++) {
        basics[i] = new Basic(i);
    }
    printf("\nTime test locked\n");
    int ntimes = 1000;
    epicsTimeGetCurrent(&startTime);
    for(int i=0; i<ntimes; i++) {
        for(int j=0;j<numNodes;j++) {
            Lock xx(mutex);
            basicList->addTail(basics[j]->node);
        }
        BasicListNode *basicNode = 0;
        {
            Lock xx(mutex);
            basicNode = basicList->removeHead();
        }
        while(basicNode!=0) {
            Lock xx(mutex);
           basicNode = basicList->removeHead();
        }
    }
    epicsTimeGetCurrent(&endTime);
    double diff = epicsTimeDiffInSeconds(&endTime,&startTime);
    diff *= 1000.0;
    printf("diff %f milliSeconds\n",diff);
    diff = diff/1000.0; // convert from milliseconds to seconds
    diff = diff/ntimes; // seconds per outer loop
    diff = diff*1e6; // converty to microseconds
    printf("time per iteration %f microseconds\n",diff);
    diff = diff/(numNodes*2); // convert to per addTail/removeHead
    printf("time per addTail/removeHead %f microseconds\n",diff);
    assert(basicList->isEmpty());
    delete basicList;
    for(int i=0; i<numNodes; i++) delete basics[i];
}

typedef std::list<Basic *> stdList;
static void testArrayListTime() {
    epicsTimeStamp  startTime;
    epicsTimeStamp  endTime;
    int numNodes = 1000;

    stdList basicList;
    Basic *basics[numNodes];
    for(int i=0; i<numNodes; i++) {
        basics[i] = new Basic(i);
    }
    printf("\nTime ArrayList test\n");
    int ntimes = 1000;
    epicsTimeGetCurrent(&startTime);
    for(int i=0; i<ntimes; i++) {
        for(int j=0;j<numNodes;j++) basicList.push_back(basics[j]);
        while(basicList.size()>0) {
            basicList.begin();
            basicList.pop_front();
        }
    }
    epicsTimeGetCurrent(&endTime);
    double diff = epicsTimeDiffInSeconds(&endTime,&startTime);
    diff *= 1000.0;
    printf("diff %f milliSeconds\n",diff);
    diff = diff/1000.0; // convert from milliseconds to seconds
    diff = diff/ntimes; // seconds per outer loop
    diff = diff*1e6; // converty to microseconds
    printf("time per iteration %f microseconds\n",diff);
    diff = diff/(numNodes*2); // convert to per addTail/removeHead
    printf("time per addTail/removeHead %f microseconds\n",diff);
    for(int i=0; i<numNodes; i++) delete basics[i];
}

static void testArrayListTimeLocked() {
    epicsTimeStamp  startTime;
    epicsTimeStamp  endTime;
    int numNodes = 1000;
    Mutex *mutex = new Mutex();

    stdList basicList;
    Basic *basics[numNodes];
    for(int i=0; i<numNodes; i++) {
        basics[i] = new Basic(i);
    }
    printf("\nTime ArrayList test locked\n");
    int ntimes = 1000;
    epicsTimeGetCurrent(&startTime);
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
    epicsTimeGetCurrent(&endTime);
    double diff = epicsTimeDiffInSeconds(&endTime,&startTime);
    diff *= 1000.0;
    printf("diff %f milliSeconds\n",diff);
    diff = diff/1000.0; // convert from milliseconds to seconds
    diff = diff/ntimes; // seconds per outer loop
    diff = diff*1e6; // converty to microseconds
    printf("time per iteration %f microseconds\n",diff);
    diff = diff/(numNodes*2); // convert to per addTail/removeHead
    printf("time per addTail/removeHead %f microseconds\n",diff);
    for(int i=0; i<numNodes; i++) delete basics[i];
}

int main(int argc, char *argv[]) {
     char *fileName = 0;
    if(argc>1) fileName = argv[1];
    FILE * fd = stdout;
    if(fileName!=0 && fileName[0]!=0) {
        fd = fopen(fileName,"w+");
    }
    testBasic(fd);
    testQueue(fd);
    testStack(fd);
    testList(fd);
    testRandomInsertRemove(fd);
    testOrderedQueue(fd);
    testTime();
    testTimeLocked();
    testArrayListTime();
    testArrayListTimeLocked();
    int totalConstructList = LinkedListVoid::getTotalConstruct();
    int totalDestructList = LinkedListVoid::getTotalDestruct();
    int totalConstructListNode = LinkedListVoidNode::getTotalConstruct();
    int totalDestructListNode = LinkedListVoidNode::getTotalDestruct();
    fprintf(fd,"totalConstructList %d totalDestructList %d",
        totalConstructList,totalDestructList);
    fprintf(fd," totalConstructListNode %d totalDestructListNode %d\n",
        totalConstructListNode,totalDestructListNode);
    assert(totalConstructList==totalDestructList);
    assert(totalConstructListNode==totalDestructListNode);
    return (0);
}
 
