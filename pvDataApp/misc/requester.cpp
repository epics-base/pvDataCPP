/* requester.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#include <string>
#include <cstdio>
#include <pv/lock.h>
#include <pv/requester.h>
namespace epics { namespace pvData { 

const size_t messageTypeCount = 4; 
static StringArray messageTypeName(messageTypeCount);

String getMessageTypeName(MessageType messageType)
{
    static Mutex mutex;
    Lock xx(mutex);
    if(messageTypeName[0].size()==0) {
        messageTypeName[0] = "info";
        messageTypeName[1] = "warning";
        messageTypeName[2] = "error";
        messageTypeName[3] = "fatalError";
    }
    return messageTypeName[messageType];
}



}}
