/* timeFunction.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef TIMEFUNCTION_H
#define TIMEFUNCTION_H
#include "noDefaultMethods.h"
#include "pvType.h"

namespace epics { namespace pvData { 

class TimeFunctionRequester {
public:
    virtual ~TimeFunctionRequester(){}
    virtual void function() = 0;
};

class TimeFunction : private NoDefaultMethods {
public:
    TimeFunction(TimeFunctionRequester *requester);
    ~TimeFunction();
    double timeCall();
private:
    TimeFunctionRequester *requester;
};
  

}}
#endif  /* TIMEFUNCTION_H */
