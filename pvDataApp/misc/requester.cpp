/* requester.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <string>
#include <pv/requester.h>
namespace epics { namespace pvData { 

static std::string typeName[] = {
    String("info"),
    String("warning"),
    String("error"),
    String("fatalError")
};

StringArray messageTypeName = typeName;
    
}}
