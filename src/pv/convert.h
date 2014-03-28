/* convert.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 * Author - Marty Kraimer
 */
/**
 *  @author mrk
 */
#ifndef CONVERT_H
#define CONVERT_H

#include <string>
#include <stdexcept>
#include <vector>

#include <pv/pvIntrospect.h>
#include <pv/pvData.h>

#include <shareLib.h>

namespace epics { namespace pvData { 

bool epicsShareExtern operator==(const PVField&, const PVField&);

static inline bool operator!=(const PVField& a, const PVField& b)
{return !(a==b);}


bool epicsShareExtern operator==(const Field&, const Field&);
bool epicsShareExtern operator==(const Scalar&, const Scalar&);
bool epicsShareExtern operator==(const ScalarArray&, const ScalarArray&);
bool epicsShareExtern operator==(const Structure&, const Structure&);
bool epicsShareExtern operator==(const StructureArray&, const StructureArray&);
bool epicsShareExtern operator==(const Union&, const Union&);
bool epicsShareExtern operator==(const UnionArray&, const UnionArray&);

static inline bool operator!=(const Field& a, const Field& b)
{return !(a==b);}
static inline bool operator!=(const Scalar& a, const Scalar& b)
{return !(a==b);}
static inline bool operator!=(const ScalarArray& a, const ScalarArray& b)
{return !(a==b);}
static inline bool operator!=(const Structure& a, const Structure& b)
{return !(a==b);}
static inline bool operator!=(const StructureArray& a, const StructureArray& b)
{return !(a==b);}
static inline bool operator!=(const Union& a, const Union& b)
{return !(a==b);}
static inline bool operator!=(const UnionArray& a, const UnionArray& b)
{return !(a==b);}


/**
 * Convert between numeric types,  convert any field to a string,
 *  or convert from a string to a scalar field.
 * <p>Numeric conversions are between scalar numeric types or between arrays of
 * numeric types. It is not possible to convert between a scalar
 * and an array.
 * Numeric conversions are between types:
 * pvByte, pvShort, pvInt, pvLong,
 * pvUByte, pvUShort, pvUInt, pvULong,
 * pvFloat, or pvDouble.</p>
 * 
 * <p>getString converts any supported type to a String.
 * Code that implements a PVField interface should implement
 * method toString by calling this method.</p>
 *
 * <p>fromString converts a String to a scalar.
 * fromStringArray converts an array of Strings
 * to a pvArray, which must have a scaler element type.
 * A scalar field is a numeric field or pvBoolean or pvString.</p>
 * <p>All from methods put data into a PVField, e.g. from means where the PVField gets it's data.</p>
 */

class Convert;
typedef std::tr1::shared_ptr<Convert> ConvertPtr;

class epicsShareClass Convert {
public:
    static ConvertPtr getConvert();
    /**
     * Get the full fieldName for the pvField.
     * @param builder The builder that will have the result.
     * @param pvField The pvField.
     */
    void getFullName(StringBuilder buf,PVFieldPtr const & pvField)
    {
        *buf = pvField->getFullName();
    }

    /**
     * Do fields have the same definition.
     *
     * @param  First field
     * @param  Second field
     * @return (false, true) if the fields (are not, are) the same.
     */
    inline bool equals(PVFieldPtr const &a,PVFieldPtr const &b)
    {
        return *a==*b;
    }

    /**
     * Do fields have the same definition.
     *
     * @param  First field
     * @param  Second field
     * @return (false, true) if the fields (are not, are) the same.
     */
    inline bool equals(PVField &a,PVField &b)
    {
        return a==b;
    }

    /**
     * Convert a PVField to a string.
     * @param buf buffer for the result
     * @param pv a PVField to convert to a string.
     * If a PVField is a structure or array be prepared for a very long string.
     * @param indentLevel indentation level
     */
    inline void getString(StringBuilder buf,PVFieldPtr const & pvField,int indentLevel)
    {getString(buf, pvField.get(), indentLevel);}
    /**
     * Convert a PVField to a string.
     * param buf buffer for the result
     * @param pv The PVField to convert to a string.
     * If the PVField is a structure or array be prepared for a very long string.
     */
    inline void getString(StringBuilder buf,PVFieldPtr const & pvField)
    {getString(buf, pvField.get(), 0);}
    /**
     * Convert a PVField to a string.
     * @param buf buffer for the result
     * @param pv a PVField to convert to a string.
     * If a PVField is a structure or array be prepared for a very long string.
     * @param indentLevel indentation level
     */
    void getString(StringBuilder buf,PVField const * pvField,int indentLevel);
    /**
     * Convert a PVField to a string.
     * param buf buffer for the result
     * @param pv The PVField to convert to a string.
     * If the PVField is a structure or array be prepared for a very long string.
     */
    inline void getString(StringBuilder buf,PVField const * pvField)
    {getString(buf, pvField, 0);}
     /**
      * Convert from an array of String to a PVScalar
      * @param pv The PV.
      * @param from The array of String value to convert and put into a PV.
      * @param fromStartIndex The first element if the array of strings.
      * @throws std::logic_error if the array of String does not have a valid values.
      */
    std::size_t fromString(
        PVStructurePtr const &pv,
        StringArray const & from,
        std::size_t fromStartIndex = 0);
     /**
     * Convert from a String to a PVScalar
     * @param pv The PV.
     * @param from The String value to convert and put into a PV.
     * @throws std::logic_error if the String does not have a valid value.
     */
    void fromString(PVScalarPtr const & pv, String const & from)
    {
        pv->putFrom<String>(from);
    }

    /**
     * Convert  from a String to a PVScalarArray.
     * The String must be a comma separated set of values optionally enclosed in []
     * @param pv The PV.
     * @param from The String value to convert and put into a PV.
     * @return The number of elements converted.
     * @throws std::invalid_argument if the element Type is not a scalar.
     * @throws std::logic_error if the String does not have a valid array values.
     */
    std::size_t fromString(PVScalarArrayPtr const & pv, String from);
    /**
     * Convert a PVScalarArray from a String array.
     * The array element type must be a scalar.
     * @param pv The PV.
     * @param offset Starting element in a PV.
     * @param length The number of elements to transfer.
     * @param from The array of values to put into the PV.
     * @param fromOffset Starting element in the source array.
     * @return The number of elements converted.
     * @throws std::invalid_argument if the element Type is not a scalar.
     * @throws std::logic_error if the String does not have a valid value.
     */
    std::size_t fromStringArray(
        PVScalarArrayPtr const & pv,
        std::size_t offset, std::size_t length,
        StringArray const & from,
        std::size_t fromOffset);
    /**
     * Convert a PVScalarArray to a String array.
     * @param pv The PV.
     * @param offset Starting element in the PV array.
     * @param length Number of elements to convert to the string array.
     * @param to String array to receive the converted PV data.
     * @param toOffset Starting element in the string array.
     * @return Number of elements converted.
     */
    std::size_t toStringArray(PVScalarArrayPtr const & pv,
        std::size_t offset,
        std::size_t length,
        StringArray & to,
        std::size_t toOffset);
    /**
     * Are from and to valid arguments to copy.
     * This first checks of both arguments have the same Type.
     * Then calls one of isCopyScalarCompatible,
     * isCopyArrayCompatible, or isCopyStructureCompatible.
     * @param from The source.
     * @param to The destination.
     * @return (false,true) is the arguments (are not, are) compatible.
     */
    bool isCopyCompatible(FieldConstPtr const & from, FieldConstPtr const & to);
    /**
     * Copy from a PVField to another PVField.
     * This calls one on copyScalar, copyArray, copyStructure.
     * The two arguments must be compatible.
     * @param from The source.
     * @param to The destination
     * @throws std::invalid_argument if the arguments are not compatible.
     */
    void copy(PVFieldPtr const & from, PVFieldPtr const & to);
    /**
     * Are from and to valid arguments to copyScalar.
     * false will be returned if either argument is not a scalar as defined by Type.isScalar().
     * If both are scalars the return value is true if any of the following are true.
     * <ul>
     *   <li>Both arguments are numeric.</li>
     *   <li>Both arguments have the same type.</li>
     *   <li>Either argument is a string.</li>
     * </ul>
     * @param from The introspection interface for the from data.
     * @param to The introspection interface for the to data..
     * @return (false,true) If the arguments (are not, are) compatible.
     */
    bool isCopyScalarCompatible(
        ScalarConstPtr const & from,
        ScalarConstPtr const & to);
    /**
     * Copy from a scalar pv to another scalar pv.
     * @param from the source.
     * @param to the destination.
     * @throws std::invalid_argument if the arguments are not compatible.
     */
    void copyScalar(PVScalarPtr const & from, PVScalarPtr const & to);
    /**
     * Are from and to valid arguments to copyArray.
     * The results are like isCopyScalarCompatible except that the tests are made on the elementType.
     * @param from The from array.
     * @param to The to array.
     * @return (false,true) If the arguments (are not, are) compatible.
     */
    bool isCopyScalarArrayCompatible(
        ScalarArrayConstPtr const & from,
        ScalarArrayConstPtr const & to);
    /**
     * Are from and to valid arguments for copyStructure.
     * They are only compatible if they have the same Structure description.
     * @param from from structure.
     * @param to structure.
     * @return (false,true) If the arguments (are not, are) compatible.
     */
    bool isCopyStructureCompatible(
        StructureConstPtr const & from, StructureConstPtr const & to);
    /**
     * Copy from a structure pv to another structure pv.
     * NOTE: Only compatible nodes are copied. This means:
     * <ul>
     *    <li>For scalar nodes this means that isCopyScalarCompatible is true.</li>
     *    <li>For array nodes this means that isCopyArrayCompatible is true.</li>
     *    <li>For structure nodes this means that isCopyStructureCompatible is true.</li>
     *    <li>Link nodes are not copied.</li>
     * </ul>
     * @param from The source.
     * @param to The destination.
     * @throws std::invalid_argument if the arguments are not compatible.
     */
    void copyStructure(PVStructurePtr const & from, PVStructurePtr const & to);
    /**
     * Are from and to valid for copyStructureArray.
     * @param from The from StructureArray.
     * @param to The to StructureArray.
     * @return (false,true) If the arguments (are not, are) compatible.
     */
    bool isCopyStructureArrayCompatible(
        StructureArrayConstPtr const & from, StructureArrayConstPtr const & to);
     /**
      * Copy from a structure array to another structure array.
      * @param from The source array.
      * @param to The destination array.
      */
    void copyStructureArray(
        PVStructureArrayPtr const & from, PVStructureArrayPtr const & to);
    /**
     * Are from and to valid arguments for copyUnion.
     * They are only compatible if they have the same Union description.
     * @param from from union.
     * @param to union.
     * @return (false,true) If the arguments (are not, are) compatible.
     */
    bool isCopyUnionCompatible(
        UnionConstPtr const & from, UnionConstPtr const & to);
    /**
     * Copy from a union pv to another union pv.
     * NOTE: Only compatible nodes are copied.
     * @param from The source.
     * @param to The destination.
     * @throws std::invalid_argument if the arguments are not compatible.
     */
    void copyUnion(PVUnionPtr const & from, PVUnionPtr const & to);
    /**
     * Are from and to valid for copyUnionArray.
     * @param from The from UnionArray.
     * @param to The to UnionArray.
     * @return (false,true) If the arguments (are not, are) compatible.
     */
    bool isCopyUnionArrayCompatible(
        UnionArrayConstPtr const & from, UnionArrayConstPtr const & to);
     /**
      * Copy from a union array to another union array.
      * @param from The source array.
      * @param to The destination array.
      */
    void copyUnionArray(
        PVUnionArrayPtr const & from, PVUnionArrayPtr const & to);
    /**
     * Convert a PV to a <byte>.
     * @param pv a PV
     * @return converted value
     */
    inline int8 toByte(PVScalarPtr const & pv) { return pv->getAs<int8>();}
    /**
     * Convert a PV to a short.
     * @param pv a PV
     * @return converted value
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    inline int16 toShort(PVScalarPtr const & pv) { return pv->getAs<int16>();}
    /**
     * Convert a PV to a int.
     * @param pv a PV
     * @return converted value
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    inline int32 toInt(PVScalarPtr const & pv) { return pv->getAs<int32>();}
    /**
     * Convert a PV to an long
     * @param pv a PV
     * @return converted value
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    inline int64 toLong(PVScalarPtr const & pv) { return pv->getAs<int32>();}
    /**
     * Convert a PV to a ubyte.
     * @param pv a PV
     * @return converted value
     */
    inline uint8 toUByte(PVScalarPtr const & pv) { return pv->getAs<uint8>();}
    /**
     * Convert a PV to a ushort.
     * @param pv a PV
     * @return converted value
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    inline uint16 toUShort(PVScalarPtr const & pv) { return pv->getAs<uint16>();}
    /**
     * Convert a PV to a uint.
     * @param pv a PV
     * @return converted value
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    inline uint32 toUInt(PVScalarPtr const & pv) { return pv->getAs<uint32>();}
    /**
     * Convert a PV to an ulong
     * @param pv a PV
     * @return converted value
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    inline uint64 toULong(PVScalarPtr const & pv) { return pv->getAs<uint64>();}
    /**
     * Convert a PV to a float
     * @param pv a PV
     * @return converted value
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    inline float toFloat(PVScalarPtr const & pv) { return pv->getAs<float>();}
    /**
     * Convert a PV to a double
     * @param pv a PV
     * @return converted value
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    inline double toDouble(PVScalarPtr const & pv) { return pv->getAs<double>();}
    /**
     * Convert a PV to a String
     * @param pv a PV
     * @return converted value
     */
    inline String toString(PVScalarPtr const & pv) { return pv->getAs<String>();}
    /**
     * Convert a PV from a byte
     * @param pv a PV
     * @param from value to put into PV
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    inline void fromByte(PVScalarPtr const & pv,int8 from) { pv->putFrom<int8>(from); }
    /**
     * Convert a PV from a short
     * @param pv a PV
     * @param from value to put into PV
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    inline void fromShort(PVScalarPtr const & pv,int16 from) { pv->putFrom<int16>(from); }
    /**
     * Convert a PV from an int
     * @param pv a PV
     * @param from value to put into PV
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    inline void fromInt(PVScalarPtr const & pv, int32 from) { pv->putFrom<int32>(from); }
    /**
     * Convert a PV from a long
     * @param pv a PV
     * @param from value to put into PV
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    inline void fromLong(PVScalarPtr const & pv, int64 from) { pv->putFrom<int64>(from); }
    /**
     * Convert a PV from a ubyte
     * @param pv a PV
     * @param from value to put into PV
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    inline void fromUByte(PVScalarPtr const & pv,uint8 from) { pv->putFrom<uint8>(from); }
    /**
     * Convert a PV from a ushort
     * @param pv a PV
     * @param from value to put into PV
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    inline void fromUShort(PVScalarPtr const & pv,uint16 from) { pv->putFrom<uint16>(from); }
    /**
     * Convert a PV from an uint
     * @param pv a PV
     * @param from value to put into PV
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    inline void fromUInt(PVScalarPtr const & pv, uint32 from) { pv->putFrom<uint32>(from); }
    /**
     * Convert a PV from a ulong
     * @param pv a PV
     * @param from value to put into PV
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    inline void fromULong(PVScalarPtr const & pv, uint64 from) { pv->putFrom<uint64>(from); }
    /**
     * Convert a PV from a float
     * @param pv a PV
     * @param from value to put into PV
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    inline void fromFloat(PVScalarPtr const & pv, float from) { pv->putFrom<float>(from); }
    /**
     * Convert a PV from a double
     * @param pv a PV
     * @param from value to put into PV
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    inline void fromDouble(PVScalarPtr const & pv, double from) { pv->putFrom<double>(from); }

    /**
     * Convenience method for implementing toString.
     * It generates a newline and inserts blanks at the beginning of the newline.
     * @param builder The StringBuilder being constructed.
     * @param indentLevel Indent level, Each level is four spaces.
     */
    void newLine(StringBuilder buf, int indentLevel);
};

static inline ConvertPtr getConvert() { return Convert::getConvert(); }
    
}}
#endif  /* CONVERT_H */
