/* byteBuffer.h */
#include <string>
#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H
#include "epicsTypes.h"
#include "pvIntrospect.h"
namespace epics { namespace pvData { 

// not sure why I have to define epicsInt64
typedef long long epicsInt64;

class ByteBuffer {
    public:
    virtual ~ByteBuffer();
    virtual int getSize() const = 0;
    virtual int getArrayOffset() const = 0;
    virtual epicsBoolean  getBoolean() const = 0;
    virtual epicsInt8 getByte() const = 0;
    virtual epicsInt16 geShort() const = 0;
    virtual epicsInt32 getInt() const = 0;
    virtual epicsInt64 getLong() const = 0;
    virtual float getFloat() const = 0;
    virtual double getDouble() const = 0;
    virtual StringConstPtr getString() const = 0;
    virtual ByteBuffer * putBoolean(epicsBoolean value) const = 0;
    virtual ByteBuffer * putByte(epicsInt8 value) const = 0;
    virtual ByteBuffer * geShort(epicsInt16 value) const = 0;
    virtual ByteBuffer * putInt(epicsInt32 value) const = 0;
    virtual ByteBuffer * putLong(epicsInt64 value) const = 0;
    virtual ByteBuffer * putFloat(float value) const = 0;
    virtual ByteBuffer * putDouble(double value) const = 0;
    virtual ByteBuffer * putString(StringConstPtr value) const = 0;
    // Must define arrays
};

}}
#endif  /* BYTEBUFFER_H */
