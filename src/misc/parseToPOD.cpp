/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <float.h>
#include <limits.h>

#include <epicsVersion.h>

#include <epicsMath.h>
#include <epicsStdlib.h>
#include <epicsString.h>
#include <epicsConvert.h>

#define epicsExportSharedSymbols
#include "pv/typeCast.h"

using std::string;

// need to use "long long" when sizeof(int)==sizeof(long)
#if (ULONG_MAX == 0xfffffffful) || defined(_WIN32) || defined(__rtems__) || defined(__APPLE__)
#define NEED_LONGLONG
#endif

#ifndef EPICS_VERSION_INT
#define VERSION_INT(V,R,M,P) ( ((V)<<24) | ((R)<<16) | ((M)<<8) | (P))
#define EPICS_VERSION_INT VERSION_INT(EPICS_VERSION, EPICS_REVISION, EPICS_MODIFICATION, EPICS_PATCH_LEVEL)
#endif

#if EPICS_VERSION_INT < VERSION_INT(3,15,0,1)
/* These integer conversion primitives added to epicsStdlib.c in 3.15.0.1 */

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

    *to = (epicsInt8)value;
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

    *to = (epicsUInt8)value;
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

    *to = (epicsInt16)value;
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

    *to = (epicsUInt16)value;
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

    *to = (epicsInt32)value;
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

    *to = (epicsUInt32)value;
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

    *to = (float)value;
    return 0;
}
#endif

// Sometimes we have to provide our own copy of strtoll()
#if defined(_MSC_VER) && _MSC_VER < 1800
// On Windows with MSVC, Base-3.15 provides strtoll()
#    define NEED_OLL_FUNCS (EPICS_VERSION_INT < VERSION_INT(3,15,0,1))
#elif defined(vxWorks)
// On VxWorks, Base-3.15 provides strtoll()
#    define NEED_OLL_FUNCS (EPICS_VERSION_INT < VERSION_INT(3,15,0,1))
#else
// Other architectures all provide strtoll()
#    define NEED_OLL_FUNCS 0
#endif

#if defined(NEED_LONGLONG) && NEED_OLL_FUNCS
static
long long strtoll(const char *ptr, char ** endp, int base)
{
    size_t inlen = strlen(ptr);
    long long result;
    unsigned char offset=0;
    
    assert(base==0);

    if(ptr[0]=='-')
        offset=1;

    try {
        std::istringstream strm(ptr);

        assert(strm.rdbuf()->in_avail()>=0
               && inlen==(size_t)strm.rdbuf()->in_avail());

        if(ptr[offset]=='0') {
            if(ptr[offset+1]=='x')
                strm >> std::hex;
            else
                strm >> std::oct;
        }

        strm >> result;
        if(strm.fail())
            goto noconvert;

        assert(strm.rdbuf()->in_avail()>=0
               && inlen>=(size_t)strm.rdbuf()->in_avail());

        size_t consumed = inlen - strm.rdbuf()->in_avail();
        *endp = (char*)ptr + consumed;

        return result;
        
    } catch(...) {
        goto noconvert;
    }

    return result;
noconvert:
    *endp = (char*)ptr;
    return 0;
}

#if defined(vxWorks)
/* The VxWorks version of std::istringstream >> uint64_t is buggy,
 * provide our own implementation
 */
static
unsigned long long strtoull(const char *nptr, char **endptr, int base)
{
  const char *s = nptr;
  unsigned long long acc;
  int c;
  unsigned long long cutoff;
  int neg = 0, any, cutlim;

  do
    c = *s++;
  while (isspace(c));
  if (c == '-')
    {
      neg = 1;
      c = *s++;
    }
  else if (c == '+')
    c = *s++;
  if ((base == 0 || base == 16) &&
      c == '0' && (*s == 'x' || *s == 'X'))
    {
      c = s[1];
      s += 2;
      base = 16;
    }
  if (base == 0)
    base = c == '0' ? 8 : 10;

  cutoff = (unsigned long long) UINT64_MAX / (unsigned long long) base;
  cutlim = (unsigned long long) UINT64_MAX % (unsigned long long) base;

  for (acc = 0, any = 0;; c = *s++)
    {
      if (isdigit(c))
        c -= '0';
      else if (isalpha(c))
        c -= isupper(c) ? 'A' - 10 : 'a' - 10;
      else
        break;
      if (c >= base)
        break;
      if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
        any = -1;
      else
        {
          any = 1;
          acc *= base;
          acc += c;
        }
    }
  if (any < 0)
    {
      acc = UINT64_MAX;
      errno = ERANGE;
    }
  else if (neg)
    acc = -acc;
  if (endptr != 0)
    *endptr = any ? (char *) s - 1 : (char *) nptr;
  return (acc);
}
#else
static
unsigned long long strtoull(const char *ptr, char ** endp, int base)
{
    size_t inlen = strlen(ptr);
    unsigned long long result;

    assert(base==0);

    try {
        std::istringstream strm(ptr);

        assert(strm.rdbuf()->in_avail()>=0
               && inlen==(size_t)strm.rdbuf()->in_avail());

        if(ptr[0]=='0') {
            if(ptr[1]=='x')
                strm >> std::hex;
            else
                strm >> std::oct;
        }

        strm >> result;
        if(strm.fail())
            goto noconvert;

        assert(strm.rdbuf()->in_avail()>=0
               && inlen>=(size_t)strm.rdbuf()->in_avail());

        size_t consumed = inlen - strm.rdbuf()->in_avail();
        *endp = (char*)ptr + consumed;

        return result;

    } catch(...) {
        goto noconvert;
    }

    return result;
noconvert:
    *endp = (char*)ptr;
    return 0;
}
#endif
#endif

/* do we need long long? */
#ifdef NEED_LONGLONG
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

void parseToPOD(const char* in, boolean *out)
{
    if(epicsStrCaseCmp(in,"true")==0)
        *out = 1;
    else if(epicsStrCaseCmp(in,"false")==0)
        *out = 0;
    else
        throw std::runtime_error("parseToPOD: string no match true/false");
}

#define INTFN(T, S) \
void parseToPOD(const char* in, T *out) { \
    epics ## S temp; \
    int err = epicsParse ## S (in, &temp, 0, NULL); \
    if(err)   handleParseError(err); \
    else      *out = temp; \
}

INTFN(int8, Int8);
INTFN(uint8, UInt8);
INTFN(int16_t, Int16);
INTFN(uint16_t, UInt16);
INTFN(int32_t, Int32);
INTFN(uint32_t, UInt32);

void parseToPOD(const char* in, int64_t *out) {
#ifdef NEED_LONGLONG
    int err = epicsParseLongLong(in, out, 0, NULL);
#else
    int err = epicsParseLong(in, out, 0, NULL);
#endif
    if(err)   handleParseError(err);
}

void parseToPOD(const char* in, uint64_t *out) {
#ifdef NEED_LONGLONG
    int err = epicsParseULongLong(in, out, 0, NULL);
#else
    int err = epicsParseULong(in, out, 0, NULL);
#endif
    if(err)   handleParseError(err);
}

void parseToPOD(const char* in, float *out) {
    int err = epicsParseFloat(in, out, NULL);
    if(err)   handleParseError(err);
}

void parseToPOD(const char* in, double *out) {
    int err = epicsParseDouble(in, out, NULL);
    if(err)   handleParseError(err);
#if defined(vxWorks)
    /* vxWorks strtod returns [-]epicsINF when it should return ERANGE error.
     * If [-]epicsINF is returned and the first char is a digit we translate
     * this into an ERANGE error
     */
    else if (*out == epicsINF || *out == -epicsINF) {
        const char* s = in;
        int c;

        /* skip spaces and the sign */
        do {
            c = *s++;
        } while (isspace(c));

        if (c == '-' || c == '+')
            c = *s++;

        if (isdigit(c))
            handleParseError(S_stdlib_overflow);
    }
#endif
}

}}}
