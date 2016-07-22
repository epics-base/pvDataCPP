/* timeFunction.h */
/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */
/**
 *  @author mrk
 */
#ifndef TIMEFUNCTION_H
#define TIMEFUNCTION_H

#include <pv/sharedPtr.h>

#include <shareLib.h>

namespace epics { namespace pvData { 

class TimeFunctionRequester;
class TimeFunction;
typedef std::tr1::shared_ptr<TimeFunctionRequester> TimeFunctionRequesterPtr;
typedef std::tr1::shared_ptr<TimeFunction> TimeFunctionPtr;

/** 
 * @brief Class that must be implemented by timeFunction requester.
 *
 */
class epicsShareClass TimeFunctionRequester {
public:
    POINTER_DEFINITIONS(TimeFunctionRequester);
    /** 
     * Destructor
     */
    virtual ~TimeFunctionRequester(){}
    /**
     *  function to be timed.
     *  It will get called multiple times.
     */
    virtual void function() = 0;
};


/** 
 * @brief Class for measuring time it takes to execute a function.
 *
 */
class epicsShareClass TimeFunction {
public:
    POINTER_DEFINITIONS(TimeFunction);
    /**
     * Constructor
     * @param requester The class that has a function method.
     */
    TimeFunction(TimeFunctionRequesterPtr const & requester);
    /**
     * Destructor
     */
    ~TimeFunction();
    /**
     * Time the function.
     * @return the time in seconds to execute the function.
     * Note that the function may be called many times.
     */
    double timeCall();
private:
    TimeFunctionRequesterPtr requester;
};
  

}}
#endif  /* TIMEFUNCTION_H */
