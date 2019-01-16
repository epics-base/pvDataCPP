/* pvType.h */
/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */
/**
 *  @author mrk
 */

/* Definitions for the primitive types for pvData.
 * It also defines the arrays of the primitive types
 */

#ifndef PVTYPE_H
#define PVTYPE_H

#if defined(_WIN32) && !defined(_MINGW)
#pragma warning( push )
#pragma warning(disable: 4251)
#endif

#include <string>
#include <vector>

#if defined(vxWorks) && \
    (_WRS_VXWORKS_MAJOR+0 <= 6) && (_WRS_VXWORKS_MINOR+0 < 9)
typedef int intptr_t;
typedef unsigned int uintptr_t;
#ifndef INT64_MAX
#define INT64_MAX (0x7fffffffffffffffLL)
#define UINT64_MAX (0xffffffffffffffffULL)
#endif
#elif _MSC_VER==1500
#include <epicsTypes.h>
typedef epicsUInt8 uint8_t;
typedef epicsInt8 int8_t;
typedef epicsUInt16 uint16_t;
typedef epicsInt16 int16_t;
typedef epicsUInt32 uint32_t;
typedef epicsInt32 int32_t;
typedef epicsUInt64 uint64_t;
typedef epicsInt64 int64_t;
#else
#include <stdint.h>
#endif

#include <shareLib.h>

#include <pv/sharedPtr.h>

/* C++11 keywords
 @code
 struct Base {
   virtual void foo();
 };
 struct Class : public Base {
   virtual void foo() OVERRIDE FINAL;
 };
 @endcode
 */
#ifndef FINAL
#  if __cplusplus>=201103L
#    define FINAL final
#  else
#    define FINAL
#  endif
#endif
#ifndef OVERRIDE
#  if __cplusplus>=201103L
#    define OVERRIDE override
#  else
#    define OVERRIDE
#  endif
#endif

//! epics
namespace epics {
//! pvData
namespace pvData {

namespace detail {
    // Pick either type If or type Else to not be Cond
    template<typename Cond, typename If, typename Else>
    struct pick_type { typedef If type; };
    template<typename Cond, typename Else>
    struct pick_type<Cond,Cond,Else> { typedef Else type; };
}

/**
 * This is a set of typedefs used by pvData.
 */

/**
 * boolean, i.e. can only have the values @c false or @c true
 */
typedef detail::pick_type<int8_t, signed char,
                          detail::pick_type<uint8_t, char, unsigned char>::type
                          >::type boolean;
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
 * A string array.
 */
typedef std::vector<std::string> StringArray;
typedef std::tr1::shared_ptr<StringArray> StringArrayPtr;
inline std::string * get(StringArray &value)
{
    return &value[0];
}
inline std::string const * get(StringArray const &value)
{
    return static_cast<std::string const *>(&value[0]);
}
inline std::string * get(StringArrayPtr &value)
{
    return get(*value.get());
}
inline std::string const * get(StringArrayPtr const &value)
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
typedef std::vector<std::string>::iterator StringArray_iterator;
typedef std::vector<std::string>::const_iterator StringArray_const_iterator;

/**
 * Definition of support field types.
 */
enum Type {
    /**
     * The type is scalar. It has a scalarType
     */
    scalar,
    /**
     * The type is scalarArray. Each element is a scalar of the same scalarType.
     */
    scalarArray,
    /**
     * The type is structure.
     */
    structure,
    /**
     * The type is structureArray. Each element is a structure.
     */
    structureArray,
    /**
     * The type is an union.
     */
    union_,
    /**
     * The type is an array of unions.
     */
    unionArray
};

/**
 * @brief Convenience functions for Type.
 *
 */
namespace TypeFunc {
    /**
     * Get a name for the type.
     * @param  type The type.
     * @return The name for the type.
     */
    epicsShareExtern const char* name(Type type);
};

epicsShareExtern std::ostream& operator<<(std::ostream& o, const Type& type);


/**
 * Definition of support scalar types.
 */
enum ScalarType {
    /**
     * The type is boolean, i.e. value can be @c false or @c true
     */
    pvBoolean,
    /**
     * The type is byte, i.e. a 8 bit signed integer.
     */
    pvByte,
    /**
     * The type is short, i.e. a 16 bit signed integer.
     */
    pvShort,
    /**
     * The type is int, i.e. a 32 bit signed integer.
     */
    pvInt,
    /**
     * The type is long, i.e. a 64 bit signed integer.
     */
    pvLong,
    /**
     * The type is unsigned byte, i.e. a 8 bit unsigned integer.
     */
    pvUByte,
    /**
     * The type is unsigned short, i.e. a 16 bit unsigned integer.
     */
    pvUShort,
    /**
     * The type is unsigned int, i.e. a 32 bit unsigned integer.
     */
    pvUInt,
    /**
     * The type is unsigned long, i.e. a 64 bit unsigned integer.
     */
    pvULong,
    /**
     * The type is float, i.e. 32 bit IEEE floating point,
     */
    pvFloat,
    /**
     * The type is float, i.e. 64 bit IEEE floating point,
     */
    pvDouble,
    /**
     * The type is string, i.e. a UTF8 character string.
     */
    pvString
};

#define MAX_SCALAR_TYPE pvString

/**
 * @brief Convenience functions for ScalarType.
 *
 */
namespace ScalarTypeFunc {
    /**
     * Is the type an integer, i.e. is it one of byte,...ulong
     * @param  scalarType The type.
     * @return (false,true) if the scalarType is an integer.
     */
    epicsShareExtern bool isInteger(ScalarType scalarType);
    /**
     * Is the type an unsigned integer, i.e. is it one of ubyte,...ulong
     * @param  scalarType The type.
     * @return (false,true) if the scalarType is an integer.
     */
    epicsShareExtern bool isUInteger(ScalarType scalarType);
    /**
     * Is the type numeric, i.e. is it one of byte,...,double
     * @param  scalarType The type.
     * @return (false,true) if the scalarType is a numeric
     */
    epicsShareExtern bool isNumeric(ScalarType scalarType);
    /**
     * Is the type primitive, i.e. not string
     * @param  scalarType The type.
     * @return (false,true) if the scalarType is primitive.
     */
    epicsShareExtern bool isPrimitive(ScalarType scalarType);
    /**
     * Get the scalarType for value.
     * @param  value The name of the scalar type.
     * @return The scalarType.
     * An exception is thrown if the name is not the name of a scalar type.
     */
    epicsShareExtern ScalarType getScalarType(std::string const &value);
    /**
     * Get a name for the scalarType.
     * @param  scalarType The type.
     * @return The name for the scalarType.
     */
    epicsShareExtern const char* name(ScalarType scalarType);

    //! gives sizeof(T) where T depends on the scalar type id.
    epicsShareExtern size_t elementSize(ScalarType id);
};

epicsShareExtern std::ostream& operator<<(std::ostream& o, const ScalarType& scalarType);

/** Define a compile time mapping from
 * type to enum value.
 @code
  ScalarType code = (ScalarType)ScalarTypeID<int8>::value;
  assert(code==pvByte);
 @endcode
 *
 * For unspecified types this evaluates to an invalid ScalarType
 * value (eg -1).
 */
template<typename T>
struct ScalarTypeID {};

/**
 * Static mapping from ScalarType enum to value type.
 @code
   typename ScalarTypeTraits<pvByte>::type value = 4;
 @endcode
 */
template<ScalarType ID>
struct ScalarTypeTraits {};

#define OP(ENUM, TYPE) \
template<> struct ScalarTypeTraits<ENUM> {typedef TYPE type;}; \
template<> struct ScalarTypeID<TYPE> { enum {value=ENUM}; }; \
template<> struct ScalarTypeID<const TYPE> { enum {value=ENUM}; };

OP(pvBoolean, boolean)
OP(pvByte, int8)
OP(pvShort, int16)
OP(pvInt, int32)
OP(pvLong, int64)
OP(pvUByte, uint8)
OP(pvUShort, uint16)
OP(pvUInt, uint32)
OP(pvULong, uint64)
OP(pvFloat, float)
OP(pvDouble, double)
OP(pvString, std::string)
#undef OP

}}

#if defined(_WIN32) && !defined(_MINGW)
#pragma warning( pop )
#endif

#endif  /* PVTYPE_H */
