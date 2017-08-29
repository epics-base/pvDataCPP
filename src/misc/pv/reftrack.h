/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */
#ifndef REFTRACK_H
#define REFTRACK_H

#ifdef __cplusplus

#include <map>
#include <string>
#include <ostream>

#include <stdlib.h>

#include <epicsVersion.h>

#ifndef VERSION_INT
#  define VERSION_INT(V,R,M,P) ( ((V)<<24) | ((R)<<16) | ((M)<<8) | (P))
#endif

#ifndef EPICS_VERSION_INT
#  define EPICS_VERSION_INT VERSION_INT(EPICS_VERSION, EPICS_REVISION, EPICS_MODIFICATION, EPICS_PATCH_LEVEL)
#endif

#if EPICS_VERSION_INT>=VERSION_INT(3,15,1,0)
#  include <epicsAtomic.h>
#  define REFTRACK_USE_ATOMIC
#endif

#ifdef REFTRACK_USE_ATOMIC
#  define REFTRACE_INCREMENT(counter) ::epics::atomic::increment(counter)
#  define REFTRACE_DECREMENT(counter) ::epics::atomic::decrement(counter)
#else
#  define REFTRACE_INCREMENT(counter) do{}while(0)
#  define REFTRACE_DECREMENT(counter) do{}while(0)
#endif

#include <shareLib.h>

namespace epics {

//! Register new global reference counter
epicsShareFunc
void registerRefCounter(const char *name, const size_t* counter);

//! Remove registration of global reference counter (if dynamically allocated)
epicsShareFunc
void unregisterRefCounter(const char *name, const size_t* counter);

//! Fetch current value of single reference counter
epicsShareFunc
size_t readRefCounter(const char *name);

//! Represent a snapshot of many reference counters
class epicsShareClass RefSnapshot
{
public:
    //! A single count
    struct Count {
        size_t current;
        ssize_t delta; //!< current - previous
        Count() :current(0u), delta(0) {}
        explicit Count(size_t c, ssize_t d) :current(c), delta(d) {}
        bool operator==(const Count& o) const
        { return current==o.current && delta==o.delta; }
    };

private:
    static const Count zero;
    typedef std::map<std::string, Count> cnt_map_t;
    cnt_map_t counts;
public:
    typedef cnt_map_t::const_iterator iterator;
    typedef cnt_map_t::const_iterator const_iterator;

    /** Fetch values of all reference counters.
     *
     * This involves many atomic reads, not a single operation.
     */
    void update();

    const Count& operator[](const std::string& name) const
    {
        cnt_map_t::const_iterator it(counts.find(name));
        return it==counts.end() ? zero : it->second;
    }

    iterator begin() const { return counts.begin(); }
    iterator end() const { return counts.end(); }
    size_t size() const { return counts.size(); }

    inline void swap(RefSnapshot& o)
    {
        counts.swap(o.counts);
    }

    friend RefSnapshot operator-(const RefSnapshot& lhs, const RefSnapshot& rhs);
};

/** Compute the difference lhs - rhs
 *
 * Returned RefSnapshot has Count::current=lhs.current
 * and Count::delta= lhs.current - rhs.current
 */
epicsShareFunc
RefSnapshot operator-(const RefSnapshot& lhs, const RefSnapshot& rhs);

//! Print all counters with a non-zero delta
epicsShareFunc
std::ostream& operator<<(std::ostream& strm, const RefSnapshot& snap);

//! Helper to run a thread which periodically prints (via show() )
//! global reference counter deltas.
class epicsShareClass RefMonitor
{
    struct Impl;
    Impl *impl;
public:
    RefMonitor();
    ~RefMonitor();

    void start(double period=10.0);
    void stop();
    bool running() const;

    //! call show() with current snapshot
    void current();
protected:
    //! Default prints to stderr
    //! @param complete when false show only non-zero delta, when true show non-zero count or delta
    virtual void show(const RefSnapshot& snap, bool complete=false);
};

} // namespace epics

extern "C" {
#endif /* __cplusplus */

/** Fetch and print current snapshot
 * @return NULL or a char* which must be free()'d
 */
char* epicsRefSnapshotCurrent();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // REFTRACK_H
