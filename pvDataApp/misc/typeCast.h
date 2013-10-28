/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/* Author:  Michael Davidsaver */
#ifndef PVTYPECAST_H
#define PVTYPECAST_H

#include <stdexcept>
#include <sstream>

#include <epicsConvert.h>

#include <pv/pvType.h>
#include <pv/pvIntrospect.h>
#include <pv/templateMeta.h>

namespace epics { namespace pvData {

typedef std::string String;

namespace detail {
    // parseToPOD wraps the epicsParse*() functions in one name
    // and throws exceptions
    void parseToPOD(const std::string&, char *out);
    void parseToPOD(const std::string&, int8_t *out);
    void parseToPOD(const std::string&, uint8_t *out);
    void parseToPOD(const std::string&, int16_t *out);
    void parseToPOD(const std::string&, uint16_t *out);
    void parseToPOD(const std::string&, int32_t *out);
    void parseToPOD(const std::string&, uint32_t *out);
    void parseToPOD(const std::string&, int64_t *out);
    void parseToPOD(const std::string&, uint64_t *out);
    void parseToPOD(const std::string&, float *out);
    void parseToPOD(const std::string&, double *out);

    /* want to pass POD types by value,
     * and String by const reference
     */
    template<typename ARG>
    struct cast_arg { typedef ARG arg; };
    template<>
    struct cast_arg<String> { typedef const String& arg; };

    // trick std::ostream into treating char's as numbers
    // by promoting char to int
    template<typename T>
    struct print_cast { typedef T type; };
    template<>
    struct print_cast<char> { typedef int type; };
    template<>
    struct print_cast<signed char> { typedef signed int type; };
    template<>
    struct print_cast<unsigned char> { typedef unsigned int type; };

    // default to C++ type casting
    template<typename TO, typename FROM, class Enable = void>
    struct cast_helper {
        static FORCE_INLINE TO op(FROM from) {
            return static_cast<TO>(from);
        }
    };

    // special handling when down-casting double to float
    template<>
    struct cast_helper<float, double> {
        static FORCE_INLINE float op(double from) {
            return epicsConvertDoubleToFloat(from);
        }
    };

    // print POD to string
    // when String!=FROM
    template<typename FROM>
    struct cast_helper<String, FROM, typename meta::not_same_type<String,FROM>::type> {
        static String op(FROM from) {
            typedef typename print_cast<FROM>::type ptype;
            std::ostringstream strm;
            strm << (ptype)from;
            if(strm.fail())
                throw std::runtime_error("Cast to string failed");
            return strm.str();
        }
    };

    // parse POD from string
    // TO!=String
    template<typename TO>
    struct cast_helper<TO, String, typename meta::not_same_type<TO,String>::type> {
        static FORCE_INLINE TO op(const String& from) {
            TO ret;
            parseToPOD(from, &ret);
            return ret;
        }
    };

} // end detail

/** @brief Casting/converting between supported scalar types.
 *
 * Supported types: uint8_t, int8_t, uint16_t, int16_t,
 *                  uint32_t, int32_t, uint64_t, int64_t,
 *                  float, double, String
 *
 * As defined in pvType.h
 *
 @throws std::runtime_error when the cast is not possible.
 @throws std::bad_alloc when the cast is not possible.
 *
 @section convertg Conversion Guarantees
 *
 * Conversions which always produce a correct result.
 *
 * - signed integer -> larger signed integer
 * - unsigned integer -> larger unsigned integer
 * - integer -> float or double (where sizeof(integer)<sizeof(floating))
 * - float -> double
 *
 * Conversions where out of range inputs always produce
 * a defined result, but may not be reversable.
 *
 * - double -> float.  When abs(value) is outside the range
 *    [FLT_MIN, FLT_MAX] the value is clipped to FLT_MIN or FLT_MAX
 *    with the sign preserved.
 *
 * Conversions where invalid or out of range inputs result
 * in an exception.
 *
 * - non-String -> String
 * - String -> non-String
 * - String -> String (throws only std::bad_alloc)
 *
 * Conversions where out of range inputs produce undefined
 * results.
 *
 * - signed integer -> smaller signed integer
 * - unsigned integer -> smaller unsigned integer
 * - signed integer <-> unsigned integer
 * - integer -> float or double (where sizeof(integer)>=sizeof(floating))
 * - float or double -> integer.  The floating point value
 *    is rounded towards zero.  However, the result for values
 *    too large to be represented by the integer type
 *    is not defined.
 *
 @section stringf String formats
 *
 * - Numbers beginning with 1-9 are parsed as base-10.
 * - Numbers beginning with '0x' are parsed as base-16
 * - Numbers beginning with '0' are parsed as base-8.
 * - Hex numbers are case insensitive.
 * - Exponential numbers may use either 'e' or 'E'.
 */
template<typename TO, typename FROM>
static FORCE_INLINE TO castUnsafe(const FROM& from)
{
    return detail::cast_helper<TO,FROM>::op(from);
}

void castUnsafeV(size_t count, ScalarType to, void *dest, ScalarType from, const void *src);

//! Cast value to printable type
//! A no-op except for char types, which are cast to int
//! so that they are printed as numbers std::ostream operators.
template<typename T>
static FORCE_INLINE
typename detail::print_cast<T>::type
print_cast(const T& v) { return v; }

}} // end namespace

#endif // PVTYPECAST_H
