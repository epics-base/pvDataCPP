/* localStaticLock.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mse
 */
#ifndef LOCALSTATICLOCK_H
#define LOCALSTATICLOCK_H

#include <pv/lock.h>

#include <sharelib.h>

epicsShareExtern epics::pvData::Mutex& getLocalStaticInitMutex();

#if defined(__GNUC__) && __GNUC__ >= 4
// noop
#define LOCAL_STATIC_LOCK
#else
#define LOCAL_STATIC_LOCK epics::pvData::Lock localStaticInitMutexLock(getLocalStaticInitMutex());
#endif

static class epicsShareClass MutexInitializer {
  public:
    MutexInitializer ();
    ~MutexInitializer ();
} localStaticMutexInitializer; // Note object here in the header.


#endif  /* LOCALSTATICLOCK_H */
