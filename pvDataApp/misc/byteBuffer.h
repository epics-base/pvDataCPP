/* byteBuffer.h */
#include <string>
#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H
#include "epicsTypes.h"
namespace epics { namespace pvData { 

// not sure why I have to define epicsInt64
typedef long long epicsInt64;

class ByteBuffer;

class ByteBuffer {
    public:
    virtual ~ByteBuffer();
    virtual int getSize() = 0;
    virtual int getArrayOffset() = 0;
    virtual epicsBoolean  getBoolean() = 0;
    virtual epicsInt8 getByte() = 0;
    virtual epicsInt16 geShort() = 0;
    virtual epicsInt32 getInt() = 0;
    virtual epicsInt64 getLong() = 0;
    virtual float getFloat() = 0;
    virtual double getDouble() = 0;
    virtual String getString() = 0;
    virtual ByteBuffer *putBoolean(epicsBoolean value) = 0;
    virtual ByteBuffer *putByte(epicsInt8 value) = 0;
    virtual ByteBuffer *geShort(epicsInt16 value) = 0;
    virtual ByteBuffer *putInt(epicsInt32 value) = 0;
    virtual ByteBuffer *putLong(epicsInt64 value) = 0;
    virtual ByteBuffer *putFloat(float value) = 0;
    virtual ByteBuffer *putDouble(double value) = 0;
    virtual ByteBuffer *putString(String value) = 0;
    // Must define arrays
};

}}
#endif  /* BYTEBUFFER_H */
