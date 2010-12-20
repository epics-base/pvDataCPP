/* status.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef STATUS_H
#define STATUS_H

#include "serialize.h"
#include "epicsException.h"
#include "byteBuffer.h"

namespace epics { namespace pvData { 

    	/**
    	 * Status type enum.
    	 */
    	enum StatusType { 
                /** Operation completed successfully. */
                STATUSTYPE_OK,
                /** Operation completed successfully, but there is a warning message. */
                STATUSTYPE_WARNING,
                /** Operation failed due to an error. */
                STATUSTYPE_ERROR,
                /** Operation failed due to an unexpected error. */
                STATUSTYPE_FATAL
    	};
    	
    	const char* StatusTypeName[] = { "OK", "WARNING", "ERROR", "FATAL" };

        /**
         * Status interface.
         * @author mse
         */
        class Status : public epics::pvData::Serializable {
            public:
            virtual ~Status() {};
            
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

            
            virtual void toString(StringBuilder buffer, int indentLevel = 0) = 0;
            
        };

        
        /**
         * Interface for creating status.
         * @author mse
         */
        class StatusCreate : private epics::pvData::NoDefaultMethods {
            public:
        	/**
        	 * Get OK status. Static instance should be returned.
        	 * @return OK <code>Status</code> instance.
        	 */
        	virtual Status* getStatusOK() = 0;
        	
        	/**
        	 * Create status.
        	 * @param type status type, non-<code>null</code>.
        	 * @param message message describing an error, non-<code>null</code>.
        	 * 		  NOTE: Do NOT use <code>throwable.getMessage()</code> as message, since it will be supplied with the <code>cause</code>.
        	 * @param cause exception that caused an error. Optional.
        	 * @return status instance.
        	 */
        	virtual Status* createStatus(StatusType type, String message, BaseException* cause = 0) = 0;
        	
        	/**
        	 * Deserialize status.
        	 * NOTE: use this method instead of <code>Status.deserialize()</code>, since this allows OK status optimization. 
        	 * @param buffer deserialization buffer.
        	 * @param control deserialization control.
        	 * @return status instance.
        	 */
        	virtual Status* deserializeStatus(ByteBuffer* buffer, DeserializableControl* control) = 0;
        };
        
        extern StatusCreate* getStatusCreate();
        

}}
#endif  /* STATUS_H */
