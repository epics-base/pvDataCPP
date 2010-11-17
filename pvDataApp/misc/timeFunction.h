/* timeFunction.h */
#ifndef TIMEFUNCTION_H
#define TIMEFUNCTION_H
#include "noDefaultMethods.h"
#include "pvType.h"

namespace epics { namespace pvData { 

class TimeFunctionRequester {
public:
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
