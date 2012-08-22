/* timeFunction.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#ifndef TIMEFUNCTION_H
#define TIMEFUNCTION_H
#include <pv/sharedPtr.h>

namespace epics { namespace pvData { 

class TimeFunctionRequester;
class TimeFunction;
typedef std::tr1::shared_ptr<TimeFunctionRequester> TimeFunctionRequesterPtr;
typedef std::tr1::shared_ptr<TimeFunction> TimeFunctionPtr;

class TimeFunctionRequester {
public:
    POINTER_DEFINITIONS(TimeFunctionRequester);
    virtual ~TimeFunctionRequester(){}
    virtual void function() = 0;
};


class TimeFunction {
public:
    POINTER_DEFINITIONS(TimeFunction);
    TimeFunction(TimeFunctionRequesterPtr const & requester);
    ~TimeFunction();
    double timeCall();
private:
    TimeFunctionRequesterPtr requester;
};
  

}}
#endif  /* TIMEFUNCTION_H */
