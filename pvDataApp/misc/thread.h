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

class epicsShareClass Thread : public epicsThread, private NoDefaultMethods {
public:

    Thread(String name,
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

    Thread(Runnable &runnable,
           String name,
           unsigned int stksize,
           unsigned int priority=lowestPriority)
        :epicsThread(runnable,
                     name.c_str(),
                     stksize,
                     priority)
    {
        this->start();
    }

    ~Thread()
    {
        this->exitWait();
    }
};


}}
#endif  /* THREAD_H */
