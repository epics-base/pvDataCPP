/* thread.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#ifndef THREAD_H
#define THREAD_H

#include <memory>

#ifdef epicsExportSharedSymbols
#define threadepicsExportSharedSymbols
#undef epicsExportSharedSymbols
#endif

#include <epicsThread.h>

#ifdef threadepicsExportSharedSymbols
#define epicsExportSharedSymbols
#undef threadepicsExportSharedSymbols
#endif

#include <pv/noDefaultMethods.h>
#include <pv/pvType.h>

#include <shareLib.h>

namespace epics { namespace pvData {

enum ThreadPriority {
    lowestPriority  =epicsThreadPriorityLow,
    lowerPriority   =epicsThreadPriorityLow + 15,
    lowPriority     =epicsThreadPriorityMedium - 15,
    middlePriority  =epicsThreadPriorityMedium,
    highPriority    =epicsThreadPriorityMedium + 15,
    higherPriority  =epicsThreadPriorityHigh - 15,
    highestPriority =epicsThreadPriorityHigh
};

class Thread;
typedef std::tr1::shared_ptr<Thread> ThreadPtr;
typedef std::tr1::shared_ptr<epicsThread> EpicsThreadPtr;

typedef epicsThreadRunable Runnable;

/**
 * @brief C++ wrapper for epicsThread from EPICS base.
 *
 */
class epicsShareClass Thread : public epicsThread, private NoDefaultMethods {
public:

    /**
     * 
     * Constructor
     * @param name thread name.
     * @param priority priority is one of:
     @code
     enum ThreadPriority {
        lowestPriority, lowerPriority, lowPriority,
        middlePriority,
        highPriority, higherPriority, highestPriority
     };
     @endcode
     * @param runnable this is a c function
     * @param stkcls stack size as specified by epicsThreadStackSizeClass
     */
    Thread(std::string name,
           ThreadPriority priority,
           Runnable *runnable,
           epicsThreadStackSizeClass stkcls=epicsThreadStackSmall)
        :epicsThread(*runnable,
                     name.c_str(),
                     epicsThreadGetStackSize(stkcls),
                     priority)
    {
        this->start();
    }

    /**
     * 
     * Constructor
     * @param runnable this is a c function
     * @name thread name.
     * @param stkcls stack size as specified by epicsThreadStackSizeClass
     * @param priority priority is one of:
     @code
     enum ThreadPriority {
        lowestPriority, lowerPriority, lowPriority,
        middlePriority,
        highPriority, higherPriority, highestPriority
     };
     @endcode
     */
    Thread(Runnable &runnable,
           std::string name,
           unsigned int stksize,
           unsigned int priority=lowestPriority)
        :epicsThread(runnable,
                     name.c_str(),
                     stksize,
                     priority)
    {
        this->start();
    }

    /**
     * Destructor
     */
    ~Thread()
    {
        this->exitWait();
    }
};


}}
#endif  /* THREAD_H */
