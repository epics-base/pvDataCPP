/* requester.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <string>
#include <pv/requester.h>
namespace epics { namespace pvData { 

const size_t messageTypeCount = 4; 

StringArray messageTypeName(messageTypeCount);
void Requester::init()
{
    messageTypeName[0] = "info";
    messageTypeName[1] = "warning";
    messageTypeName[2] = "error";
    messageTypeName[3] = "fatalError";
}

}}
