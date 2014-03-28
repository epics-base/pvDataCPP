/* requester.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#ifndef REQUESTER_H
#define REQUESTER_H
#include <string>

#include <pv/pvType.h>
#include <pv/sharedPtr.h>

#include <shareLib.h>

namespace epics { namespace pvData { 

class Requester;
typedef std::tr1::shared_ptr<Requester> RequesterPtr;

enum MessageType {
   infoMessage,warningMessage,errorMessage,fatalErrorMessage
};
#define MESSAGE_TYPE_COUNT 4

epicsShareExtern String getMessageTypeName(MessageType messageType);

class epicsShareClass Requester {
public:
    POINTER_DEFINITIONS(Requester);
    virtual ~Requester(){}
    virtual String getRequesterName() = 0;
    virtual void message(String const & message,MessageType messageType) = 0;
};

}}
#endif  /* REQUESTER_H */
