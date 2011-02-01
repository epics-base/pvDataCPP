/* byteBuffer.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H

#include <string>
#include <pvType.h>
#include <epicsEndian.h>

#include "epicsException.h"

namespace epics {
    namespace pvData {

        /** @brief A buffer of bytes.
         *
         * A buffer of bytes, which has
         *  - capacity (size)
         *  - limit
         *  - position
         * The capacity is the maximum capacity of the buffer, the buffer's
         * limit is the index of the first element that should not be read or
         * written, and its position is the index of the next element to be
         * written.
         *
         * The buffer also ha a byte order specified which can be little or
         * big endian.
         */
        class ByteBuffer {
        public:
            /** @brief ByteBuffer constructor.
             *
             * Creates a buffer of a given size, and with a specified byteOrder.
             *
             * @param size Specify the capacity of the buffer. Default is 32.
             * @param byteOrder the byte order of the buffer. Can be either
             * EPICS_ENDIAN_LITTLE or EPICS_ENDIAN_BIG.
             */
            ByteBuffer(int size = 32, int byteOrder = EPICS_BYTE_ORDER);

            /**
             * @brief ByteBuffer destructor.
             */
            ~ByteBuffer();

            /**
             * Clears the buffer. Sets the {@code position} to 0 and {@code limit}
             * to {@code capacity}.
             */
            ByteBuffer* clear();

            /**
             * Flips the buffer. The {@code limit} is set to the current {@code position}
             * and the {@code position} is set to {@code 0}. This prepares the
             * buffer for a series of relative @em get operations after a series
             * of relative @em put operations.
             */
            ByteBuffer* flip();

            /**
             * Rewinds the buffer. This sets {@code position} to {@code 0}
             * without affecting the {@code limit}.
             */
            ByteBuffer* rewind();

            /**
             * Relative boolean read, {@code position} is incremented by
             * {@code 1}.
             *
             * @returns The next byte in the buffer as a boolean.
             * @throws EpicsException - Buffer underflow if there are no bytes
             * remaining in the buffer.
             */
            bool getBoolean();

            /**
             * Absolute get method. Reads the bool at the given {@code index}.
             *
             * @param[in] index The index from which the bool will be read
             * @returns The bool at the given index
             * @throws EpicsException - If index is negative or not smaller
             * than the buffer's limit
             */
            bool getBoolean(int index);

            /**
             * Relative Int8 read, {@code position} is incremented by
             * {@code 1}.
             *
             * @returns The next byte in the buffer as a signed Int8.
             * @throws EpicsException - Buffer underflow if there are no bytes
             * remaining in the buffer.
             */
            int8 getByte();

            /**
             * Absolute get method. Reads the byte at the given {@code index}.
             *
             * @param[in] index The index from which the byte will be read
             * @returns The byte at the given index
             * @throws EpicsException - If index is negative or not smaller
             * than the buffer's limit
             */
            int8 getByte(int index);

            /**
             * Relative Int16 read, {@code position} is incremented by
             * {@code 2}.
             *
             * @returns The next Int16 value in the buffer.
             * @throws EpicsException - Buffer underflow if there are less than
             * 2 bytes remaining in the buffer.
             */
            int16 getShort();

            /**
             * Absolute get method. Reads the int16 at the given {@code index}.
             *
             * @param[in] index The index from which the int16 will be read
             * @returns The int16 at the given index
             * @throws EpicsException - If index is negative or not within
             * the buffer's limit
             */
            int16 getShort(int index);

            /**
             * Relative Int32 read, {@code position} is incremented by
             * {@code 4}.
             *
             * @returns The next Int32 value in the buffer.
             * @throws EpicsException - Buffer underflow if there are less than
             * 4 bytes remaining in the buffer.
             */
            int32 getInt();

            /**
             * Absolute get method. Reads the int32 at the given {@code index}.
             *
             * @param[in] index The index from which the int32 will be read
             * @returns The int32 at the given index
             * @throws EpicsException - If index is negative or not within
             * the buffer's limit
             */
            int32 getInt(int index);

            /**
             * Relative Int64 read, {@code position} is incremented by
             * {@code 8}.
             *
             * @returns The next Int64 value in the buffer.
             * @throws EpicsException - Buffer underflow if there are less than
             * 8 bytes remaining in the buffer.
             */
            int64 getLong();

            /**
             * Absolute get method. Reads the int64 at the given {@code index}.
             *
             * @param[in] index The index from which the int64 will be read
             * @returns The int64 at the given index
             * @throws EpicsException - If index is negative or not within
             * the buffer's limit
             */
            int64 getLong(int index);

            /**
             * Relative float read, {@code position} is incremented by
             * {@code 4}.
             *
             * @returns The next float value in the buffer.
             * @throws EpicsException - Buffer underflow if there are less than
             * 4 bytes remaining in the buffer.
             */
            float getFloat();

            /**
             * Absolute get method. Reads the float at the given {@code index}.
             *
             * @param[in] index The index from which the float will be read
             * @returns The float at the given index
             * @throws EpicsException - If index is negative or not within
             * the buffer's limit
             */
            float getFloat(int index);

            /**
             * Relative double read, {@code position} is incremented by
             * {@code 8}.
             *
             * @returns The next double value in the buffer.
             * @throws EpicsException - Buffer underflow if there are less than
             * 8 bytes remaining in the buffer.
             */
            double getDouble();

            /**
             * Absolute get method. Reads the double at the given {@code index}.
             *
             * @param[in] index The index from which the double will be read
             * @returns The double at the given index
             * @throws EpicsException - If index is negative or not within
             * the buffer's limit
             */
            double getDouble(int index);

            /**
             * Relative bulk @em get method. It transfers {@code count} bytes
             * from the buffer into the {@code dst}.
             *
             * @param[out] dst Destination buffer
             * @param[in] offset Offset in the destination buffer
             * @param[in] count The number of bytes to copy
             * @throws EpicsException - Buffer underflow if there are fewer
             * than count bytes remaining in the buffer.
             */
            void get(char* dst, int offset, int count);

            //virtual String getString() = 0; // TODO


            /**
             * Relative bulk @em put method. It transfers {@code count} bytes
             * from the {@code src} into the the buffer.
             *
             * @param[in] src Source buffer
             * @param[in] offset Offset in the source buffer
             * @param[in] count The number of bytes to copy
             * @returns Pointer to this ByteBuffer instance.
             * @throws EpicsException - Buffer overflow if there are not
             * enough bytes remaining in the buffer.
             */
            ByteBuffer* put(const char* src, int offset, int count);

            /**
             * Relative boolean write, {@code position} is incremented by
             * {@code 1}.
             *
             * @param[in] value The boolean value to write.
             * @throws EpicsException - Buffer overflow if there are no
             * bytes remaining in the buffer.
             */
            ByteBuffer* putBoolean(bool value);

            /**
             * Absolute bool write.
             *
             * @param[in] index The index at which the bool will be written.
             * @param[in] value The bool value to write.
             * @throws EpicsException - Buffer overflow if there are no
             * bytes remaining in the buffer.
             */
            ByteBuffer* putBoolean(int index, bool value);

            /**
             * Relative Int8 write, {@code position} is incremented by
             * {@code 1}.
             *
             * @param[in] value The Int8 value to write.
             * @throws EpicsException - Buffer overflow if there are no
             * bytes remaining in the buffer.
             */
            ByteBuffer* putByte(int8 value);

            /**
             * Absolute Int8 write.
             *
             * @param[in] index The index at which the byte will be written.
             * @param[in] value The Int8 value to write.
             * @throws EpicsException - Buffer overflow if there are no
             * bytes remaining in the buffer.
             */
            ByteBuffer* putByte(int index, int8 value);

            /**
             * Relative Int16 write, {@code position} is incremented by
             * {@code 2}.
             *
             * @param[in] value The Int16 value to write.
             * @throws EpicsException - Buffer overflow if there are less than
             * 2 bytes remaining in the buffer.
             */
            ByteBuffer* putShort(int16 value);

            /**
             * Absolute Int16 write.
             *
             * @param[in] index The index at which the Int16 will be written.
             * @param[in] value The Int16 value to write.
             * @throws EpicsException - Buffer overflow if there are less than
             * 2 bytes remaining in the buffer.
             */
            ByteBuffer* putShort(int index, int16 value);

            /**
             * Relative Int32 write, {@code position} is incremented by
             * {@code 4}.
             *
             * @param[in] value The Int32 value to write.
             * @throws EpicsException - Buffer overflow if there are less than
             * 4 bytes remaining in the buffer.
             */
            ByteBuffer* putInt(int32 value);

            /**
             * Absolute Int32 write.
             *
             * @param[in] index The index at which the Int32 will be written.
             * @param[in] value The Int32 value to write.
             * @throws EpicsException - Buffer overflow if there are less than
             * 4 bytes remaining in the buffer.
             */
            ByteBuffer* putInt(int index, int32 value);

            /**
             * Relative Int64 write, {@code position} is incremented by
             * {@code 8}.
             *
             * @param[in] value The Int64 value to write.
             * @throws EpicsException - Buffer overflow if there are less than
             * 8 bytes remaining in the buffer.
             */
            ByteBuffer* putLong(int64 value);

            /**
             * Absolute Int64 write.
             *
             * @param[in] index The index at which the Int64 will be written.
             * @param[in] value The Int64 value to write.
             * @throws EpicsException - Buffer overflow if there are less than
             * 8 bytes remaining in the buffer.
             */
            ByteBuffer* putLong(int index, int64 value);

            /**
             * Relative float write, {@code position} is incremented by
             * {@code 4}.
             *
             * @param[in] value The float value to write.
             * @throws EpicsException - Buffer overflow if there are less than
             * 4 bytes remaining in the buffer.
             */
            ByteBuffer* putFloat(float value);

            /**
             * Absolute float write.
             *
             * @param[in] index The index at which the float will be written.
             * @param[in] value The float value to write.
             * @throws EpicsException - Buffer overflow if there are less than
             * 4 bytes remaining in the buffer.
             */
            ByteBuffer* putFloat(int index, float value);

            /**
             * Relative float write, {@code position} is incremented by
             * {@code 8}.
             *
             * @param[in] value The double value to write.
             * @throws EpicsException - Buffer overflow if there are less than
             * 8 bytes remaining in the buffer.
             */
            ByteBuffer* putDouble(double value);

            /**
             * Absolute double write.
             *
             * @param[in] index The index at which the double will be written.
             * @param[in] value The double value to write.
             * @throws EpicsException - Buffer overflow if there are less than
             * 8 bytes remaining in the buffer.
             */
            ByteBuffer* putDouble(int index, double value);

            //virtual ByteBuffer *putString(String value) = 0; // TODO

            /**
             * The capacity (size) of the buffer in bytes.
             *
             * @returns The capacity of the buffer in bytes.
             */
            inline int getSize() const {
                return _size;
            }

            /**
             * The offset from the start of the buffer in bytes. Currently this
             * method always returns {@code 0}.
             *
             * @returns The offset from the start of the buffer in bytes
             */
            inline int getArrayOffset() const {
                return 0;
            }

            /**
             * The byte index of the next element to @em get or @em put.
             *
             * @returns The byte index of the next element.
             */
            inline int getPosition() const {
                return _position;
            }

            /**
             * The byte index of the {@code limit}. {@code limit} is always less
             * or equal to the buffer capacity. No @em put or @em get operation
             * can be performed pass the {@code limit}.
             *
             * @returns The byte index of the {@code limit}.
             */
            inline int getLimit() const {
                return _limit;
            }

            /**
             * The number of the bytes that can be successfully read or written.
             *
             * @returns The number of the bytes that can be successfully read or
             * written.
             */
            inline int getRemaining() const {
                return _limit-_position;
            }

            /**
             * The byte order of the buffer.
             *
             * @returns Either {@code EPICS_ENDIAN_LITTLE} or
             * {@code EPICS_ENDIAN_BIG}.
             */
            inline int getByteOrder() const {
                return _bufferByteOrder;
            }

            /**
             * Gives a read-only access to the buffer.
             *
             * @returns A const (read-only) pointer to the actual buffer.
             */
            inline const char* getArray() const {
                return _buffer;
            }

            /**
             * Sets this buffer's position. If the new position is invalid in
             * regards to the invariant then it is discarded.
             *
             * @param[in] newPosition The new position value; must be
             * non-negative and no larger than the current limit.
             * @throws EpicsException - If the preconditions on
             * {@code newPosition} do not hold
             */
            inline void setPosition(int newPosition) {
                if(newPosition<0||newPosition>_limit) THROW_BASE_EXCEPTION(
                        "invalid limit");
                _position = newPosition;
            }

            inline void setLimit(int newLimit) {
                if(newLimit<0||_position>newLimit) THROW_BASE_EXCEPTION(
                        "invalid limit");
                _limit = newLimit;
            }

            // TODO must define arrays

        private:
            int _bufferByteOrder, _size, _position, _limit;
            char* _buffer;

            /**
             * Reads the next <pre>size</pre> bytes from the buffer and stores them
             * into the destination taking into account endianness of the buffer
             * and the current hardware.
             */
            void getWithEndianness(char* dest, size_t size);

            /**
             * Puts the  next <pre>size</pre> bytes into the buffer reading them
             * from source taking into account endianness of the buffer
             * and the current hardware.
             */
            void putWithEndianness(char* src, size_t size);

            /**
             * Puts the <pre>size</pre> bytes into the buffer reading them
             * from source taking into account endianness of the buffer
             * and the current hardware.
             */
            void putWithEndianness(int index, char* src, size_t size);

            /**
             * Reads the next <pre>size</pre> bytes from the buffer and stores them
             * into the destination taking into account endianness of the buffer
             * and the current hardware.
             */
            void getWithEndianness(int index, char* dest, size_t size);

        };

    }
}
#endif  /* BYTEBUFFER_H */
