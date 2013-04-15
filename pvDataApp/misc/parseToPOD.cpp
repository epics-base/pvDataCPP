#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <float.h>

#include <epicsVersion.h>

#include <epicsMath.h>
#include <epicsStdlib.h>
#include <epicsString.h>
#include <epicsConvert.h>

#include "typeCast.h"

#ifndef EPICS_VERSION_INT
#define VERSION_INT(V,R,M,P) ( ((V)<<24) | ((R)<<16) | ((M)<<8) | (P))
#define EPICS_VERSION_INT VERSION_INT(EPICS_VERSION, EPICS_REVISION, EPICS_MODIFICATION, EPICS_PATCH_LEVEL)
#endif

#if EPICS_VERSION_INT < VERSION_INT(3,15,0,1)
/* integer conversion primatives added to epicsStdlib.c in 3.15.0.1 */

#define S_stdlib_noConversion 1 /* No digits to convert */
#define S_stdlib_extraneous   2 /* Extraneous characters */
#define S_stdlib_underflow    3 /* Too small to represent */
#define S_stdlib_overflow     4 /* Too large to represent */
#define S_stdlib_badBase      5 /* Number base not supported */

static int
epicsParseLong(const char *str, long *to, int base, char **units)
{
    int c;
    char *endp;
    long value;

    while ((c = *str) && isspace(c))
        ++str;

    errno = 0;
    value = strtol(str, &endp, base);

    if (endp == str)
        return S_stdlib_noConversion;
    if (errno == EINVAL)    /* Not universally supported */
        return S_stdlib_badBase;
    if (errno == ERANGE)
        return S_stdlib_overflow;

    while ((c = *endp) && isspace(c))
        ++endp;
    if (c && !units)
        return S_stdlib_extraneous;

    *to = value;
    if (units)
        *units = endp;
    return 0;
}

static int
epicsParseULong(const char *str, unsigned long *to, int base, char **units)
{
    int c;
    char *endp;
    unsigned long value;

    while ((c = *str) && isspace(c))
        ++str;

    errno = 0;
    value = strtoul(str, &endp, base);

    if (endp == str)
        return S_stdlib_noConversion;
    if (errno == EINVAL)    /* Not universally supported */
        return S_stdlib_badBase;
    if (errno == ERANGE)
        return S_stdlib_overflow;

    while ((c = *endp) && isspace(c))
        ++endp;
    if (c && !units)
        return S_stdlib_extraneous;

    *to = value;
    if (units)
        *units = endp;
    return 0;
}

static int
epicsParseDouble(const char *str, double *to, char **units)
{
    int c;
    char *endp;
    double value;

    while ((c = *str) && isspace(c))
        ++str;

    errno = 0;
    value = epicsStrtod(str, &endp);

    if (endp == str)
        return S_stdlib_noConversion;
    if (errno == ERANGE)
        return (value == 0) ? S_stdlib_underflow : S_stdlib_overflow;

    while ((c = *endp) && isspace(c))
        ++endp;
    if (c && !units)
        return S_stdlib_extraneous;

    *to = value;
    if (units)
        *units = endp;
    return 0;
}


/* These call the primitives */

static int
epicsParseInt8(const char *str, epicsInt8 *to, int base, char **units)
{
    long value;
    int status = epicsParseLong(str, &value, base, units);

    if (status)
        return status;

    if (value < -0x80 || value > 0x7f)
        return S_stdlib_overflow;

    *to = value;
    return 0;
}

static int
epicsParseUInt8(const char *str, epicsUInt8 *to, int base, char **units)
{
    unsigned long value;
    int status = epicsParseULong(str, &value, base, units);

    if (status)
        return status;

    if (value > 0xff && value <= ~0xffUL)
        return S_stdlib_overflow;

    *to = value;
    return 0;
}

static int
epicsParseInt16(const char *str, epicsInt16 *to, int base, char **units)
{
    long value;
    int status = epicsParseLong(str, &value, base, units);

    if (status)
        return status;

    if (value < -0x8000 || value > 0x7fff)
        return S_stdlib_overflow;

    *to = value;
    return 0;
}

static int
epicsParseUInt16(const char *str, epicsUInt16 *to, int base, char **units)
{
    unsigned long value;
    int status = epicsParseULong(str, &value, base, units);

    if (status)
        return status;

    if (value > 0xffff && value <= ~0xffffUL)
        return S_stdlib_overflow;

    *to = value;
    return 0;
}

static int
epicsParseInt32(const char *str, epicsInt32 *to, int base, char **units)
{
    long value;
    int status = epicsParseLong(str, &value, base, units);

    if (status)
        return status;

#if (LONG_MAX > 0x7fffffff)
    if (value < -0x80000000L || value > 0x7fffffffL)
        return S_stdlib_overflow;
#endif

    *to = value;
    return 0;
}

static int
epicsParseUInt32(const char *str, epicsUInt32 *to, int base, char **units)
{
    unsigned long value;
    int status = epicsParseULong(str, &value, base, units);

    if (status)
        return status;

#if (ULONG_MAX > 0xffffffff)
    if (value > 0xffffffffUL && value <= ~0xffffffffUL)
        return S_stdlib_overflow;
#endif

    *to = value;
    return 0;
}

static int
epicsParseFloat(const char *str, float *to, char **units)
{
    double value, abs;
    int status = epicsParseDouble(str, &value, units);

    if (status)
        return status;

    abs = fabs(value);
    if (value > 0 && abs <= FLT_MIN)
        return S_stdlib_underflow;
    if (finite(value) && abs >= FLT_MAX)
        return S_stdlib_overflow;

    *to = value;
    return 0;
}
#endif

/* do we need long long? */
#if INT_MAX == LONG_MAX
static int
epicsParseLongLong(const char *str, long long *to, int base, char **units)
{
    int c;
    char *endp;
    long long value;

    while ((c = *str) && isspace(c))
        ++str;

    errno = 0;
    value = strtoll(str, &endp, base);

    if (endp == str)
        return S_stdlib_noConversion;
    if (errno == EINVAL)    /* Not universally supported */
        return S_stdlib_badBase;
    if (errno == ERANGE)
        return S_stdlib_overflow;

    while ((c = *endp) && isspace(c))
        ++endp;
    if (c && !units)
        return S_stdlib_extraneous;

    *to = value;
    if (units)
        *units = endp;
    return 0;
}

static int
epicsParseULongLong(const char *str, unsigned long long *to, int base, char **units)
{
    int c;
    char *endp;
    unsigned long long value;

    while ((c = *str) && isspace(c))
        ++str;

    errno = 0;
    value = strtoull(str, &endp, base);

    if (endp == str)
        return S_stdlib_noConversion;
    if (errno == EINVAL)    /* Not universally supported */
        return S_stdlib_badBase;
    if (errno == ERANGE)
        return S_stdlib_overflow;

    while ((c = *endp) && isspace(c))
        ++endp;
    if (c && !units)
        return S_stdlib_extraneous;

    *to = value;
    if (units)
        *units = endp;
    return 0;
}
#endif

static
void handleParseError(int err)
{
    switch(err) {
    case 0: break;
    case S_stdlib_noConversion: throw std::runtime_error("parseToPOD: No digits to convert");
    case S_stdlib_extraneous: throw std::runtime_error("parseToPOD: Extraneous characters");
    case S_stdlib_underflow: throw std::runtime_error("parseToPOD: Too small to represent");
    case S_stdlib_overflow: throw std::runtime_error("parseToPOD: Too large to represent");
    case S_stdlib_badBase: throw std::runtime_error("parseToPOD: Number base not supported");
    default:
        throw std::runtime_error("parseToPOD: unknown error");
    }
}

namespace epics { namespace pvData { namespace detail {

void parseToPOD(const std::string& in, int8 *out) {
    epicsInt8 temp;
    int err = epicsParseInt8(in.c_str(), &temp, 0, NULL);
    if(err)   handleParseError(err);
    else      *out = temp;
}

#define INTFN(T, S) \
void parseToPOD(const std::string& in, T *out) { \
    int err = epicsParse ## S(in.c_str(), out, 0, NULL); \
    if(err)   handleParseError(err); \
}

INTFN(char, Int8);
INTFN(uint8_t, UInt8);
INTFN(int16_t, Int16);
INTFN(uint16_t, UInt16);
INTFN(int32_t, Int32);
INTFN(uint32_t, UInt32);

void parseToPOD(const std::string& in, int64_t *out) {
#if INT_MAX == LONG_MAX
    int err = epicsParseLongLong(in.c_str(), out, 0, NULL);
#else
    int err = epicsParseLong(in.c_str(), out, 0, NULL);
#endif
    if(err)   handleParseError(err);
}

void parseToPOD(const std::string& in, uint64_t *out) {
#if INT_MAX == LONG_MAX
    int err = epicsParseULongLong(in.c_str(), out, 0, NULL);
#else
    int err = epicsParseULong(in.c_str(), out, 0, NULL);
#endif
    if(err)   handleParseError(err);
}

void parseToPOD(const std::string& in, float *out) {
    int err = epicsParseFloat(in.c_str(), out, NULL);
    if(err)   handleParseError(err);
}

void parseToPOD(const std::string& in, double *out) {
    int err = epicsParseDouble(in.c_str(), out, NULL);
    if(err)   handleParseError(err);
}

}}}
