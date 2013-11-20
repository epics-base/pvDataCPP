/* destroyable.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mse
 */
#ifndef DESTROYABLE_H
#define DESTROYABLE_H

#include <pv/sharedPtr.h>

#include <sharelib.h>

namespace epics { namespace pvData { 


        /**
         * Instance declaring destroy method.
         * @author mse
         */
        class epicsShareClass Destroyable {
            public:
            POINTER_DEFINITIONS(Destroyable);
            /**
             * Destroy this instance.
             */
            virtual void destroy() = 0;
            
            protected:
            /**
             * Do not allow delete on this instance and derived classes, destroy() must be used instead.
             */ 
            virtual ~Destroyable() {};
        };

}}
#endif  /* DESTROYABLE_H */
