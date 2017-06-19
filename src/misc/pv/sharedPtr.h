/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */
/**
 *  @author Michael Davidsaver
 */

#ifndef SHAREDPTR_H
#define SHAREDPTR_H

/*
 * Pulls in the std::tr1 namespace with the following names
 *
 * class shared_ptr
 * class weak_ptr
 * class bad_weak_ptr
 * function static_pointer_cast;
 * function dynamic_pointer_cast
 * function const_pointer_cast
 * function swap
 * function get_deleter
 * function enable_shared_from_this
 */

/* where should we look? (In decending order of preference)
 *
 * # manual (per source file) selection
 * # c++11 version of <memory>, then alias into tr1
 * # <tr1/memory>
 * # boost version of tr1/memory
 */

#if defined(SHARED_FROM_MANUAL)
// define SHARED_FROM_MANUAL if from some reason it is desirable to manually select
// which shared_ptr implementation to use
#elif __cplusplus>=201103L || (defined(_MSC_VER) && (_MSC_VER>=1600)) || (__clang__ && __APPLE__)
// c++11 or MSVC 2010
// clang on linux has tr1/memory, clang on OSX doesn't
#  define SHARED_FROM_STD

#elif defined(__GNUC__) && __GNUC__>=4 && !defined(vxWorks)
   // GCC >=4.0.0
#  define SHARED_FROM_TR1

#elif defined(_MSC_VER) && (_MSC_VER>1500 || defined(_HAS_TR1))
   // MSVC > 2008, or 2008 w/ SP1
#  define SHARED_FROM_TR1

#else
#  define SHARED_FROM_BOOST
#endif

// go and get it

#if defined(SHARED_FROM_MANUAL)
// no-op
#elif defined(SHARED_FROM_STD)

#include <memory>

namespace std {
    namespace tr1 {
        using std::shared_ptr;
        using std::weak_ptr;
        using std::static_pointer_cast;
        using std::dynamic_pointer_cast;
        using std::const_pointer_cast;
        using std::enable_shared_from_this;
        using std::bad_weak_ptr;
    }
}

#elif defined(SHARED_FROM_TR1)
#  include <tr1/memory>

#elif defined(SHARED_FROM_BOOST)

#if defined(__GNUC__) && __GNUC__ < 3
#define BOOST_EXCEPTION_DISABLE
#define BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
#endif

#  include <boost/tr1/memory.hpp>

#else
#  error No shared_ptr selection
#endif

// cleanup

#ifdef SHARED_FROM_STD
#  undef SHARED_FROM_STD
#endif

#ifdef SHARED_FROM_TR1
#  undef SHARED_FROM_TR1
#endif

#ifdef SHARED_FROM_BOOST
#  undef SHARED_FROM_BOOST
#endif

#define POINTER_DEFINITIONS(clazz) \
    typedef std::tr1::shared_ptr<clazz> shared_pointer; \
    typedef std::tr1::shared_ptr<const clazz> const_shared_pointer; \
    typedef std::tr1::weak_ptr<clazz> weak_pointer; \
    typedef std::tr1::weak_ptr<const clazz> const_weak_pointer

#endif // SHAREDPTR_H
