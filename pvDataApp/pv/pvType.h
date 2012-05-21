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
#include <stdint.h>

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
/* get is same is UInt */
typedef std::vector<uint8>::iterator BooleanArray_iterator;
typedef std::vector<uint8>::const_iterator BooleanArray_const_iterator;
/**
 * A byte array.
 */
typedef std::vector<int8> ByteArray;
inline int8 * get(ByteArray &value)
{
    return &value[0];
}
inline const int8 * get(const ByteArray &value)
{
    return static_cast<const int8 *>(&value[0]);
}
typedef std::vector<int8>::iterator ByteArray_iterator;
typedef std::vector<int8>::const_iterator ByteArray_const_iterator;
/**
 * A short array.
 */
typedef std::vector<int16> ShortArray;
inline int16 * get(ShortArray &value)
{
    return &value[0];
}
inline const int16 * get(const ShortArray &value)
{
    return static_cast<const int16 *>(&value[0]);
}
typedef std::vector<int16>::iterator ShortArray_iterator;
typedef std::vector<int16>::const_iterator ShortArray_const_iterator;
/**
 * A int array.
 */
typedef std::vector<int32> IntArray;
inline int32 * get(IntArray &value)
{
    return &value[0];
}
inline const int32 * get(const IntArray &value)
{
    return static_cast<const int32 *>(&value[0]);
}
typedef std::vector<int32>::iterator IntArray_iterator;
typedef std::vector<int32>::const_iterator IntArray_const_iterator;
/**
 * A long array.
 */
typedef std::vector<int64> LongArray;
inline int64 * get(LongArray &value)
{
    return &value[0];
}
inline const int64 * get(const LongArray &value)
{
    return static_cast<const int64 *>(&value[0]);
}
typedef std::vector<int64>::iterator LongArray_iterator;
typedef std::vector<int64>::const_iterator LongArray_const_iterator;
/**
 * An unsigned byte array.
 */
typedef std::vector<uint8> UByteArray;
inline uint8 * get(UByteArray &value)
{
    return &value[0];
}
inline const uint8 * get(const UByteArray &value)
{
    return static_cast<const uint8 *>(&value[0]);
}
typedef std::vector<uint8>::iterator UByteArray_iterator;
typedef std::vector<uint8>::const_iterator UByteArray_const_iterator;
/**
 * An unsigned short array.
 */
typedef std::vector<uint16> UShortArray;
inline uint16 * get(UShortArray &value)
{
    return &value[0];
}
inline const uint16 * get(const UShortArray &value)
{
    return static_cast<const uint16 *>(&value[0]);
}
typedef std::vector<uint16>::iterator UShortArray_iterator;
typedef std::vector<uint16>::const_iterator UShortArray_const_iterator;
/**
 * An unsigned int array.
 */
typedef std::vector<uint32> UIntArray;
inline uint32 * get(UIntArray &value)
{
    return &value[0];
}
inline const uint32 * get(const UIntArray &value)
{
    return static_cast<const uint32 *>(&value[0]);
}
typedef std::vector<uint32>::iterator UIntArray_iterator;
typedef std::vector<uint32>::const_iterator UIntArray_const_iterator;
/**
 * An unsigned long array.
 */
typedef std::vector<uint64> ULongArray;
inline uint64 * get(ULongArray &value)
{
    return &value[0];
}
inline const uint64 * get(const ULongArray &value)
{
    return static_cast<const uint64 *>(&value[0]);
}
typedef std::vector<uint64>::iterator ULongArray_iterator;
typedef std::vector<uint64>::const_iterator ULongArray_const_iterator;
/**
 * A float array.
 */
typedef std::vector<float> FloatArray;
inline float * get(FloatArray &value)
{
    return &value[0];
}
inline const float * get(const FloatArray &value)
{
    return static_cast<const float *>(&value[0]);
}
typedef std::vector<float>::iterator FloatArray_iterator;
typedef std::vector<float>::const_iterator FloatArray_const_iterator;
/**
 * A double array.
 */
typedef std::vector<double> DoubleArray;
inline double * get(DoubleArray &value)
{
    return &value[0];
}
inline const double * get(const DoubleArray &value)
{
    return static_cast<const double *>(&value[0]);
}
typedef std::vector<double>::iterator DoubleArray_iterator;
typedef std::vector<double>::const_iterator DoubleArray_const_iterator;
/**
 * A string array.
 */
typedef std::vector<String> StringArray;
inline String * get(StringArray &value)
{
    return &value[0];
}
inline const String * get(const StringArray &value)
{
    return static_cast<const String *>(&value[0]);
}
typedef std::vector<String>::iterator StringArray_iterator;
typedef std::vector<String>::const_iterator StringArray_const_iterator;

/**
 * A convenience definition for toString methods
 */
typedef String * StringBuilder;

}}
#endif  /* PVTYPE_H */



