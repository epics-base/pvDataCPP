/* thread.h */
/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */
/**
 *  @author mrk
 */
#ifndef THREAD_H
#define THREAD_H

#include <memory>
#include <sstream>

#if __cplusplus>=201103L
#include <functional>
#endif

#include <epicsThread.h>
#include <shareLib.h>

#include <pv/noDefaultMethods.h>
#include <pv/pvType.h>


namespace epics { namespace pvData {

enum ThreadPriority {
    lowestPriority  =epicsThreadPriorityLow,
    lowerPriority   =epicsThreadPriorityLow + 15,
    lowPriority     =epicsThreadPriorityMedium - 15,
    middlePriority  =epicsThreadPriorityMedium,
    highPriority    =epicsThreadPriorityMedium + 15,
    higherPriority  =epicsThreadPriorityHigh - 15,
    highestPriority =epicsThreadPriorityHigh
};

class Thread;
typedef std::tr1::shared_ptr<Thread> ThreadPtr;
typedef std::tr1::shared_ptr<epicsThread> EpicsThreadPtr;

typedef epicsThreadRunable Runnable;

//! Helper for those cases where a class should have more than one runnable
template<typename C>
class epicsShareClass RunnableMethod : public Runnable, private NoDefaultMethods
{
    typedef void (C::*meth_t)();
    C *inst;
    meth_t meth;

    virtual void run()
    {
        (inst->*meth)();
    }
public:
    RunnableMethod(C* inst, void (C::*meth)())
        :inst(inst), meth(meth)
    {}
};

namespace detail {
struct FuncRunner : public epicsThreadRunable
{
    typedef void (*fn_t)(void*);
    fn_t fn;
    void *arg;
    FuncRunner(fn_t f, void *a) :fn(f), arg(a) {}
    virtual ~FuncRunner(){}
    virtual void run()
    {
        (*fn)(arg);
    }
};
template<typename C>
struct MethRunner : public epicsThreadRunable
{
    typedef void(C::*fn_t)();
    fn_t fn;
    C* inst;
    MethRunner(C* i, fn_t f) :fn(f), inst(i) {}
    virtual ~MethRunner() {}
    virtual void run()
    {
        (inst->*fn)();
    }
};
#if __cplusplus>=201103L
struct BindRunner : public epicsThreadRunable
{
    typedef std::function<void()> fn_t;
    fn_t fn;
    BindRunner(const fn_t f) : fn(f) {}
    virtual ~BindRunner() {}
    virtual void run()
    {
        fn();
    }
};
#endif
} // namespace detail

/**
 * @brief C++ wrapper for epicsThread from EPICS base.
 *
 */
class epicsShareClass Thread : public epicsThread, private NoDefaultMethods {
public:
    /** @brief Holds all the configuration necessary to launch a @class Thread
     *
     * The defaults may be used except for the runnable, which must be given
     * either in the constructor, or the @method run() method.
     *
     * @note Instances of class Config may not be reused.
     *
     * Defaults:
     *  name: ""
     *  priority: epicsThreadPriorityLow (aka epics::pvData::lowestPriority)
     *  stack size: epicsThreadStackSmall
     *  auto start: true
     *  runner: nil (must be set explictly)
     *
     @code
        stuct bar { void meth(); ... } X;
        // with a static thread name
        Thread foo(Thread::Config(&X, &bar::meth)
                    .name("example")
                    .prio(epicsThreadPriorityHigh));

        // with a constructed thread name
        Thread foo(Thread::Config(&X, &bar::meth)
                    .prio(epicsThreadPriorityHigh)
                    <<"example"<<1);
     @endcode
     */
    class epicsShareClass Config
    {
        unsigned int p_prio, p_stack;
        std::ostringstream p_strm;
        bool p_autostart;
        Runnable *p_runner;
#if __cplusplus>=201103L
        typedef std::unique_ptr<Runnable> p_owned_runner_t;
#else
        typedef std::auto_ptr<Runnable> p_owned_runner_t;
#endif
        p_owned_runner_t p_owned_runner;
        friend class Thread;
        Runnable& x_getrunner()
        {
            if(!this->p_runner)
                throw std::logic_error("Thread::Config missing run()");
            return *this->p_runner;
        }
        void x_setdefault()
        {
            this->p_prio = epicsThreadPriorityLow;
            this->p_autostart = true;
            this->p_runner = NULL;
            (*this).stack(epicsThreadStackSmall);
        }

    public:
        Config() {this->x_setdefault();}
        Config(Runnable *r) {this->x_setdefault();this->run(r);}
        Config(void(*fn)(void*), void *ptr) {this->x_setdefault();this->run(fn, ptr);}
        template<typename C>
        Config(C* inst, void(C::*meth)()) {this->x_setdefault();this->run(inst, meth);}
#if __cplusplus>=201103L
        Config(const std::function<void()>& fn) {this->x_setdefault();this->run(fn);}
#endif

        inline Config& name(const std::string& n)
        { this->p_strm.str(n); return *this; }
        inline Config& prio(unsigned int p)
        { this->p_prio = p; return *this; }
        inline Config& stack(epicsThreadStackSizeClass s)
        { this->p_stack = epicsThreadGetStackSize(s); return *this; }
        inline Config& autostart(bool a)
        { this->p_autostart = a; return *this; }

        //! Thread will execute Runnable::run()
        Config& run(Runnable* r)
        { this->p_runner = r; return *this; }
        //! Thread will execute (*fn)(ptr)
        Config& run(void(*fn)(void*), void *ptr)
        {
            this->p_owned_runner.reset(new detail::FuncRunner(fn, ptr));
            this->p_runner = this->p_owned_runner.get();
            return *this;
        }
        //! Thread will execute (inst->*meth)()
        template<typename C>
        Config& run(C* inst, void(C::*meth)())
        {
            this->p_owned_runner.reset(new detail::MethRunner<C>(inst, meth));
            this->p_runner = this->p_owned_runner.get();
            return *this;
        }
#if __cplusplus>=201103L
        Config& run(const std::function<void()>& fn)
        {
            this->p_owned_runner.reset(new detail::BindRunner(fn));
            this->p_runner = this->p_owned_runner.get();
            return *this;
        }
#endif

        //! Append to thread name string.  Argument must be understood by std::ostream::operator<<
        template<typename T>
        Config& operator<<(T x) { this->p_strm<<x; return *this; }
    };

    /**
     * 
     * Constructor
     * @param name thread name.
     * @param priority priority is one of:
     @code
     enum ThreadPriority {
        lowestPriority, lowerPriority, lowPriority,
        middlePriority,
        highPriority, higherPriority, highestPriority
     };
     @endcode
     * @param runnable this is a c function
     * @param stkcls stack size as specified by epicsThreadStackSizeClass
     */
    Thread(std::string name,
           ThreadPriority priority,
           Runnable *runnable,
           epicsThreadStackSizeClass stkcls=epicsThreadStackSmall)
        :epicsThread(*runnable,
                     name.c_str(),
                     epicsThreadGetStackSize(stkcls),
                     priority)
    {
        this->start();
    }

    /**
     * 
     * Constructor
     * @param runnable this is a c function
     * @name thread name.
     * @param stkcls stack size as specified by epicsThreadStackSizeClass
     * @param priority priority is one of:
     @code
     enum ThreadPriority {
        lowestPriority, lowerPriority, lowPriority,
        middlePriority,
        highPriority, higherPriority, highestPriority
     };
     @endcode
     */
    Thread(Runnable &runnable,
           std::string name,
           unsigned int stksize,
           unsigned int priority=lowestPriority)
        :epicsThread(runnable,
                     name.c_str(),
                     stksize,
                     priority)
    {
        this->start();
    }

    //! @brief Create a new thread using the given @class Config
    //! @throws std::logic_error for improper class Config (ie. missing runner)
    Thread(Config& c)
        :epicsThread(c.x_getrunner(), c.p_strm.str().c_str(),
                     c.p_stack, c.p_prio)
    {
#if __cplusplus>=201103L
        p_owned = std::move(c.p_owned_runner);
#else
        p_owned = c.p_owned_runner;
#endif
        if(c.p_autostart)
            this->start();
    }

    /**
     * Destructor
     */
    ~Thread()
    {
        this->exitWait();
    }

    Config::p_owned_runner_t p_owned;
};


}}
#endif  /* THREAD_H */
