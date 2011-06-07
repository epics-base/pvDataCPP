/* requester.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef REQUESTER_H
#define REQUESTER_H
#include <string>
#include <pv/pvType.h>
#include <pv/sharedPtr.h>

namespace epics { namespace pvData { 

class Requester;

enum MessageType {
   infoMessage,warningMessage,errorMessage,fatalErrorMessage
};

extern StringArray messageTypeName;

class Requester {
public:
    typedef std::tr1::shared_ptr<Requester> shared_pointer;
    typedef std::tr1::shared_ptr<const Requester> const_shared_pointer;

    virtual ~Requester(){}
    virtual String getRequesterName() = 0;
    virtual void message(String message,MessageType messageType) = 0;
};

}}
#endif  /* REQUESTER_H */
