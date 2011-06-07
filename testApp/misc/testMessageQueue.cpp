/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/*
 * testQueue.cpp
 *
 *  Created on: 2010.12
 *      Author: Marty Kraimer
 */

#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>

#include <epicsAssert.h>
#include <epicsExit.h>

#include <pv/lock.h>
#include <pv/timeStamp.h>
#include <pv/requester.h>
#include <pv/messageQueue.h>
#include <pv/CDRMonitor.h>
#include <pv/event.h>
#include <pv/thread.h>
#include <pv/executor.h>


using namespace epics::pvData;

static void testBasic(FILE * fd,FILE *auxfd ) {
    int queueSize = 3;
    String messages[]= {
        String("1"),String("2"),String("3"),String("4"),String("5")
    };
    MessageQueue *queue = new MessageQueue(queueSize);
    bool result;
    MessageNode *messageNode;
    result = queue->isEmpty();
    assert(result);
    result = queue->put(messages[0],infoMessage,true);
    assert(result);
    result = queue->put(messages[1],infoMessage,true);
    assert(result);
    result = queue->put(messages[2],infoMessage,true);
    assert(result);
    assert(queue->isFull());
    result = queue->put(messages[3],infoMessage,true);
    assert(result==false);
    messageNode = queue->get();
    assert(messageNode!=0);
    fprintf(fd,"message %s messageType %s\n",
        messageNode->getMessage().c_str(),
        messageTypeName[messageNode->getMessageType()].c_str());
    assert(messageNode->getMessage().compare(messages[0])==0);
    queue->release();
    messageNode = queue->get();
    assert(messageNode!=0);
    assert(messageNode->getMessage().compare(messages[1])==0);
    queue->release();
    messageNode = queue->get();
    assert(messageNode!=0);
    fprintf(fd,"message %s messageType %s\n",
        messageNode->getMessage().c_str(),
        messageTypeName[messageNode->getMessageType()].c_str());
    assert(messageNode->getMessage().compare(messages[3])==0);
    queue->release();
    result = queue->isEmpty();
    delete queue;
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
    FILE *auxfd = stdout;
    if(auxFileName!=0 && auxFileName[0]!=0) {
        auxfd = fopen(auxFileName,"w+");
    }
    testBasic(fd,auxfd);
    epicsExitCallAtExits();
    CDRMonitor::get().show(fd);
    return (0);
}
 
