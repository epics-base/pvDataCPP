/* pvType.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */

/* Definitions for the primitive types for pvData.
 * It also defines the arrays of the primitive types
 */

#ifndef PVTYPE_H
#define PVTYPE_H
#include <string>
#include <stdint.h>

namespace epics { namespace pvData { 

typedef bool     boolean;
typedef int8_t   int8;
typedef int16_t  int16;
typedef int32_t  int32;
typedef int64_t  int64;
typedef uint32_t uint32;
typedef uint64_t uint64;
// float and double are types

typedef std::string String;

typedef bool * BooleanArray;
typedef int8 * ByteArray;
typedef int16 * ShortArray;
typedef int32 * IntArray;
typedef int64 * LongArray;
typedef float * FloatArray;
typedef double * DoubleArray;
typedef String* StringArray;

// convenience definition for toString methods
typedef std::string * StringBuilder;

}}
#endif  /* PVTYPE_H */



