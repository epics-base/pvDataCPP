/* lock.h */
#ifndef LOCK_H
#define LOCK_H
#include <epicsMutex.h>
#include "noDefaultMethods.h"
/* This is based on item 14 of 
 * Effective C++, Third Edition, Scott Meyers
 */

namespace epics { namespace pvData { 

    class Mutex  {
    public:
        Mutex() : lockPtr(new epicsMutex()){}
        ~Mutex() { delete lockPtr;};
        void lock(){lockPtr->lock();}\
        void unlock(){lockPtr->unlock();}
    private:
        epicsMutex *lockPtr;
    };

    class Lock : private NoDefaultMethods {
    public:
        explicit Lock(Mutex *pm)
        : mutexPtr(pm)
        {mutexPtr->lock();}
        ~Lock(){mutexPtr->unlock();}
    private:
        Mutex *mutexPtr;
    };
}}
#endif  /* LOCK_H */
