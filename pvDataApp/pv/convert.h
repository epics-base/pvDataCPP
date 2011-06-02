/* convert.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef CONVERT_H
#define CONVERT_H
#include <string>
#include <stdexcept>

#include "pvIntrospect.h"
#include "pvData.h"
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


class Convert : NoDefaultMethods {
public:
    Convert();
    ~Convert();
    void getFullName(StringBuilder buf,PVField *pvField);
    bool equals(PVField &a,PVField &b);
    void getString(StringBuilder buf,PVField * pvField,int indentLevel);
    void getString(StringBuilder buf,PVField *pvField);
    int fromString(PVStructure *pv, std::vector<String>& from, int fromStartIndex = 0);    
    void fromString(PVScalar *pv, String from);
    int fromString(PVScalarArray *pv, String from);
    int fromStringArray(PVScalarArray *pv, int offset, int length,
        StringArray from, int fromOffset);
    int toStringArray(PVScalarArray *pv, int offset, int length,
        StringArray to, int toOffset);
    bool isCopyCompatible(FieldConstPtr from, FieldConstPtr to);
    void copy(PVField *from,PVField *to);
    bool isCopyScalarCompatible(
         ScalarConstPtr from, ScalarConstPtr to);
    void copyScalar(PVScalar *from, PVScalar *to);
    bool isCopyScalarArrayCompatible(ScalarArrayConstPtr from,
        ScalarArrayConstPtr to);
    int copyScalarArray(PVScalarArray *from, int offset,
        PVScalarArray *to, int toOffset, int length);
    bool isCopyStructureCompatible(
        StructureConstPtr from, StructureConstPtr to);
    void copyStructure(PVStructure *from, PVStructure *to);
    bool isCopyStructureArrayCompatible(
        StructureArrayConstPtr from, StructureArrayConstPtr to);
    void copyStructureArray(
        PVStructureArray *from, PVStructureArray *to);
    int8 toByte(PVScalar *pv);
    int16 toShort(PVScalar *pv);
    int32 toInt(PVScalar *pv);
    int64 toLong(PVScalar *pv);
    float toFloat(PVScalar *pv);
    double toDouble(PVScalar *pv);
    String toString(PVScalar *pv);
    void fromByte(PVScalar *pv,int8 from);
    void fromShort(PVScalar *pv,int16 from);
    void fromInt(PVScalar *pv, int32 from);
    void fromLong(PVScalar *pv, int64 from);
    void fromFloat(PVScalar* pv, float from);
    void fromDouble(PVScalar *pv, double from);
    int toByteArray(PVScalarArray *pv, int offset, int length,
        ByteArray to, int toOffset);
    int toShortArray(PVScalarArray *pv, int offset, int length,
        ShortArray to, int toOffset);
    int toIntArray(PVScalarArray *pv, int offset, int length,
        IntArray to, int toOffset);
    int toLongArray(PVScalarArray *pv, int offset, int length,
        LongArray to, int toOffset);
    int toFloatArray(PVScalarArray *pv, int offset, int length,
        FloatArray to, int toOffset);
    int toDoubleArray(PVScalarArray *pv, int offset, int length,
        DoubleArray to, int toOffset);
    int fromByteArray(PVScalarArray *pv, int offset, int length,
        ByteArray from, int fromOffset);
    int fromShortArray(PVScalarArray *pv, int offset, int length,
        ShortArray from, int fromOffset);
    int fromIntArray(PVScalarArray *pv, int offset, int length,
        IntArray from, int fromOffset);
    int fromLongArray(PVScalarArray *pv, int offset, int length,
        LongArray from, int fromOffset);
    int fromFloatArray(PVScalarArray *pv, int offset, int length,
        FloatArray from, int fromOffset);
    int fromDoubleArray(PVScalarArray *pv, int offset, int length,
        DoubleArray from, int fromOffset);
    void newLine(StringBuilder buf, int indentLevel);
};

extern Convert * getConvert();
    
}}
#endif  /* CONVERT_H */
