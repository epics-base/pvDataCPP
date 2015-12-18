/* noDefaultMethods.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
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
