/* lock.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef LOCK_H
#define LOCK_H
#include <stdexcept>
#include <epicsMutex.h>
#include <pv/noDefaultMethods.h>
/* This is based on item 14 of 
 * Effective C++, Third Edition, Scott Meyers
 */

namespace epics { namespace pvData { 

typedef epicsMutex Mutex;

class Lock : private NoDefaultMethods {
public:
    explicit Lock(Mutex &m)
    : mutexPtr(m), locked(true)
    { mutexPtr.lock();}
    ~Lock(){unlock();}
    void lock()
    {
        if(!locked) mutexPtr.lock();
        locked = true;
    }
    void unlock()
    {
        if(locked)
            mutexPtr.unlock();
        locked=false;
    }
    bool ownsLock() const{return locked;}
private:
    Mutex &mutexPtr;
    bool locked;
};


}}
#endif  /* LOCK_H */
