/* requester.h */
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
