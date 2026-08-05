/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */
/*
 * serializeHelper.h
 *
 *  Created on: Oct 21, 2010
 *      Author: Miha Vitorovic
 */

#ifndef SERIALIZEHELPER_H_
#define SERIALIZEHELPER_H_

#include <pv/serialize.h>
#include <pv/byteBuffer.h>
#include <pv/noDefaultMethods.h>
#include <pv/pvIntrospect.h>

#include <shareLib.h>

namespace epics {
    namespace pvData {

        /**
         * @brief Serialization helper.
         *
         */
        class epicsShareClass SerializeHelper {
            EPICS_NOT_COPYABLE(SerializeHelper)
        public:

            /**
             * Serialize the specified array size into the specified
             * buffer, flushing when necessary.
             * The specified SerializableControl manages any flushing
             * required.
             *
             * @param[in] s size to encode
             * @param[in] buffer serialization buffer
             * @param[in] flusher SerializableControl to manage the flushing
             */
            static void writeSize(std::size_t s, ByteBuffer* buffer,
                    SerializableControl* flusher);

            /**
             * Deserialize array size.
             * The specified DeserializableControl ensures
             * sufficient bytes are available.
             *
             * @param[in] buffer deserialization buffer.
             * @param[in] control the DeserializableControl.
             * @returns array size, or (size_t) -1.
             */
            static std::size_t readSize(ByteBuffer* buffer,
                    DeserializableControl* control);

            /**
             * std::string serialization helper method.
             *
             * @param[in] value std::string to serialize
             * @param[in] buffer serialization buffer
             * @param[in] flusher flusher
             */
            static void serializeString(const std::string& value, ByteBuffer* buffer,
                    SerializableControl* flusher);

            /**
             * std::string serialization helper method.
             *
             * @param[in] value std::string to serialize
             * @param[in] offset start of the substring in value
             * @param[in] count the number of characters to write
             * @param[in] buffer serialization buffer
             * @param[in] flusher flusher
             */
            static void serializeSubstring(const std::string& value, std::size_t offset,
                    std::size_t count, ByteBuffer* buffer,
                    SerializableControl* flusher);

            /**
             * std::string deserialization helper method.
             *
             * @param[in] buffer deserialization buffer
             * @param[in] control control
             * @param[in] max length limit
             * @returns deserialized string
             *
             * @todo This method cannot return "null", but Java implementation
             * could have serialized "null" value as well. This implementation
             * returns an empty string in that event.
             */
            static std::string deserializeString(ByteBuffer* buffer,
                    DeserializableControl* control, std::size_t max = INT32_MAX);

        private:
            SerializeHelper() {};
            ~SerializeHelper() {};

            /**
             * Serialize array size.
             *
             * @param[in] s size to encode
             * @param[in] buffer serialization buffer
             */
            static void writeSize(std::size_t s, ByteBuffer* buffer);

        };

    }
}

#define DESERIALIZE_STRING_HAS_LIMIT

#endif /* SERIALIZEHELPER_H_ */
