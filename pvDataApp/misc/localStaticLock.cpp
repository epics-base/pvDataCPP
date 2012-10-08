/* localStaticLock.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mse
 */

#include <pv/localStaticLock.h>

static int nifty_counter;
static epics::pvData::Mutex* g_localStaticInitMutex;

epics::pvData::Mutex& getLocalStaticInitMutex()
{
    return *g_localStaticInitMutex;
}


// The counter is initialized at load-time, i.e., before any of the static objects are initialized.
MutexInitializer::MutexInitializer ()
{
    if (0 == nifty_counter++)
    {
        // Initialize static members.
        g_localStaticInitMutex = new epics::pvData::Mutex();
    }
}

MutexInitializer::~MutexInitializer ()
{
    if (0 == --nifty_counter)
    {
        // Clean-up.
        delete g_localStaticInitMutex;
    }
}
