/* localStaticLock.h */
/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */
/**
 *  @author mse
 */
#ifndef LOCALSTATICLOCK_H
#define LOCALSTATICLOCK_H

#include <compilerDependencies.h>

#include <pv/lock.h>

#include <shareLib.h>

epicsShareExtern epics::pvData::Mutex& getLocalStaticInitMutex() EPICS_DEPRECATED;

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
