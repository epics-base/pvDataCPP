/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/*
 * testMessageQueue.cpp
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
    StringArray messages;
    messages.reserve(5);
    messages.push_back("1");
    messages.push_back("2");
    messages.push_back("3");
    messages.push_back("4");
    messages.push_back("5");
    MessageQueuePtr queue = MessageQueue::create(queueSize);
    bool result;
    MessageNodePtr messageNode;
    result = queue->isEmpty();
    assert(result);
    result = queue->put(messages[0],infoMessage,true);
    assert(result);
    result = queue->put(messages[1],infoMessage,true);
    assert(result);
    result = queue->put(messages[2],warningMessage,true);
    assert(result);
    assert(queue->isFull());
    result = queue->put(messages[3],infoMessage,true);
    assert(result==true);
    messageNode = queue->get();
    assert(messageNode.get()!=0);
    fprintf(fd,"message %s messageType %s\n",
        messageNode->getMessage().c_str(),
        getMessageTypeName(messageNode->getMessageType()).c_str());
    assert(messageNode->getMessage().compare(messages[0])==0);
    queue->release();
    messageNode = queue->get();
    assert(messageNode.get()!=0);
    assert(messageNode->getMessage().compare(messages[1])==0);
    queue->release();
    messageNode = queue->get();
    assert(messageNode.get()!=0);
    fprintf(fd,"message %s messageType %s\n",
        messageNode->getMessage().c_str(),
        getMessageTypeName(messageNode->getMessageType()).c_str());
    assert(messageNode->getMessage().compare(messages[3])==0);
    queue->release();
    result = queue->isEmpty();
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
 
