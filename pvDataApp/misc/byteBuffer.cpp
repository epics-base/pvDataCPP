/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/*
 * byteBuffer.cpp
 *
 *  Created on: Oct 18, 2010
 *      Author: Miha Vitorovic
 */
#include <cstring>
#include "byteBuffer.h"
#include "epicsException.h"

// TODO optimize, avoid so many checks (endianness, positions), allow wrapping of external buffer, chance of endianess
namespace epics {
    namespace pvData {

        using std::stringstream;

        ByteBuffer::ByteBuffer(int size, int byteOrder) :
                _bufferByteOrder(byteOrder), _size(size), _position(0),
                _limit(size), _buffer(0) {

            if(size<0) THROW_BASE_EXCEPTION("negative size");

            if (byteOrder!=EPICS_ENDIAN_BIG && byteOrder!=EPICS_ENDIAN_LITTLE)
                THROW_BASE_EXCEPTION("invalid endianness");

            _buffer = new char[_size];
        }

        ByteBuffer::~ByteBuffer() {
            if (_buffer) delete[] _buffer;
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
            return getBoolean(_position++);
        }

        bool ByteBuffer::getBoolean(int index) {
            if(index>=0&&index<_limit)
                return _buffer[index]==0 ? false : true;
            else
                THROW_BASE_EXCEPTION("index out of bounds");
        }

        int8 ByteBuffer::getByte() {
            return getByte(_position++);
        }

        int8 ByteBuffer::getByte(int index) {
            if(index>=0&&index<_limit)
                return (int8)_buffer[index];
            else
                THROW_BASE_EXCEPTION("index out of bounds");
        }

        int16 ByteBuffer::getShort() {
            if(_limit-_position<(int)sizeof(int16))
                THROW_BASE_EXCEPTION("buffer underflow");
            int16 val;
            getWithEndianness((char*)&val, sizeof(int16)); // store short into val
            return val;
        }

        int16 ByteBuffer::getShort(int index) {
            if(index<0||_limit-index<(int)sizeof(int16))
                THROW_BASE_EXCEPTION("index out of bounds");
            int16 val;
            getWithEndianness(index, (char*)&val, sizeof(int16)); // store short into val
            return val;

        }

        int32 ByteBuffer::getInt() {
            if(_limit-_position<(int)sizeof(int32))
                THROW_BASE_EXCEPTION("buffer underflow");
            int32 val;
            getWithEndianness((char*)&val, sizeof(int32)); // store int into val
            return val;
        }

        int32 ByteBuffer::getInt(int index) {
            if(index<0||_limit-index<(int)sizeof(int32))
                THROW_BASE_EXCEPTION("index out of bounds");
            int32 val;
            getWithEndianness(index, (char*)&val, sizeof(int32)); // store int into val
            return val;
        }

        int64 ByteBuffer::getLong() {
            if(_limit-_position<(int)sizeof(int64))
                THROW_BASE_EXCEPTION("buffer underflow");
            int64 val;
            getWithEndianness((char*)&val, sizeof(int64)); // store long into val
            return val;
        }

        int64 ByteBuffer::getLong(int index) {
            if(index<0||_limit-index<(int)sizeof(int64))
                THROW_BASE_EXCEPTION("index out of bounds");
            int64 val;
            getWithEndianness(index, (char*)&val, sizeof(int64)); // store long into val
            return val;
        }

        float ByteBuffer::getFloat() {
            if(_limit-_position<(int)sizeof(float))
                THROW_BASE_EXCEPTION("buffer underflow");
            float val;
            getWithEndianness((char*)&val, sizeof(float)); // store float into val
            return val;
        }

        float ByteBuffer::getFloat(int index) {
            if(index<0||_limit-index<(int)sizeof(float))
                THROW_BASE_EXCEPTION("index out of bounds");
            float val;
            getWithEndianness(index, (char*)&val, sizeof(float)); // store float into val
            return val;
        }

        double ByteBuffer::getDouble() {
            if(_limit-_position<(int)sizeof(double))
                THROW_BASE_EXCEPTION("buffer underflow");
            double val;
            getWithEndianness((char*)&val, sizeof(double)); // store double into val
            return val;
        }

        double ByteBuffer::getDouble(int index) {
            if(index>=0&&_limit-index<(int)sizeof(double)) THROW_BASE_EXCEPTION(
                    "index out of bounds");
            double val;
            getWithEndianness(index, (char*)&val, sizeof(double)); // store double into val
            return val;
        }

        void ByteBuffer::get(char* dst, int offset, int count) {
            if(count>getRemaining()) THROW_BASE_EXCEPTION("buffer underflow");
            for(int i = 0; i<count; i++)
                dst[offset+i] = _buffer[_position++];
        }

        ByteBuffer* ByteBuffer::put(const char* src, int offset, int count) {
            if(count>getRemaining()) THROW_BASE_EXCEPTION("buffer overflow");
            for(int i = 0; i<count; i++)
                _buffer[_position++] = src[offset+i];
            return this;
        }

        ByteBuffer* ByteBuffer::putBoolean(bool value) {
            return putByte(_position++, value ? 1 : 0);
        }

        ByteBuffer* ByteBuffer::putBoolean(int index, bool value) {
            return putByte(index, value ? 1 : 0);
        }

        ByteBuffer* ByteBuffer::putByte(int8 value) {
            return putByte(_position++, value);
        }

        ByteBuffer* ByteBuffer::putByte(int index, int8 value) {
            if(index>=0&&index<_limit)
                _buffer[index] = (char)value;
            else
                THROW_BASE_EXCEPTION("index out of bounds");
            return this;
        }

        ByteBuffer* ByteBuffer::putShort(int16 value) {
            if(_limit-_position<(int)sizeof(int16))
                THROW_BASE_EXCEPTION("buffer overflow");
            putWithEndianness((char*)&value, sizeof(int16)); // store short into buffer
            return this;
        }

        ByteBuffer* ByteBuffer::putShort(int index, int16 value) {
            if(index<0||_limit-index<(int)sizeof(int16))
                THROW_BASE_EXCEPTION("index out of bounds");
            putWithEndianness(index, (char*)&value, sizeof(int16)); // store short into buffer
            return this;
        }

        ByteBuffer* ByteBuffer::putInt(int32 value) {
            if(_limit-_position<(int)sizeof(int32))
                THROW_BASE_EXCEPTION("buffer overflow");
            putWithEndianness((char*)&value, sizeof(int32)); // store int into buffer
            return this;
        }

        ByteBuffer* ByteBuffer::putInt(int index, int32 value) {
            if(index<0||_limit-index<(int)sizeof(int32))
                THROW_BASE_EXCEPTION("index out of bounds");
            putWithEndianness(index, (char*)&value, sizeof(int32)); // store int into buffer
            return this;
        }

        ByteBuffer* ByteBuffer::putLong(int64 value) {
            if(_limit-_position<(int)sizeof(int64))
                THROW_BASE_EXCEPTION("buffer overflow");
            putWithEndianness((char*)&value, sizeof(int64)); // store long into buffer
            return this;
        }

        ByteBuffer* ByteBuffer::putLong(int index, int64 value) {
            if(index<0||_limit-index<(int)sizeof(int64))
                THROW_BASE_EXCEPTION("index out of bounds");
            putWithEndianness(index, (char*)&value, sizeof(int64)); // store long into buffer
            return this;
        }

        ByteBuffer* ByteBuffer::putFloat(float value) {
            if(_limit-_position<(int)sizeof(float))
                THROW_BASE_EXCEPTION("buffer overflow");
            putWithEndianness((char*)&value, sizeof(float)); // store float into buffer
            return this;
        }

        ByteBuffer* ByteBuffer::putFloat(int index, float value) {
            if(index<0||_limit-index<(int)sizeof(float))
                THROW_BASE_EXCEPTION("index out of bounds");
            putWithEndianness(index, (char*)&value, sizeof(float)); // store float into buffer
            return this;
        }

        ByteBuffer* ByteBuffer::putDouble(double value) {
            if(_limit-_position<(int)sizeof(double))
                THROW_BASE_EXCEPTION("buffer overflow");
            putWithEndianness((char*)&value, sizeof(double)); // store double into buffer
            return this;
        }

        ByteBuffer* ByteBuffer::putDouble(int index, double value) {
            if(index<0||_limit-index<(int)sizeof(double))
                THROW_BASE_EXCEPTION("index out of bounds");
            putWithEndianness(index, (char*)&value, sizeof(double)); // store double into buffer
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

        /**
         * Buffer underflow or overflow checks are performed in the caller.
         */
        void ByteBuffer::putWithEndianness(int index, char* src, size_t size) {
            if(_bufferByteOrder==EPICS_BYTE_ORDER)
                for(size_t i = 0; i<size; i++)
                    _buffer[index++] = src[i];
            else
                for(int i = (int)size-1; i>=0; i--)
                    _buffer[index++] = src[i];
        }

        /**
         * Buffer underflow or overflow checks are performed in the caller.
         */
        void ByteBuffer::getWithEndianness(int index, char* dest, size_t size) {
            if(_bufferByteOrder==EPICS_BYTE_ORDER)
                for(size_t i = 0; i<size; i++)
                    dest[i] = _buffer[index++];
            else
                for(int i = (int)size-1; i>=0; i--)
                    dest[i] = _buffer[index++];
        }

    }
}
