/* destroyable.h */
/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */
/**
 *  @author mse
 */
#ifndef DESTROYABLE_H
#define DESTROYABLE_H

#include <pv/sharedPtr.h>

#include <shareLib.h>

namespace epics { namespace pvData { 


        /**
         * @brief Instance declaring destroy method.
         * 
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
