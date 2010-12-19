/* status.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef STATUS_H
#define STATUS_H

#include "serialize.h"

namespace epics { namespace pvData { 

    	/**
    	 * Status type enum.
    	 */
    	enum StatusType { 
                /** Operation completed successfully. */
                OK,
                /** Operation completed successfully, but there is a warning message. */
                WARNING,
                /** Operation failed due to an error. */
                ERROR,
                /** Operation failed due to an unexpected error. */
                FATAL
    	};

        /**
         * Status interface.
         * @author mse
         */
        class Status : public epics::pvData::Serializable {
            public:

            /**
             * Get status type.
             * @return status type, non-<code>null</code>.
             */
            virtual StatusType getType() = 0;

            /**
             * Get error message describing an error. Required if error status.
             * @return error message.
             */
            virtual epics::pvData::String getMessage() = 0;

            /**
             * Get stack dump where error (exception) happened. Optional.
             * @return stack dump.
             */
            virtual epics::pvData::String getStackDump() = 0;

            /**
             * Convenient OK test. Same as <code>(getType() == StatusType.OK)</code>.
             * NOTE: this will return <code>false</code> on WARNING message although operation succeeded.
             * To check if operation succeeded, use <code>isSuccess</code>.
             * @return OK status.
             * @see #isSuccess()
             */
            virtual bool isOK() = 0;

            /**
             * Check if operation succeeded.
             * @return operation success status.
             */
            virtual bool isSuccess() = 0;
        };

}}
#endif  /* STATUS_H */
