/* convert.h */
#include <string>
#include <stdexcept>
#ifndef CONVERT_H
#define CONVERT_H
#include "pvIntrospect.h"
#include "pvData.h"

namespace epics { namespace pvData { 

    class Convert : NoDefaultMethods {
    public:
        Convert();
        ~Convert();
        void getFullName(StringBuilder buf,PVField *pvField);
        void getString(StringBuilder buf,PVField * pvField,int indentLevel);
        void getString(StringBuilder buf,PVField *pvField);
        void fromString(PVScalar *pv, String from);
        int fromString(PVScalarArray *pv, String from);
        int fromStringArray(PVScalarArray *pv, int offset, int length,
            StringArray from, int fromOffset);
        int toStringArray(PVScalarArray *pv, int offset, int length,
            StringArray to, int toOffset);
        epicsBoolean isCopyCompatible(FieldConstPtr from, FieldConstPtr to);
        void copy(PVField *from,PVField *to);
        epicsBoolean isCopyScalarCompatible(
             ScalarConstPtr from, ScalarConstPtr to);
        void copyScalar(PVScalar *from, PVScalar *to);
        epicsBoolean isCopyScalarArrayCompatible(ScalarArrayConstPtr from,
            ScalarArrayConstPtr to);
        int copyScalarArray(PVScalarArray *from, int offset,
            PVScalarArray *to, int toOffset, int length);
        epicsBoolean isCopyStructureCompatible(
            StructureConstPtr from, StructureConstPtr to);
        void copyStructure(PVStructure *from, PVStructure *to);
        epicsBoolean isCopyStructureArrayCompatible(
            StructureArrayConstPtr from, StructureArrayConstPtr to);
        void copyStructureArray(
            PVStructureArray *from, PVStructureArray *to);
        epicsInt8 toByte(PVScalar *pv);
        epicsInt16 toShort(PVScalar *pv);
        epicsInt32 toInt(PVScalar *pv);
        epicsInt64 toLong(PVScalar *pv);
        float toFloat(PVScalar *pv);
        double toDouble(PVScalar *pv);
        void fromByte(PVScalar *pv,epicsInt8 from);
        void  fromShort(PVScalar *pv,epicsInt16 from);
        void  fromInt(PVScalar *pv, epicsInt32 from);
        void  fromLong(PVScalar *pv, epicsInt64 from);
        void  fromFloat(PVScalar* pv, float from);
        void  fromDouble(PVScalar *pv, double from);
        int toByteArray(PVScalarArray *pv, int offset, int length,
            epicsInt8 to[], int toOffset);
        int toShortArray(PVScalarArray *pv, int offset, int length,
            epicsInt16 to[], int toOffset);
        int toIntArray(PVScalarArray *pv, int offset, int length,
            epicsInt32 to[], int toOffset);
        int toLongArray(PVScalarArray *pv, int offset, int length,
            epicsInt64 to[], int toOffset);
        int toFloatArray(PVScalarArray *pv, int offset, int length,
            float to[], int toOffset);
        int toDoubleArray(PVScalarArray *pv, int offset, int length,
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
    };

    extern Convert * getConvert();
    
}}
#endif  /* CONVERT_H */
