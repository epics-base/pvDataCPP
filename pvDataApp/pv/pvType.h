/* pvType.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 * Author - Marty Kraimer
 */

/* Definitions for the primitive types for pvData.
 * It also defines the arrays of the primitive types
 */

#ifndef PVTYPE_H
#define PVTYPE_H
#include <string>
#include <vector>

#ifdef __vxworks
typedef int intptr_t;
typedef unsigned int uintptr_t;
#else
#include <stdint.h>
#endif

#include <pv/sharedPtr.h>

namespace epics { namespace pvData { 

/**
 * This is a set of typdefs used by pvData.
 */

/**
 * boolean, i.e. can only have the values {@code false} or {@code true}
 */
typedef uint8_t     boolean;
/**
 * A 8 bit signed integer
 */
typedef int8_t   int8;
/**
 * A 16 bit signed integer
 */
typedef int16_t  int16;
/**
 * A 32 bit signed integer
 */
typedef int32_t  int32;
/**
 * A 64 bit signed integer
 */
typedef int64_t  int64;
/**
 * A 8 bit unsigned integer
 */
typedef uint8_t   uint8;
/**
 * A 16 bit unsigned integer
 */
typedef uint16_t  uint16;
/**
 * A 32 bit unsigned integer
 */
typedef uint32_t uint32;
/**
 * A 64 bit unsigned integer
 */
typedef uint64_t uint64;

// float and double are types

/**
 * A string
 */
typedef std::string String;

/**
 * A boolean array.
 */
typedef std::vector<uint8> BooleanArray;
typedef std::tr1::shared_ptr<BooleanArray> BooleanArrayPtr;
/* get is same is ubyte*/
typedef std::vector<uint8>::iterator BooleanArray_iterator;
typedef std::vector<uint8>::const_iterator BooleanArray_const_iterator;

/**
 * A byte array.
 */
typedef std::vector<int8> ByteArray;
typedef std::tr1::shared_ptr<ByteArray> ByteArrayPtr;
inline int8 * get(ByteArray &value)
{
    return &value[0];
}
inline int8 const * get(ByteArray const &value)
{
    return static_cast<int8 const *>(&value[0]);
}
inline int8 * get(ByteArrayPtr &value)
{
    return get(*value.get());
}
inline int8 const * get(ByteArrayPtr const &value)
{
    return get(*value.get());
}
inline ByteArray & getVector(ByteArrayPtr &value)
{
     return *value.get();
}
inline ByteArray const & getVector(ByteArrayPtr const &value)
{
     return *value.get();
}
typedef std::vector<int8>::iterator ByteArray_iterator;
typedef std::vector<int8>::const_iterator ByteArray_const_iterator;

/**
 * A short array.
 */
typedef std::vector<int16> ShortArray;
typedef std::tr1::shared_ptr<ShortArray> ShortArrayPtr;
inline int16 * get(ShortArray &value)
{
    return &value[0];
}
inline int16 const * get(ShortArray const &value)
{
    return static_cast<int16 const *>(&value[0]);
}
inline int16 * get(ShortArrayPtr &value)
{
    return get(*value.get());
}
inline int16 const * get(ShortArrayPtr const &value)
{
    return get(*value.get());
}
inline ShortArray & getVector(ShortArrayPtr &value)
{
     return *value.get();
}
inline ShortArray const & getVector(ShortArrayPtr const &value)
{
     return *value.get();
}
typedef std::vector<int16>::iterator ShortArray_iterator;
typedef std::vector<int16>::const_iterator ShortArray_const_iterator;

/**
 * A int array.
 */
typedef std::vector<int32> IntArray;
typedef std::tr1::shared_ptr<IntArray> IntArrayPtr;
inline int32 * get(IntArray &value)
{
    return &value[0];
}
inline int32 const * get(IntArray const &value)
{
    return static_cast<int32 const *>(&value[0]);
}
inline int32 * get(IntArrayPtr &value)
{
    return get(*value.get());
}
inline int32 const * get(IntArrayPtr const &value)
{
    return get(*value.get());
}
inline IntArray & getVector(IntArrayPtr &value)
{
     return *value.get();
}
inline IntArray const & getVector(IntArrayPtr const &value)
{
     return *value.get();
}
typedef std::vector<int32>::iterator IntArray_iterator;
typedef std::vector<int32>::const_iterator IntArray_const_iterator;

/**
 * A long array.
 */
typedef std::vector<int64> LongArray;
typedef std::tr1::shared_ptr<LongArray> LongArrayPtr;
inline int64 * get(LongArray &value)
{
    return &value[0];
}
inline int64 const * get(LongArray const &value)
{
    return static_cast<int64 const *>(&value[0]);
}
inline int64 * get(LongArrayPtr &value)
{
    return get(*value.get());
}
inline int64 const * get(LongArrayPtr const &value)
{
    return get(*value.get());
}
inline LongArray & getVector(LongArrayPtr &value)
{
     return *value.get();
}
inline LongArray const & getVector(LongArrayPtr const &value)
{
     return *value.get();
}
typedef std::vector<int64>::iterator LongArray_iterator;
typedef std::vector<int64>::const_iterator LongArray_const_iterator;

/**
 * An unsigned byte array.
 */
typedef std::vector<uint8> UByteArray;
typedef std::tr1::shared_ptr<UByteArray> UByteArrayPtr;
inline uint8 * get(UByteArray &value)
{
    return &value[0];
}
inline uint8 const * get(UByteArray const &value)
{
    return static_cast<uint8 const *>(&value[0]);
}
inline uint8 * get(UByteArrayPtr &value)
{
    return get(*value.get());
}
inline uint8 const * get(UByteArrayPtr const &value)
{
    return get(*value.get());
}
inline UByteArray & getVector(UByteArrayPtr &value)
{
     return *value.get();
}
inline UByteArray const & getVector(UByteArrayPtr const &value)
{
     return *value.get();
}
typedef std::vector<uint8>::iterator UByteArray_iterator;
typedef std::vector<uint8>::const_iterator UByteArray_const_iterator;

/**
 * An unsigned short array.
 */
typedef std::vector<uint16> UShortArray;
typedef std::tr1::shared_ptr<UShortArray> UShortArrayPtr;
inline uint16 * get(UShortArray &value)
{
    return &value[0];
}
inline uint16 const * get(UShortArray const &value)
{
    return static_cast<uint16 const *>(&value[0]);
}
inline uint16 * get(UShortArrayPtr &value)
{
    return get(*value.get());
}
inline uint16 const * get(UShortArrayPtr const &value)
{
    return get(*value.get());
}
inline UShortArray & getVector(UShortArrayPtr &value)
{
     return *value.get();
}
inline UShortArray const & getVector(UShortArrayPtr const &value)
{
     return *value.get();
}
typedef std::vector<uint16>::iterator UShortArray_iterator;
typedef std::vector<uint16>::const_iterator UShortArray_const_iterator;

/**
 * An unsigned int array.
 */
typedef std::vector<uint32> UIntArray;
typedef std::tr1::shared_ptr<UIntArray> UIntArrayPtr;
inline uint32 * get(UIntArray &value)
{
    return &value[0];
}
inline uint32 const * get(UIntArray const &value)
{
    return static_cast<uint32 const *>(&value[0]);
}
inline uint32 * get(UIntArrayPtr &value)
{
    return get(*value.get());
}
inline uint32 const * get(UIntArrayPtr const &value)
{
    return get(*value.get());
}
inline UIntArray & getVector(UIntArrayPtr &value)
{
     return *value.get();
}
inline UIntArray const & getVector(UIntArrayPtr const &value)
{
     return *value.get();
}
typedef std::vector<uint32>::iterator UIntArray_iterator;
typedef std::vector<uint32>::const_iterator UIntArray_const_iterator;

/**
 * An unsigned long array.
 */
typedef std::vector<uint64> ULongArray;
typedef std::tr1::shared_ptr<ULongArray> ULongArrayPtr;
inline uint64 * get(ULongArray &value)
{
    return &value[0];
}
inline uint64 const * get(ULongArray const &value)
{
    return static_cast<uint64 const *>(&value[0]);
}
inline uint64 * get(ULongArrayPtr &value)
{
    return get(*value.get());
}
inline uint64 const * get(ULongArrayPtr const &value)
{
    return get(*value.get());
}
inline ULongArray & getVector(ULongArrayPtr &value)
{
     return *value.get();
}
inline ULongArray const & getVector(ULongArrayPtr const &value)
{
     return *value.get();
}
typedef std::vector<uint64>::iterator ULongArray_iterator;
typedef std::vector<uint64>::const_iterator ULongArray_const_iterator;

/**
 * A float array.
 */
typedef std::vector<float> FloatArray;
typedef std::tr1::shared_ptr<FloatArray> FloatArrayPtr;
inline float * get(FloatArray &value)
{
    return &value[0];
}
inline float const * get(FloatArray const &value)
{
    return static_cast<float const *>(&value[0]);
}
inline float * get(FloatArrayPtr &value)
{
    return get(*value.get());
}
inline float const * get(FloatArrayPtr const &value)
{
    return get(*value.get());
}
inline FloatArray & getVector(FloatArrayPtr &value)
{
     return *value.get();
}
inline FloatArray const & getVector(FloatArrayPtr const &value)
{
     return *value.get();
}
typedef std::vector<float>::iterator FloatArray_iterator;
typedef std::vector<float>::const_iterator FloatArray_const_iterator;

/**
 * A double array.
 */
typedef std::vector<double> DoubleArray;
typedef std::tr1::shared_ptr<DoubleArray> DoubleArrayPtr;
inline double * get(DoubleArray &value)
{
    return &value[0];
}
inline double const * get(DoubleArray const &value)
{
    return static_cast<double const *>(&value[0]);
}
inline double * get(DoubleArrayPtr &value)
{
    return get(*value.get());
}
inline double const * get(DoubleArrayPtr const &value)
{
    return get(*value.get());
}
inline DoubleArray & getVector(DoubleArrayPtr &value)
{
     return *value.get();
}
inline DoubleArray const & getVector(DoubleArrayPtr const &value)
{
     return *value.get();
}
typedef std::vector<double>::iterator DoubleArray_iterator;
typedef std::vector<double>::const_iterator DoubleArray_const_iterator;

/**
 * A string array.
 */
typedef std::vector<String> StringArray;
typedef std::tr1::shared_ptr<StringArray> StringArrayPtr;
inline String * get(StringArray &value)
{
    return &value[0];
}
inline String const * get(StringArray const &value)
{
    return static_cast<String const *>(&value[0]);
}
inline String * get(StringArrayPtr &value)
{
    return get(*value.get());
}
inline String const * get(StringArrayPtr const &value)
{
    return get(*value.get());
}
inline StringArray & getVector(StringArrayPtr &value)
{
     return *value.get();
}
inline StringArray const & getVector(StringArrayPtr const &value)
{
     return *value.get();
}
typedef std::vector<String>::iterator StringArray_iterator;
typedef std::vector<String>::const_iterator StringArray_const_iterator;

/**
 * A convenience definition for toString methods
 */
typedef String * StringBuilder;

}}
#endif  /* PVTYPE_H */



