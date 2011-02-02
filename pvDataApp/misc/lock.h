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
#include "noDefaultMethods.h"
/* This is based on item 14 of 
 * Effective C++, Third Edition, Scott Meyers
 */

namespace epics { namespace pvData { 

typedef epicsMutexId native_handle_type;

/** @brief Acquires and holds a mutex until destructed
 *
 * Partial implementation of boost::lock_guard<>
 */
template<typename Lockable>
class lock_guard : private NoDefaultMethods {
public:
    typedef Lockable mutex_type;
    explicit lock_guard(Lockable &m)
        : mutex(m)
    {mutex.lock();}
    explicit lock_guard(Lockable *pm)
        : mutex(*pm)
    {mutex.lock();}
    ~lock_guard(){mutex.unlock();}
private:
    Lockable& mutex;
};

/* Lock action tags.
 * Used to select construction behaviour of locks
 */
struct defer_lock_t{};
//struct adopt_lock_t{};

const defer_lock_t defer_lock={};
//const adopt_lock_t adopt_lock={};

/** @brief Acquires a mutex.  Always releases destructed
 *
 * May release and re-acquire.
 * Partial implementation of boost::unique_lock<>
 */
template<typename Lockable>
class unique_lock : private NoDefaultMethods {
public:
    typedef Lockable mutex_type;
    explicit unique_lock(Lockable &m)
        : mutexPtr(&m), locked(true)
    {mutexPtr->lock();}
    unique_lock(Lockable &m, defer_lock_t)
        : mutexPtr(m), locked(false)
    {}
    ~unique_lock(){unlock();}
    void swap(unique_lock& O)
    {
        Lockable *t=O.mutexPtr;
        bool tl=O.locked;
        O.mutexPtr=mutexPtr;
        O.locked=locked;
        mutexPtr=t;
        locked=tl;
    }
    void lock()
    {
        if(!locked)
            mutexPtr->lock();
        locked=true;
    }
    void unlock()
    {
        if(locked)
            mutexPtr->unlock();
        locked=false;
    }
    bool owns_lock() const{return locked;}

    Lockable* mutex() const{return mutex;}
    Lockable* release(){locked=false; return mutex;}

private:
    Lockable *mutexPtr;
    bool locked;
};

class Mutex  {
public:
    typedef unique_lock<Mutex> scoped_lock;
    Mutex() : id(epicsMutexCreate())
    {if(!id) throw std::bad_alloc();}
    ~Mutex() { epicsMutexDestroy(id) ;}
    void lock(){
        if(epicsMutexLock(id)!=epicsMutexLockOK)
            throw std::logic_error("Failed to acquire Mutex");
    }
    void unlock(){epicsMutexUnlock(id);}

    native_handle_type native_handle(){return id;}
private:
    epicsMutexId id;
};

typedef lock_guard<Mutex> Lock;


}}
#endif  /* LOCK_H */
