/* status.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mse
 */
#ifndef STATUS_H
#define STATUS_H

#include <ostream>

#include <pv/serialize.h>
#include <pv/byteBuffer.h>
#include <pv/sharedPtr.h>

#include <shareLib.h>

namespace epics { namespace pvData { 

        /**
         * @brief Status.
         *
         * This is a class for returning status to clients.
         * @author mse
         */
        class epicsShareClass Status : public epics::pvData::Serializable {
            public:
                POINTER_DEFINITIONS(Status);
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
        	
        	static Status Ok;

            /**
             * Creates OK status; STATUSTYPE_OK, empty message and stackDump.
             */
            Status();
        	
        	/**
        	 * Create non-OK status.
        	 */
            Status(StatusType type, std::string const & message);
        
            /**
             * Create non-OK status.
             */
            Status(StatusType type, std::string const & message, std::string const & stackDump);
        
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
            std::string getMessage() const;

            /**
             * Get stack dump where error (exception) happened. Optional.
             * @return stack dump.
             */
            std::string getStackDump() const;

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

            void serialize(ByteBuffer *buffer, SerializableControl *flusher) const;
            void deserialize(ByteBuffer *buffer, DeserializableControl *flusher);
    
            void dump(std::ostream& o) const;

            private:
                        
            StatusType m_statusType;
            std::string m_message;
            std::string m_stackDump;

        };

        epicsShareExtern std::ostream& operator<<(std::ostream& o, const Status& status);
        epicsShareExtern std::ostream& operator<<(std::ostream& o, const Status::StatusType& statusType);

}}
#endif  /* STATUS_H */
