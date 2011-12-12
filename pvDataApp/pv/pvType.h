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
#include <stdint.h>

namespace epics { namespace pvData { 

/**
 * This is a set of typdefs used by pvData.
 */

/**
 * boolean, i.e. can only have the values {@code false} or {@code true}
 */
typedef bool     boolean;
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
typedef bool * BooleanArray;
/**
 * A byte array.
 */
typedef int8 * ByteArray;
/**
 * A short array.
 */
typedef int16 * ShortArray;
/**
 * A int array.
 */
typedef int32 * IntArray;
/**
 * A long array.
 */
typedef int64 * LongArray;
/**
 * A float array.
 */
typedef float * FloatArray;
/**
 * A double array.
 */
typedef double * DoubleArray;
/**
 * A string array.
 */
typedef String* StringArray;

/**
 * A convenience definition for toString methods
 */
typedef std::string * StringBuilder;

}}
#endif  /* PVTYPE_H */



