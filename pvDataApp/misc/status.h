/* status.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef STATUS_H
#define STATUS_H

#include "serialize.h"
#include "byteBuffer.h"

namespace epics { namespace pvData { 

        /**
         * Status.
         * @author mse
         */
        class Status : public epics::pvData::Serializable {
            public:
            
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
        	
        	static const char* StatusTypeName[];
        	
        	static Status OK;

            /**
             * Creates OK status; STATUSTYPE_OK, empty message and stackDump.
             */
            Status();
        	
        	/**
        	 * Create non-OK status.
        	 */
            Status(StatusType type, epics::pvData::String message);
        
            /**
             * Create non-OK status.
             */
            Status(StatusType type, epics::pvData::String message, epics::pvData::String stackDump);
        
            ~Status();
            
            /**
             * Get status type.
             * @return status type, non-<code>null</code>.
             */
            StatusType getType() const;

            /**
             * Get error message describing an error. Required if error status.
             * @return error message.
             */
            epics::pvData::String getMessage() const;

            /**
             * Get stack dump where error (exception) happened. Optional.
             * @return stack dump.
             */
            epics::pvData::String getStackDump() const;

            /**
             * Convenient OK test. Same as <code>(getType() == StatusType.OK)</code>.
             * NOTE: this will return <code>false</code> on WARNING message although operation succeeded.
             * To check if operation succeeded, use <code>isSuccess</code>.
             * @return OK status.
             * @see #isSuccess()
             */
            bool isOK() const;

            /**
             * Check if operation succeeded.
             * @return operation success status.
             */
            bool isSuccess() const;

            String toString() const;
            void toString(StringBuilder buffer, int indentLevel = 0) const;
            
            void serialize(ByteBuffer *buffer, SerializableControl *flusher) const;
            void deserialize(ByteBuffer *buffer, DeserializableControl *flusher);
    
            private:
            
            static epics::pvData::String m_emptyString;
            
            StatusType m_type;
            String m_message;
            String m_stackDump;
        };

}}
#endif  /* STATUS_H */
