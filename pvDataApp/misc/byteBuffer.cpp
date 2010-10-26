/*
 * byteBuffer.cpp
 *
 *  Created on: Oct 18, 2010
 *      Author: Miha Vitorovic
 */

#include "byteBuffer.h"
#include "epicsException.h"

namespace epics {
    namespace pvData {

        using std::stringstream;

        ByteBuffer::ByteBuffer(int size, int byteOrder) :
                _bufferByteOrder(byteOrder), _size(size), _position(0),
                _limit(size), _buffer(0) {

            if (size < 0)
                throw EpicsException("negative size");

            if (byteOrder!=EPICS_ENDIAN_BIG && byteOrder!=EPICS_ENDIAN_LITTLE)
                throw EpicsException("invalid endian");

            _buffer = new char[_size];
        }

        ByteBuffer::~ByteBuffer() {
            if (_buffer) delete _buffer;
        }

        ByteBuffer* ByteBuffer::clear() {
            _position = 0;
            _limit = _size;
            return this;
        }

        ByteBuffer* ByteBuffer::flip() {
            _limit = _position;
            _position = 0;
            return this;
        }

        ByteBuffer* ByteBuffer::rewind() {
            _position = 0;
            return this;
        }

        bool ByteBuffer::getBoolean() {
            if(_position<_limit)
                return _buffer[_position++]==0 ? false : true;
            else
                throw EpicsException("buffer underflow");
        }

        epicsInt8 ByteBuffer::getByte() {
            if(_position<_limit)
                return (epicsInt8)_buffer[_position++];
            else
                throw EpicsException("buffer underflow");
        }

        epicsInt16 ByteBuffer::getShort() {
            if(_limit-_position<(int)sizeof(epicsInt16))
                throw EpicsException("buffer underflow");
            epicsInt16 val;
            getWithEndianness((char*)&val, sizeof(epicsInt16)); // store short into val
            return val;
        }

        epicsInt32 ByteBuffer::getInt() {
            if(_limit-_position<(int)sizeof(epicsInt32))
                throw EpicsException("buffer underflow");
            epicsInt32 val;
            getWithEndianness((char*)&val, sizeof(epicsInt32)); // store int into val
            return val;
        }

        epicsInt64 ByteBuffer::getLong() {
            if(_limit-_position<(int)sizeof(epicsInt64))
                throw EpicsException("buffer underflow");
            epicsInt64 val;
            getWithEndianness((char*)&val, sizeof(epicsInt64)); // store long into val
            return val;
        }

        float ByteBuffer::getFloat() {
            if(_limit-_position<(int)sizeof(float))
                throw EpicsException("buffer underflow");
            float val;
            getWithEndianness((char*)&val, sizeof(float)); // store float into val
            return val;
        }

        double ByteBuffer::getDouble() {
            if(_limit-_position<(int)sizeof(double))
                throw EpicsException("buffer underflow");
            double val;
            getWithEndianness((char*)&val, sizeof(double)); // store double into val
            return val;
        }

        ByteBuffer* ByteBuffer::put(const char* src, int offset, int count) {
            if(count>getRemaining()) throw EpicsException("buffer overflow");
            strncpy(&_buffer[_position], &src[offset], count);
            _position += count;
            return this;
        }

        void ByteBuffer::get(char*dst, int offset, int count) {
            if(count>getRemaining()) throw EpicsException("buffer underflow");
            strncpy(&dst[offset], &_buffer[_position], count);
            _position += count;
        }

        ByteBuffer* ByteBuffer::putBoolean(bool value) {
            if(_position<_limit)
                _buffer[_position++] = value ? 1 : 0;
            else
                throw EpicsException("buffer overflow");
            return this;
        }

        ByteBuffer* ByteBuffer::putByte(epicsInt8 value) {
            if(_position<_limit)
                _buffer[_position++] = (char)value;
            else
                throw EpicsException("buffer overflow");
            return this;
        }

        ByteBuffer* ByteBuffer::putShort(epicsInt16 value) {
            if(_limit-_position<(int)sizeof(epicsInt16))
                throw EpicsException("buffer overflow");
            putWithEndianness((char*)&value, sizeof(epicsInt16)); // store short into buffer
            return this;
        }

        ByteBuffer* ByteBuffer::putInt(epicsInt32 value) {
            if(_limit-_position<(int)sizeof(epicsInt32))
                throw EpicsException("buffer overflow");
            putWithEndianness((char*)&value, sizeof(epicsInt32)); // store int into buffer
            return this;
        }

        ByteBuffer* ByteBuffer::putLong(epicsInt64 value) {
            if(_limit-_position<(int)sizeof(epicsInt64))
                throw EpicsException("buffer overflow");
            putWithEndianness((char*)&value, sizeof(epicsInt64)); // store long into buffer
            return this;
        }

        ByteBuffer* ByteBuffer::putFloat(float value) {
            if(_limit-_position<(int)sizeof(float))
                throw EpicsException("buffer overflow");
            putWithEndianness((char*)&value, sizeof(float)); // store float into buffer
            return this;
        }

        ByteBuffer* ByteBuffer::putDouble(double value) {
            if(_limit-_position<(int)sizeof(double))
                throw EpicsException("buffer overflow");
            putWithEndianness((char*)&value, sizeof(double)); // store double into buffer
            return this;
        }

        /**
         * Buffer underflow or overflow checks are performed in the caller.
         */
        void ByteBuffer::getWithEndianness(char* dest, size_t size) {
            if(_bufferByteOrder==EPICS_BYTE_ORDER)
                for(size_t i = 0; i<size; i++)
                    dest[i] = _buffer[_position++];
            else
                for(int i = (int)size-1; i>=0; i--)
                    dest[i] = _buffer[_position++];
        }

        /**
         * Buffer underflow or overflow checks are performed in the caller.
         */
        void ByteBuffer::putWithEndianness(char* src, size_t size) {
            if(_bufferByteOrder==EPICS_BYTE_ORDER)
                for(size_t i = 0; i<size; i++)
                    _buffer[_position++] = src[i];
            else
                for(int i = (int)size-1; i>=0; i--)
                    _buffer[_position++] = src[i];
        }

    }
}
