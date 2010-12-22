/* destroyable.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef DESTROYABLE_H
#define DESTROYABLE_H
namespace epics { namespace pvData { 


        /**
         * Instance declaring destroy method.
         * @author mse
         */
        class Destroyable  {
            public:
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
