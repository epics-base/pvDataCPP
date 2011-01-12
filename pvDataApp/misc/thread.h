/* thread.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef THREAD_H
#define THREAD_H
#include "noDefaultMethods.h"
#include "pvType.h"

namespace epics { namespace pvData { 

enum ThreadPriority {
    lowestPriority,
    lowerPriority,
    lowPriority,
    middlePriority,
    highPriority,
    higherPriority,
    highestPriority
};
    
class ThreadPriorityFunc {
public:
    static unsigned int const * const getEpicsPriorities();
    static int getEpicsPriority(ThreadPriority threadPriority);
};

class Runnable{
public:
    virtual void run() = 0;
};

class Thread;

class Thread :  private NoDefaultMethods {
public:
    Thread(String name,ThreadPriority priority,Runnable *runnable);
    ~Thread();
    String getName();
    ThreadPriority getPriority();
    static void showThreads(StringBuilder buf);
    static void sleep(double seconds);
private:
    class ThreadPvt *pImpl;
    friend class ThreadPvt;
};

}}
#endif  /* THREAD_H */
