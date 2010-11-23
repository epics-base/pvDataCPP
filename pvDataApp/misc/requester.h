/* requester.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <string>
#ifndef REQUESTER_H
#define REQUESTER_H
#include "pvIntrospect.h"
namespace epics { namespace pvData { 

    class Requester;
    
    enum MessageType {
       infoMessage,warningMessage,errorMessage,fatalErrorMessage
    };

    static std::string messageTypeName[] = {
       "info","warning","error","fatalError"
    };
    
    class Requester {
    public:
        virtual String getRequesterName() = 0;
        virtual void message(String message,MessageType messageType) = 0;
    };

}}
#endif  /* REQUESTER_H */
