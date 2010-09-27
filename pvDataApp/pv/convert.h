/* convert.h */
#include <string>
#include <stdexcept>
#ifndef CONVERT_H
#define CONVERT_H
#include "pvIntrospect.h"
#include "pvData.h"

namespace epics { namespace pvData { 

    class Convert {
    public:
        Convert();
        ~Convert();
        void getFullName(StringConst buf,PVField const *pvField);
        void getString(StringBuilder buf,PVField const * pvField,int indentLevel);
        void getString(StringBuilder buf,PVField const * pvField);
        void fromString(PVScalar *pv, StringConst from);
        int fromString(PVScalarArray *pv, StringConst from);
        int fromStringArray(PVScalarArray *pv, int offset, int length,
            StringConstArray from, int fromOffset);
        int toStringArray(PVScalarArray const *pv, int offset, int length,
            StringConstArray to, int fromOffset);
        epicsBoolean isCopyCompatible(FieldConstPtr from, FieldConstPtr to);
        void copy(PVField const *from,PVField *to);
        epicsBoolean isCopyScalarCompatible(
             ScalarConstPtr from, ScalarConstPtr to);
        void copyScalar(PVScalar const *from, PVScalar *to);
        epicsBoolean isCopyScalarArrayCompatible(ScalarArrayConstPtr from,
            ScalarArrayConstPtr to);
        int copyScalarArray(PVScalarArray const *from, int offset,
            PVScalarArray *to, int toOffset, int length);
        epicsBoolean isCopyStructureCompatible(
            StructureConstPtr from, StructureConstPtr to);
        void copyStructure(PVStructure const *from, PVStructure *to);
        epicsBoolean isCopyStructureArrayCompatible(
            StructureArrayConstPtr from, StructureArrayConstPtr to);
        void copyStructureArray(
            PVStructureArray const *from, PVStructureArray *to);
        epicsInt8 toByte(PVScalar const *pv);
        epicsInt16 toShort(PVScalar const *pv);
        epicsInt32 toInt(PVScalar const *pv);
        epicsInt64 toLong(PVScalar const *pv);
        float toFloat(PVScalar const *pv);
        double toDouble(PVScalar const *pv);
        void fromByte(PVScalar *pv,epicsInt8 from);
        void  fromShort(PVScalar *pv,epicsInt16 from);
        void  fromInt(PVScalar *pv, epicsInt32 from);
        void  fromLong(PVScalar *pv, epicsInt64 from);
        void  fromFloat(PVScalar* pv, float from);
        void  fromDouble(PVScalar *pv, double from);
        int toByteArray(PVScalarArray const *pv, int offset, int length,
            epicsInt8 to[], int toOffset);
        int toShortArray(PVScalarArray const *pv, int offset, int length,
            epicsInt16 to[], int toOffset);
        int toIntArray(PVScalarArray const *pv, int offset, int length,
            epicsInt32 to[], int toOffset);
        int toLongArray(PVScalarArray const *pv, int offset, int length,
            epicsInt64 to[], int toOffset);
        int toFloatArray(PVScalarArray const *pv, int offset, int length,
            float to[], int toOffset);
        int toDoubleArray(PVScalarArray const *pv, int offset, int length,
            double to[], int toOffset);
        int fromByteArray(PVScalarArray *pv, int offset, int length,
            epicsInt8 from[], int fromOffset);
        int fromShortArray(PVScalarArray *pv, int offset, int length,
            epicsInt16 from[], int fromOffset);
        int fromIntArray(PVScalarArray *pv, int offset, int length,
            epicsInt32 from[], int fromOffset);
        int fromLongArray(PVScalarArray *pv, int offset, int length,
            epicsInt64 from[], int fromOffset);
        int fromFloatArray(PVScalarArray *pv, int offset, int length,
            float from[], int fromOffset);
        int fromDoubleArray(PVScalarArray *pv, int offset, int length,
            double from[], int fromOffset);
        void newLine(StringBuilder buf, int indentLevel);

    private:
        Convert(Convert const & ); // not implemented
        Convert & operator=(Convert const &); //not implemented
    };

    extern Convert * getConvert();
    
}}
#endif  /* CONVERT_H */
