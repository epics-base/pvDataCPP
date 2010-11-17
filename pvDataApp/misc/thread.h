/* thread.h */
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


class ThreadReady {
public:
    virtual void ready() = 0;
};

class RunnableReady {
public:
    virtual void run(ThreadReady *threadReady) = 0;
};

class Thread;

class Thread :  private NoDefaultMethods {
public:
    Thread(String name,ThreadPriority priority,RunnableReady *runnableReady);
    ~Thread();
    static ConstructDestructCallback *getConstructDestructCallback();
    void start();
    String getName();
    ThreadPriority getPriority();
    static void showThreads(StringBuilder buf);
    static void sleep(double seconds);
private:
    class Runnable *pImpl;
    friend class Runnable;
};

}}
#endif  /* THREAD_H */
