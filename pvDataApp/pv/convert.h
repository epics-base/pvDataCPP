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

#include <pv/pvIntrospect.h>
#include <pv/pvData.h>
#include <vector>

namespace epics { namespace pvData { 

bool operator==(PVField&, PVField&);

static inline bool operator!=(PVField& a, PVField& b)
{return !(a==b);}


bool operator==(const Field&, const Field&);
bool operator==(const Scalar&, const Scalar&);
bool operator==(const ScalarArray&, const ScalarArray&);
bool operator==(const Structure&, const Structure&);
bool operator==(const StructureArray&, const StructureArray&);

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

class Convert {
public:
    static ConvertPtr getConvert();
    ~Convert();
    /**
     * Get the full fieldName for the pvField.
     * @param builder The builder that will have the result.
     * @param pvField The pvField.
     */
    void getFullName(StringBuilder buf,PVFieldPtr const & pvField);
    /**
     * Do fields have the same definition.
     *
     * @param  First field
     * @param  Second field
     * @return (false, true) if the fields (are not, are) the same.
     */
    bool equals(PVFieldPtr const &a,PVFieldPtr const &b);
    /**
     * Do fields have the same definition.
     *
     * @param  First field
     * @param  Second field
     * @return (false, true) if the fields (are not, are) the same.
     */
    bool equals(PVField &a,PVField &b);
    /**
     * Convert a PVField to a string.
     * @param buf buffer for the result
     * @param pv a PVField to convert to a string.
     * If a PVField is a structure or array be prepared for a very long string.
     * @param indentLevel indentation level
     */
    void getString(StringBuilder buf,PVFieldPtr const & pvField,int indentLevel);
    /**
     * Convert a PVField to a string.
     * param buf buffer for the result
     * @param pv The PVField to convert to a string.
     * If the PVField is a structure or array be prepared for a very long string.
     */
    void getString(StringBuilder buf,PVFieldPtr const & pvField);
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
    void getString(StringBuilder buf,PVField const * pvField);
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
    void fromString(PVScalarPtr const & pv, String const & from);
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
     * Convert from a source PV array to a destination PV array.
     * @param from The source array.
     * @param offset Starting element in the source.
     * @param to The destination array.
     * @param toOffset Starting element in the array.
     * @param length Number of elements to transfer.
     * @return Number of elements converted.
     * @throws std::invalid_argument if the arguments are not compatible.
     */
    std::size_t copyScalarArray(
        PVScalarArrayPtr const & from,
        std::size_t offset,
        PVScalarArrayPtr const & to,
        std::size_t toOffset,
        std::size_t length);
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
     * Convert a PV to a <byte>.
     * @param pv a PV
     * @return converted value
     */
    int8 toByte(PVScalarPtr const & pv);
    /**
     * Convert a PV to a short.
     * @param pv a PV
     * @return converted value
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    int16 toShort(PVScalarPtr const & pv);
    /**
     * Convert a PV to a int.
     * @param pv a PV
     * @return converted value
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    int32 toInt(PVScalarPtr const & pv);
    /**
     * Convert a PV to an long
     * @param pv a PV
     * @return converted value
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    int64 toLong(PVScalarPtr const & pv);
    /**
     * Convert a PV to a ubyte.
     * @param pv a PV
     * @return converted value
     */
    uint8 toUByte(PVScalarPtr const & pv);
    /**
     * Convert a PV to a ushort.
     * @param pv a PV
     * @return converted value
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    uint16 toUShort(PVScalarPtr const & pv);
    /**
     * Convert a PV to a uint.
     * @param pv a PV
     * @return converted value
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    uint32 toUInt(PVScalarPtr const & pv);
    /**
     * Convert a PV to an ulong
     * @param pv a PV
     * @return converted value
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    uint64 toULong(PVScalarPtr const & pv);
    /**
     * Convert a PV to a float
     * @param pv a PV
     * @return converted value
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    float toFloat(PVScalarPtr const & pv);
    /**
     * Convert a PV to a double
     * @param pv a PV
     * @return converted value
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    double toDouble(PVScalarPtr const & pv);
    /**
     * Convert a PV to a String
     * @param pv a PV
     * @return converted value
     */
    String toString(PVScalarPtr const & pv);
    /**
     * Convert a PV from a byte
     * @param pv a PV
     * @param from value to put into PV
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    void fromByte(PVScalarPtr const & pv,int8 from);
    /**
     * Convert a PV from a short
     * @param pv a PV
     * @param from value to put into PV
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    void fromShort(PVScalarPtr const & pv,int16 from);
    /**
     * Convert a PV from an int
     * @param pv a PV
     * @param from value to put into PV
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    void fromInt(PVScalarPtr const & pv, int32 from);
    /**
     * Convert a PV from a long
     * @param pv a PV
     * @param from value to put into PV
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    void fromLong(PVScalarPtr const & pv, int64 from);
    /**
     * Convert a PV from a ubyte
     * @param pv a PV
     * @param from value to put into PV
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    void fromUByte(PVScalarPtr const & pv,uint8 from);
    /**
     * Convert a PV from a ushort
     * @param pv a PV
     * @param from value to put into PV
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    void fromUShort(PVScalarPtr const & pv,uint16 from);
    /**
     * Convert a PV from an uint
     * @param pv a PV
     * @param from value to put into PV
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    void fromUInt(PVScalarPtr const & pv, uint32 from);
    /**
     * Convert a PV from a ulong
     * @param pv a PV
     * @param from value to put into PV
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    void fromULong(PVScalarPtr const & pv, uint64 from);
    /**
     * Convert a PV from a float
     * @param pv a PV
     * @param from value to put into PV
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    void fromFloat(PVScalarPtr const & pv, float from);
    /**
     * Convert a PV from a double
     * @param pv a PV
     * @param from value to put into PV
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    void fromDouble(PVScalarPtr const & pv, double from);
    /**
     * Convert a PV array to a byte array.
     * @param pv a PV
     * @param offset starting element in a PV
     * @param length number of elements to transfer
     * @param to where to put the PV data
     * @param toOffset starting element in the array
     * @return number of elements converted
     * @throws std::invalid_argument if the element type is not numeric
     */
    std::size_t toByteArray(PVScalarArrayPtr const & pv,
        std::size_t offset,
        std::size_t length,
        int8* to,
        std::size_t toOffset);
    /**
     * Convert a PV array to a short array.
     * @param pv a PV
     * @param offset starting element in a PV
     * @param length number of elements to transfer
     * @param to where to put the PV data
     * @param toOffset starting element in the array
     * @return number of elements converted
    * @throws std::invalid_argument if the element type is not numeric
    */
    std::size_t toShortArray(PVScalarArrayPtr const & pv,
        std::size_t offset,
        std::size_t length,
        int16* to,
        std::size_t toOffset);
    /**
     * Convert a PV array to an int array.
     * @param pv a PV
     * @param offset starting element in a PV
     * @param length number of elements to transfer
     * @param to where to put the PV data
     * @param toOffset starting element in the array
     * @return number of elements converted
     * @throws std::invalid_argument if the element type is not numeric
     */
    std::size_t toIntArray(PVScalarArrayPtr const & pv,
        std::size_t offset,
        std::size_t length,
        int32* to,
        std::size_t toOffset);
    /**
     * Convert a PV array to a long array.
     * @param pv a PV
     * @param offset starting element in a PV
     * @param length number of elements to transfer
     * @param to where to put the PV data
     * @param toOffset starting element in the array
     * @return number of elements converted
     * @throws std::invalid_argument if the element type is not numeric
     */
    std::size_t toLongArray(PVScalarArrayPtr const & pv,
        std::size_t offset,
        std::size_t length,
        int64* to,
        std::size_t toOffset);
    /**
     * Convert a PV array to a ubyte array.
     * @param pv a PV
     * @param offset starting element in a PV
     * @param length number of elements to transfer
     * @param to where to put the PV data
     * @param toOffset starting element in the array
     * @return number of elements converted
     * @throws std::invalid_argument if the element type is not numeric
     */
    std::size_t toUByteArray(PVScalarArrayPtr const & pv,
        std::size_t offset,
        std::size_t length,
        uint8* to,
        std::size_t toOffset);
    /**
     * Convert a PV array to a ushort array.
     * @param pv a PV
     * @param offset starting element in a PV
     * @param length number of elements to transfer
     * @param to where to put the PV data
     * @param toOffset starting element in the array
     * @return number of elements converted
    * @throws std::invalid_argument if the element type is not numeric
    */
    std::size_t toUShortArray(PVScalarArrayPtr const & pv,
        std::size_t offset,
        std::size_t length,
        uint16* to,
        std::size_t toOffset);
    /**
     * Convert a PV array to an uint array.
     * @param pv a PV
     * @param offset starting element in a PV
     * @param length number of elements to transfer
     * @param to where to put the PV data
     * @param toOffset starting element in the array
     * @return number of elements converted
     * @throws std::invalid_argument if the element type is not numeric
     */
    std::size_t toUIntArray(
        PVScalarArrayPtr const & pv,
        std::size_t offset,
        std::size_t length,
        uint32* to,
        std::size_t toOffset);
    /**
     * Convert a PV array to a ulong array.
     * @param pv a PV
     * @param offset starting element in a PV
     * @param length number of elements to transfer
     * @param to where to put the PV data
     * @param toOffset starting element in the array
     * @return number of elements converted
     * @throws std::invalid_argument if the element type is not numeric
     */
    std::size_t toULongArray(
        PVScalarArrayPtr const & pv,
        std::size_t offset,
        std::size_t length,
        uint64* to,
        std::size_t toOffset);
     /**
      * Convert a PV array to a float array.
      * @param pv a PV
      * @param offset starting element in a PV
      * @param length number of elements to transfer
      * @param to where to put the PV data
      * @param toOffset starting element in the array
      * @return number of elements converted
      * @throws std::invalid_argument if the element type is not numeric
      */
    std::size_t toFloatArray(
        PVScalarArrayPtr const & pv,
        std::size_t offset,
        std::size_t length,
        float* to,
        std::size_t toOffset);
    /**
     * Convert a PV array to a double array.
     * @param pv a PV
     * @param offset starting element in a PV
     * @param length number of elements to transfer
     * @param to where to put the PV data
     * @param toOffset starting element in the array
     * @return number of elements converted
     * @throws std::invalid_argument if the element type is not numeric
     */
    std::size_t toDoubleArray(
        PVScalarArrayPtr const & pv,
        std::size_t offset,
        std::size_t length,
        double* to, std::size_t
        toOffset);
    /**
     * Convert a PV array from a byte array.
     * @param pv a PV
     * @param offset starting element in a PV
     * @param length number of elements to transfer
     * @param from value to put into PV
     * @param fromOffset
     * @return number of elements converted
     * @throws std::invalid_argument if the element type is not numeric
     */
    std::size_t fromByteArray(
        PVScalarArrayPtr & pv, std::size_t offset, std::size_t length,
        const int8* from, std::size_t fromOffset);
    std::size_t fromByteArray(
        PVScalarArrayPtr & pv, std::size_t offset, std::size_t length,
        const ByteArray & from, std::size_t fromOffset);
    /**
     * Convert a PV array from a short array.
     * @param pv a PV
     * @param offset starting element in a PV
     * @param length number of elements to transfer
     * @param from value to put into PV
     * @param fromOffset starting element in the source array
     * @return number of elements converted
     * @throws std::invalid_argument if the element type is not numeric
     */
    std::size_t fromShortArray(
        PVScalarArrayPtr & pv, std::size_t offset, std::size_t length,
        const int16* from, std::size_t fromOffset);
    std::size_t fromShortArray(
        PVScalarArrayPtr & pv, std::size_t offset, std::size_t length,
        const ShortArray & from, std::size_t fromOffset);
    /**
     * Convert a PV array from an int array.
     * @param pv a PV
     * @param offset starting element in a PV
     * @param length number of elements to transfer
     * @param from value to put into PV
     * @param fromOffset starting element in the source array
     * @return number of elements converted
     * @throws std::invalid_argument if the element type is not numeric
     */
    std::size_t fromIntArray(
       PVScalarArrayPtr & pv, std::size_t offset, std::size_t length,
       const int32* from, std::size_t fromOffset);
    std::size_t fromIntArray(
       PVScalarArrayPtr & pv, std::size_t offset, std::size_t length,
       const IntArray & from, std::size_t fromOffset);
    /**
     * Convert a PV array from a long array.
     * @param pv a PV
     * @param offset starting element in a PV
     * @param length number of elements to transfer
     * @param from value to put into PV
     * @param fromOffset starting element in the source array
     * @return number of elements converted
     * @throws std::invalid_argument if the element type is not numeric
     */
    std::size_t fromLongArray(
        PVScalarArrayPtr & pv, std::size_t offset, std::size_t length,
        const int64* from, std::size_t fromOffset);
    std::size_t fromLongArray(
        PVScalarArrayPtr & pv, std::size_t offset, std::size_t length,
        const LongArray & from, std::size_t fromOffset);
    /**
     * Convert a PV array from a ubyte array.
     * @param pv a PV
     * @param offset starting element in a PV
     * @param length number of elements to transfer
     * @param from value to put into PV
     * @param fromOffset
     * @return number of elements converted
     * @throws std::invalid_argument if the element type is not numeric
     */
    std::size_t fromUByteArray(
        PVScalarArrayPtr & pv, std::size_t offset, std::size_t length,
        const uint8* from, std::size_t fromOffset);
    std::size_t fromUByteArray(
        PVScalarArrayPtr & pv, std::size_t offset, std::size_t length,
        const UByteArray & from, std::size_t fromOffset);
    /**
     * Convert a PV array from a ushort array.
     * @param pv a PV
     * @param offset starting element in a PV
     * @param length number of elements to transfer
     * @param from value to put into PV
     * @param fromOffset starting element in the source array
     * @return number of elements converted
     * @throws std::invalid_argument if the element type is not numeric
     */
    std::size_t fromUShortArray(
        PVScalarArrayPtr & pv, std::size_t offset, std::size_t length,
        const uint16* from, std::size_t fromOffset);
    std::size_t fromUShortArray(
        PVScalarArrayPtr & pv, std::size_t offset, std::size_t length,
        const UShortArray & from, std::size_t fromOffset);
    /**
     * Convert a PV array from an uint array.
     * @param pv a PV
     * @param offset starting element in a PV
     * @param length number of elements to transfer
     * @param from value to put into PV
     * @param fromOffset starting element in the source array
     * @return number of elements converted
     * @throws std::invalid_argument if the element type is not numeric
     */
    std::size_t fromUIntArray(
        PVScalarArrayPtr & pv, std::size_t offset, std::size_t length,
        const uint32* from, std::size_t fromOffset);
    std::size_t fromUIntArray(
        PVScalarArrayPtr & pv, std::size_t offset, std::size_t length,
        const UIntArray & from, std::size_t fromOffset);
    /**
     * Convert a PV array from a ulong array.
     * @param pv a PV
     * @param offset starting element in a PV
     * @param length number of elements to transfer
     * @param from value to put into PV
     * @param fromOffset starting element in the source array
     * @return number of elements converted
     * @throws std::invalid_argument if the element type is not numeric
     */
    std::size_t fromULongArray(
        PVScalarArrayPtr & pv, std::size_t offset, std::size_t length,
        const uint64* from, std::size_t fromOffset);
    std::size_t fromULongArray(
        PVScalarArrayPtr & pv, std::size_t offset, std::size_t length,
        const ULongArray & from, std::size_t fromOffset);
    /**
     * Convert a PV array from a float array.
     * @param pv a PV
     * @param offset starting element in a PV
     * @param length number of elements to transfer
     * @param from value to put into PV
     * @param fromOffset starting element in the source array
     * @return number of elements converted
     * @throws std::invalid_argument if the element type is not numeric
     */
    std::size_t fromFloatArray(
        PVScalarArrayPtr & pv, std::size_t offset, std::size_t length,
        const float* from, std::size_t fromOffset);
    std::size_t fromFloatArray(
        PVScalarArrayPtr & pv, std::size_t offset, std::size_t length,
        const FloatArray & from, std::size_t fromOffset);
    /**
     * Convert a PV array from a double array.
     * @param pv a PV
     * @param offset starting element in a PV
     * @param length number of elements to transfer
     * @param from value to put into PV
     * @param fromOffset starting element in the source array
     * @return number of elements converted
     * @throws std::invalid_argument if the element type is not numeric
     */
    std::size_t fromDoubleArray(
        PVScalarArrayPtr & pv, std::size_t offset, std::size_t length,
        const double* from, std::size_t fromOffset);
    std::size_t fromDoubleArray(
        PVScalarArrayPtr & pv, std::size_t offset, std::size_t length,
        const DoubleArray & from, std::size_t fromOffset);
    /**
     * Convenience method for implementing toString.
     * It generates a newline and inserts blanks at the beginning of the newline.
     * @param builder The StringBuilder being constructed.
     * @param indentLevel Indent level, Each level is four spaces.
     */
    void newLine(StringBuilder buf, int indentLevel);
private:
    Convert();
    PVDataCreatePtr pvDataCreate;
    String trueString;
    String falseString;
    String illegalScalarType;
};

extern ConvertPtr getConvert();
    
}}
#endif  /* CONVERT_H */
