/* convert.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 * Author - Marty Kraimer
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
 * pvByte, pvShort, pvInt,
 * pvLong, pvFloat, or pvDouble.</p>
 * 
 * <p>getString converts any supported type to a String.
 * Code that implements a PVField interface can implement
 * method toString by calling this method.</p>
 *
 * <p>fromString converts a String to a scalar.
 * fromStringArray converts an array of String
 * to a pvArray, which must have a scaler element type.
 * A scalar field is a numeric field or pvBoolean or pvString.</p>
 * <p>All from methods put data into a PVField, e.g. from means where the PVField gets it's data.</p>
 */

class Convert : NoDefaultMethods {
public:
    Convert();
    ~Convert();
    /**
     * Get the full fieldName for the pvField.
     * @param builder The builder that will have the result.
     * @param pvField The pvField.
     */
    void getFullName(StringBuilder buf,PVField *pvField);
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
    void getString(StringBuilder buf,PVField * pvField,int indentLevel);
    /**
     * Convert a PVField to a string.
     * param buf buffer for the result
     * @param pv The PVField to convert to a string.
     * If the PVField is a structure or array be prepared for a very long string.
     */
    void getString(StringBuilder buf,PVField *pvField);
     /**
      * Convert from an array of String to a PVScalar
      * @param pv The PV.
      * @param from The array of String value to convert and put into a PV.
      * @param fromStartIndex The first element if the array of strings.
      * @throws std::logic_error if the array of String does not have a valid values.
      */
    int fromString(PVStructure *pv, std::vector<String>& from, int fromStartIndex = 0);    
     /**
     * Convert from a String to a PVScalar
     * @param pv The PV.
     * @param from The String value to convert and put into a PV.
     * @throws std::logic_error if the String does not have a valid value.
     */
    void fromString(PVScalar *pv, String from);
    /**
     * Convert  from a String to a PVScalarArray.
     * The String must be a comma separated set of values optionally enclosed in []
     * @param pv The PV.
     * @param from The String value to convert and put into a PV.
     * @return The number of elements converted.
     * @throws std::invalid_argument if the element Type is not a scalar.
     * @throws std::logic_error if the String does not have a valid array values.
     */
    int fromString(PVScalarArray *pv, String from);
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
    int fromStringArray(PVScalarArray *pv, int offset, int length,
        StringArray from, int fromOffset);
    /**
     * Convert a PVScalarArray to a String array.
     * @param pv The PV.
     * @param offset Starting element in the PV array.
     * @param length Number of elements to convert to the string array.
     * @param to String array to receive the converted PV data.
     * @param toOffset Starting element in the string array.
     * @return Number of elements converted.
     */
    int toStringArray(PVScalarArray *pv, int offset, int length,
        StringArray to, int toOffset);
    /**
     * Are from and to valid arguments to copy.
     * This first checks of both arguments have the same Type.
     * Then calls one of isCopyScalarCompatible,
     * isCopyArrayCompatible, or isCopyStructureCompatible.
     * @param from The source.
     * @param to The destination.
     * @return (false,true) is the arguments (are not, are) compatible.
     */
    bool isCopyCompatible(FieldConstPtr from, FieldConstPtr to);
    /**
     * Copy from a PVField to another PVField.
     * This calls one on copyScalar, copyArray, copyStructure.
     * The two arguments must be compatible.
     * @param from The source.
     * @param to The destination
     * @throws std::invalid_argument if the arguments are not compatible.
     */
    void copy(PVField *from,PVField *to);
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
         ScalarConstPtr from, ScalarConstPtr to);
    /**
     * Copy from a scalar pv to another scalar pv.
     * @param from the source.
     * @param to the destination.
     * @throws std::invalid_argument if the arguments are not compatible.
     */
    void copyScalar(PVScalar *from, PVScalar *to);
    /**
     * Are from and to valid arguments to copyArray.
     * The results are like isCopyScalarCompatible except that the tests are made on the elementType.
     * @param from The from array.
     * @param to The to array.
     * @return (false,true) If the arguments (are not, are) compatible.
     */
    bool isCopyScalarArrayCompatible(ScalarArrayConstPtr from,
        ScalarArrayConstPtr to);
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
    int copyScalarArray(PVScalarArray *from, int offset,
        PVScalarArray *to, int toOffset, int length);
    /**
     * Are from and to valid arguments for copyStructure.
     * They are only compatible if they have the same Structure description.
     * @param from from structure.
     * @param to structure.
     * @return (false,true) If the arguments (are not, are) compatible.
     */
    bool isCopyStructureCompatible(
        StructureConstPtr from, StructureConstPtr to);
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
    void copyStructure(PVStructure *from, PVStructure *to);
    /**
     * Are from and to valid for copyStructureArray.
     * @param from The from StructureArray.
     * @param to The to StructureArray.
     * @return (false,true) If the arguments (are not, are) compatible.
     */
    bool isCopyStructureArrayCompatible(
        StructureArrayConstPtr from, StructureArrayConstPtr to);
     /**
      * Copy from a structure array to another structure array.
      * @param from The source array.
      * @param to The destination array.
      */
    void copyStructureArray(
        PVStructureArray *from, PVStructureArray *to);
    /**
     * Convert a PV to a <byte>.
     * @param pv a PV
     * @return converted value
     */
    int8 toByte(PVScalar *pv);
    /**
     * Convert a PV to a short.
     * @param pv a PV
     * @return converted value
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    int16 toShort(PVScalar *pv);
    /**
     * Convert a PV to a short.
     * @param pv a PV
     * @return converted value
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    int32 toInt(PVScalar *pv);
    /**
     * Convert a PV to an int
     * @param pv a PV
     * @return converted value
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    int64 toLong(PVScalar *pv);
    /**
     * Convert a PV to a float
     * @param pv a PV
     * @return converted value
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    float toFloat(PVScalar *pv);
    /**
     * Convert a PV to a double
     * @param pv a PV
     * @return converted value
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    double toDouble(PVScalar *pv);
    /**
     * Convert a PV to a String
     * @param pv a PV
     * @return converted value
     */
    String toString(PVScalar *pv);
    /**
     * Convert a PV from a byte
     * @param pv a PV
     * @param from value to put into PV
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    void fromByte(PVScalar *pv,int8 from);
    /**
     * Convert a PV from a short
     * @param pv a PV
     * @param from value to put into PV
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    void fromShort(PVScalar *pv,int16 from);
    /**
     * Convert a PV from an int
     * @param pv a PV
     * @param from value to put into PV
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    void fromInt(PVScalar *pv, int32 from);
    /**
     * Convert a PV from a long
     * @param pv a PV
     * @param from value to put into PV
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    void fromLong(PVScalar *pv, int64 from);
    /**
     * Convert a PV from a float
     * @param pv a PV
     * @param from value to put into PV
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    void fromFloat(PVScalar* pv, float from);
    /**
     * Convert a PV from a double
     * @param pv a PV
     * @param from value to put into PV
     * @throws std::invalid_argument if the Type is not a numeric scalar
     */
    void fromDouble(PVScalar *pv, double from);
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
    int toByteArray(PVScalarArray *pv, int offset, int length,
        ByteArray to, int toOffset);
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
    int toShortArray(PVScalarArray *pv, int offset, int length,
        ShortArray to, int toOffset);
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
    int toIntArray(PVScalarArray *pv, int offset, int length,
        IntArray to, int toOffset);
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
    int toLongArray(PVScalarArray *pv, int offset, int length,
        LongArray to, int toOffset);
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
    int toFloatArray(PVScalarArray *pv, int offset, int length,
        FloatArray to, int toOffset);
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
    int toDoubleArray(PVScalarArray *pv, int offset, int length,
        DoubleArray to, int toOffset);
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
    int fromByteArray(PVScalarArray *pv, int offset, int length,
        ByteArray from, int fromOffset);
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
    int fromShortArray(PVScalarArray *pv, int offset, int length,
        ShortArray from, int fromOffset);
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
    int fromIntArray(PVScalarArray *pv, int offset, int length,
        IntArray from, int fromOffset);
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
    int fromLongArray(PVScalarArray *pv, int offset, int length,
        LongArray from, int fromOffset);
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
    int fromFloatArray(PVScalarArray *pv, int offset, int length,
        FloatArray from, int fromOffset);
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
    int fromDoubleArray(PVScalarArray *pv, int offset, int length,
        DoubleArray from, int fromOffset);
    /**
     * Convenience method for implementing toString.
     * It generates a newline and inserts blanks at the beginning of the newline.
     * @param builder The StringBuilder being constructed.
     * @param indentLevel Indent level, Each level is four spaces.
     */
    void newLine(StringBuilder buf, int indentLevel);
};

extern Convert * getConvert();
    
}}
#endif  /* CONVERT_H */
