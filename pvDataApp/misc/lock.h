/* lock.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
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
        Mutex() : id(epicsMutexMustCreate()){}
        ~Mutex() { epicsMutexDestroy(id) ;};
        void lock(){epicsMutexMustLock(id);}\
        void unlock(){epicsMutexUnlock(id);}
    private:
        epicsMutexId id;
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
