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
class epicsShareClass NoDefaultMethods {
protected:
    /**
     * Constructor
     */
    NoDefaultMethods(){};
    /**
     * Destructor
     */
    ~NoDefaultMethods(){}
    private:
    // do not implement
    NoDefaultMethods(const NoDefaultMethods&);
    NoDefaultMethods & operator=(const NoDefaultMethods &);
};

}}
#endif  /* NO_DEFAULT_METHODS_H */
