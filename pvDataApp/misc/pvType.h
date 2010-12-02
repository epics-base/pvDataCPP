/* pvType.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <string>
#ifndef PVTYPE_H
#define PVTYPE_H

namespace epics { namespace pvData { 

typedef signed char int8;
typedef short       int16;
typedef int         int32;
typedef long long   int64;
typedef unsigned int uint32;
typedef unsigned long long uint64;

typedef std::string String;
typedef std::string * StringBuilder;
typedef String* StringArray;

}}
#endif  /* PVTYPE_H */



