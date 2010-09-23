/* requester.h */
#include <string>
#ifndef REQUESTER_H
#define REQUESTER_H
#include "pvIntrospect.h"
namespace epics { namespace pvData { 
    
    enum MessageType {infoMessage,warningMessage,errorMessage,fatalErrorMessage};

    static std::string messageTypeName[] = {"info","warning","error","fatalError"};
    
    class Requester {
    public:
        virtual StringConstPtr getRequesterName() const = 0;
        virtual void message(StringConstPtr message,MessageType messageType) const = 0;
    };
}}
#endif  /* REQUESTER_H */
