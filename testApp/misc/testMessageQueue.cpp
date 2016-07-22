/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
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

#include <epicsUnitTest.h>
#include <testMain.h>

#include <pv/lock.h>
#include <pv/timeStamp.h>
#include <pv/requester.h>
#include <pv/messageQueue.h>
#include <pv/event.h>


using namespace epics::pvData;

static void testBasic() {
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
    testOk1(result);
    result = queue->put(messages[0],infoMessage,true);
    testOk1(result);
    result = queue->put(messages[1],infoMessage,true);
    testOk1(result);
    result = queue->put(messages[2],warningMessage,true);
    testOk1(result);
    testOk1(queue->isFull());
    result = queue->put(messages[3],infoMessage,true);
    testOk1(result==true);
    messageNode = queue->get();
    testOk1(messageNode.get()!=NULL);
    printf("message %s messageType %s\n",
        messageNode->getMessage().c_str(),
        getMessageTypeName(messageNode->getMessageType()).c_str());
    testOk1(messageNode->getMessage().compare(messages[0])==0);
    queue->release();
    messageNode = queue->get();
    testOk1(messageNode.get()!=NULL);
    testOk1(messageNode->getMessage().compare(messages[1])==0);
    queue->release();
    messageNode = queue->get();
    testOk1(messageNode.get()!=NULL);
    printf("message %s messageType %s\n",
        messageNode->getMessage().c_str(),
        getMessageTypeName(messageNode->getMessageType()).c_str());
    testOk1(messageNode->getMessage().compare(messages[3])==0);
    queue->release();
    result = queue->isEmpty();
    testOk1(result);
    printf("PASSED\n");
}

MAIN(testMessageQueue)
{
    testPlan(13);
    testDiag("Tests messageQueue");
    testBasic();
    return testDone();
}
 
