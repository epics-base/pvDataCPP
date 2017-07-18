/* noDefaultMethods.h */
/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */
/**
 *  @author mrk
 */
#ifndef NO_DEFAULT_METHODS_H
#define NO_DEFAULT_METHODS_H

#include <shareLib.h>

namespace epics { namespace pvData { 
/* This is based on Item 6 of 
 * Effective C++, Third Edition, Scott Meyers
 */

/**
 * @brief Base class for not allowing default methods.
 *
 * Note that copy constructor a copy methods are declared private.
 */
class NoDefaultMethods {
public:
    NoDefaultMethods() {}
private:
#if __cplusplus>=201103L
    NoDefaultMethods(const NoDefaultMethods&) = delete;
    NoDefaultMethods & operator=(const NoDefaultMethods &) = delete;
#else
    // do not implement
    NoDefaultMethods(const NoDefaultMethods&);
    NoDefaultMethods & operator=(const NoDefaultMethods &);
#endif
};

}}
#endif  /* NO_DEFAULT_METHODS_H */
