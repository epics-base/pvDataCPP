/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
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

namespace epics {
    namespace pvData {

        class SerializeHelper : public NoDefaultMethods {
        public:

            /**
             * Serialize array size.
             *
             * @param[in] s size to encode
             * @param[in] buffer serialization buffer
             * @param[in] flusher flusher
             */
            static void writeSize(int s, ByteBuffer* buffer,
                    SerializableControl* flusher);

            /**
             * Deserialize array size.
             *
             * @param[in] buffer deserialization buffer.
             * @returns array size.
             */
            static int readSize(ByteBuffer* buffer,
                    DeserializableControl* control);

            /**
             * String serialization helper method.
             *
             * @param[in] value String to serialize
             * @param[in] buffer serialization buffer
             * @param[in] flusher flusher
             */
            static void serializeString(const String& value, ByteBuffer* buffer,
                    SerializableControl* flusher);

            /**
             * String serialization helper method.
             *
             * @param[in] value String to serialize
             * @param[in] offset start of the substring in {@code value}
             * @param[in] count the number of characters to write
             * @param[in] buffer serialization buffer
             * @param[in] flusher flusher
             */
            static void serializeSubstring(const String& value, int offset,
                    int count, ByteBuffer* buffer,
                    SerializableControl* flusher);

            /**
             * String deserialization helper method.
             * TODO This method cannot return "null", but Java implementation
             * could have serialized "null" value as well. We need to decide
             * how to deserialize "null".
             *
             * @param[in] buffer deserialization buffer
             * @param[in] control control
             * @returns deserialized string
             *
             * @todo This method cannot return "null", but Java implementation
             * could have serialized "null" value as well. We need to decide
             * how to deserialize "null".
             */
            static String deserializeString(ByteBuffer* buffer,
                    DeserializableControl* control);

        private:
            SerializeHelper() {};
            ~SerializeHelper() {};

            /**
             * Serialize array size.
             *
             * @param[in] s size to encode
             * @param[in] buffer serialization buffer
             */
            static void writeSize(int s, ByteBuffer* buffer);

        };

    }
}

#endif /* SERIALIZEHELPER_H_ */
