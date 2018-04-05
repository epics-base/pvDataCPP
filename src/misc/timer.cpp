/* timer.cpp */
/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */
/**
 *  @author mrk
 */
 
#if defined(_WIN32) && !defined(NOMINMAX)
#define NOMINMAX
#endif

#include <stdexcept>
#include <string>
#include <iostream>

#include <epicsThread.h>
#include <epicsGuard.h>

#define epicsExportSharedSymbols
#include <pv/timer.h>

using std::string;

namespace epics { namespace pvData { 

TimerCallback::TimerCallback()
: period(0.0),
  onList(false)
{
}

Timer::Timer(string threadName,ThreadPriority priority)
: waitForWork(false),
  alive(true),
  thread(threadName,priority,this)
{}

struct TimerCallback::IncreasingTime {
    bool operator()(const TimerCallbackPtr& lhs, const TimerCallbackPtr& rhs) {
        assert(lhs && rhs);
        return lhs->timeToRun < rhs->timeToRun;
    }
};

// call with mutex held
void Timer::addElement(TimerCallbackPtr const & timerCallback)
{
    assert(!timerCallback->onList);

    queue_t temp;
    temp.push_back(timerCallback);

    timerCallback->onList = true;

    // merge sorted lists.
    // for us effectively insertion sort.
    queue.merge(temp, TimerCallback::IncreasingTime());
}


bool Timer::cancel(TimerCallbackPtr const &timerCallback)
{
    Lock xx(mutex);
    if(!timerCallback->onList) return false;
    for(queue_t::iterator it(queue.begin()), end(queue.end()); it != end; ++it)
    {
        TimerCallbackPtr& cur = *it;
        if(cur.get() == timerCallback.get()) {
            queue.erase(it);
            cur->onList = false;
            // iteration now invalid
            return true;
        }
    }
    throw std::logic_error("Timer::cancel() onList==true, but not found");
}

bool Timer::isScheduled(TimerCallbackPtr const &timerCallback) const
{
    Lock xx(mutex);
    return timerCallback->onList;
}


void Timer::run()
{
    epicsGuard<epicsMutex> G(mutex);

    epicsTime now(epicsTime::getCurrent());

    while(alive) {
        double waitfor;

        if(queue.empty()) {
            // no jobs, just go to sleep
            epicsGuardRelease<epicsMutex> U(G);

            waitForWork.wait();
            now = epicsTime::getCurrent();

        } else if((waitfor = queue.front()->timeToRun - now) <= 0) {
            // execute first expired job

            TimerCallbackPtr work;
            work.swap(queue.front());
            work->onList = false;
            queue.pop_front();

            {
                epicsGuardRelease<epicsMutex> U(G);

                work->callback();
            }

            if(work->period > 0.0) {
                work->timeToRun += work->period;
                addElement(work);
            }

            // don't update 'now' until all expired jobs run

        } else {
            // wait for first un-expired
            epicsGuardRelease<epicsMutex> U(G);

            waitForWork.wait(waitfor);
            now = epicsTime::getCurrent();
        }
    }
}

Timer::~Timer() {
    close();
}

void Timer::close() {
    {
         Lock xx(mutex);
         if(!alive)
             return; // already closed
         alive = false;
    }
    waitForWork.signal();
    thread.exitWait();

    queue_t temp;
    temp.swap(queue);

    for(;!temp.empty(); temp.pop_front()) {
        TimerCallbackPtr& head = temp.front();
        head->onList = false;
        head->timerStopped();
    }
}

void Timer::scheduleAfterDelay(
    TimerCallbackPtr const &timerCallback,
    double delay)
{
    schedulePeriodic(timerCallback,delay,0.0);
}

void Timer::schedulePeriodic(
    TimerCallbackPtr const &timerCallback,
    double delay,
    double period)
{
    epicsTime now(epicsTime::getCurrent());

    bool wasempty;
    {
        Lock xx(mutex);
        if(timerCallback->onList) {
            throw std::logic_error(string("already queued"));
        }

        if(!alive) {
            timerCallback->timerStopped();
            return;
        }

        timerCallback->timeToRun = now + delay;
        timerCallback->period = period;

        wasempty = queue.empty();
        addElement(timerCallback);
    }
    if(wasempty) waitForWork.signal();
}

void Timer::dump(std::ostream& o) const
{
    Lock xx(mutex);
    if(!alive) return;
    epicsTime now(epicsTime::getCurrent());

    for(queue_t::const_iterator it(queue.begin()), end(queue.end()); it!=end; ++it) {
        const TimerCallbackPtr& nodeToCall = *it;
        o << "timeToRun " << (nodeToCall->timeToRun - now)
          << " period " << nodeToCall->period << "\n";
    }
}

std::ostream& operator<<(std::ostream& o, const Timer& timer)
{
    timer.dump(o);
    return o;
}

}}
