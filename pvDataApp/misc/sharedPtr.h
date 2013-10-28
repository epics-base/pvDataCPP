/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
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

// where should we look?

#if defined(__GNUC__) && __GNUC__>=4 && !defined(__vxworks)
   // GCC >=4.0.0
#  define SHARED_FROM_TR1

#elif defined(_MSC_VER) && (_MSC_VER>1500 || defined(_HAS_TR1))
   // MSVC > 2008, or 2008 w/ SP1
#  define SHARED_FROM_TR1

#else
#  define SHARED_FROM_BOOST
#endif

#if defined(_MSC_VER) && (_MSC_VER>=1600)
// MSVC 2010 has it in <memory>
#  undef SHARED_FROM_BOOST
#  undef SHARED_FROM_TR1
#endif

// go and get it

#if defined(SHARED_FROM_TR1)
#  include <tr1/memory>

#elif defined(SHARED_FROM_BOOST)

#if defined(__GNUC__) && __GNUC__ < 3
#define BOOST_EXCEPTION_DISABLE
#define BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
#endif

#  include <boost/tr1/memory.hpp>

#else
   // eventually...
#  include <memory>
#endif

// cleanup

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
    typedef std::tr1::weak_ptr<const clazz> const_weak_pointer;

#endif // SHAREDPTR_H
