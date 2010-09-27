/* Convert.cpp */
#include <string>
#include <stdexcept>
#include <lock.h>
#include "convert.h"

namespace epics { namespace pvData { 

static std::string notImplemented("not implemented");

    Convert::Convert(){}

    Convert::~Convert(){}

    void Convert::getFullName(StringConst buf,PVField const *pvField)
    {
        throw std::logic_error(notImplemented);
    }

    void Convert::getString(StringBuilder buf,PVField const * pvField,int indentLevel)
    {
        throw std::logic_error(notImplemented);
    }

    void Convert::getString(StringBuilder buf,PVField const *pvField)
    {
        throw std::logic_error(notImplemented);
    }

    void Convert::fromString(PVScalar *pv, StringConst from)
    {
        throw std::logic_error(notImplemented);
    }

    int Convert::fromString(PVScalarArray *pv, StringConst from)
    {
        throw std::logic_error(notImplemented);
    }

    int Convert::fromStringArray(PVScalarArray *pv, int offset, int length,
        StringConstArray from, int fromOffset)
    {
        throw std::logic_error(notImplemented);
    }

    int Convert::toStringArray(PVScalarArray const *pv, int offset, int length,
        StringConstArray to, int fromOffset)
    {
        throw std::logic_error(notImplemented);
    }

    epicsBoolean Convert::isCopyCompatible(FieldConstPtr from, FieldConstPtr to)
    {
        throw std::logic_error(notImplemented);
    }

    void Convert::copy(PVField const *from,PVField *to)
    {
        throw std::logic_error(notImplemented);
    }

    epicsBoolean Convert::isCopyScalarCompatible(
         ScalarConstPtr from, ScalarConstPtr to)
    {
        throw std::logic_error(notImplemented);
    }

    void copyScalar(PVScalar const *from, PVScalar *to)
    {
        throw std::logic_error(notImplemented);
    }

    epicsBoolean Convert::isCopyScalarArrayCompatible(ScalarArrayConstPtr from,
        ScalarArrayConstPtr to)
    {
        throw std::logic_error(notImplemented);
    }

    int Convert::copyScalarArray(PVScalarArray const *from, int offset,
        PVScalarArray *to, int toOffset, int length)
    {
        throw std::logic_error(notImplemented);
    }

    epicsBoolean Convert::isCopyStructureCompatible(
        StructureConstPtr from, StructureConstPtr to)
    {
        throw std::logic_error(notImplemented);
    }

    void Convert::copyStructure(PVStructure const *from, PVStructure *to)
    {
        throw std::logic_error(notImplemented);
    }

    epicsBoolean Convert::isCopyStructureArrayCompatible(
        StructureArrayConstPtr from, StructureArrayConstPtr to)
    {
        throw std::logic_error(notImplemented);
    }

    void Convert::copyStructureArray(
        PVStructureArray const *from, PVStructureArray *to)
    {
        throw std::logic_error(notImplemented);
    }

    epicsInt8 Convert::toByte(PVScalar const *pv)
    {
        throw std::logic_error(notImplemented);
    }

    epicsInt16 Convert::toShort(PVScalar const *pv)
    {
        throw std::logic_error(notImplemented);
    }

    epicsInt32 Convert::toInt(PVScalar const *pv)
    {
        throw std::logic_error(notImplemented);
    }

    epicsInt64 Convert::toLong(PVScalar const *pv)
    {
        throw std::logic_error(notImplemented);
    }

    float Convert::toFloat(PVScalar const *pv)
    {
        throw std::logic_error(notImplemented);
    }

    double Convert::toDouble(PVScalar const *pv)
    {
        throw std::logic_error(notImplemented);
    }

    void Convert::fromByte(PVScalar *pv,epicsInt8 from)
    {
        throw std::logic_error(notImplemented);
    }

    void  Convert::fromShort(PVScalar *pv,epicsInt16 from)
    {
        throw std::logic_error(notImplemented);
    }

    void  Convert::fromInt(PVScalar *pv, epicsInt32 from)
    {
        throw std::logic_error(notImplemented);
    }

    void  Convert::fromLong(PVScalar *pv, epicsInt64 from)
    {
        throw std::logic_error(notImplemented);
    }

    void  Convert::fromFloat(PVScalar* pv, float from)
    {
        throw std::logic_error(notImplemented);
    }

    void  Convert::fromDouble(PVScalar *pv, double from)
    {
        throw std::logic_error(notImplemented);
    }

    int Convert::toByteArray(PVScalarArray const *pv, int offset, int length,
        epicsInt8 to[], int toOffset)
    {
        throw std::logic_error(notImplemented);
    }

    int Convert::toShortArray(PVScalarArray const *pv, int offset, int length,
        epicsInt16 to[], int toOffset)
    {
        throw std::logic_error(notImplemented);
    }

    int Convert::toIntArray(PVScalarArray const *pv, int offset, int length,
        epicsInt32 to[], int toOffset)
    {
        throw std::logic_error(notImplemented);
    }

    int Convert::toLongArray(PVScalarArray const *pv, int offset, int length,
        epicsInt64 to[], int toOffset)
    {
        throw std::logic_error(notImplemented);
    }

    int Convert::toFloatArray(PVScalarArray const *pv, int offset, int length,
        float to[], int toOffset)
    {
        throw std::logic_error(notImplemented);
    }

    int Convert::toDoubleArray(PVScalarArray const *pv, int offset, int length,
        double to[], int toOffset)
    {
        throw std::logic_error(notImplemented);
    }

    int Convert::fromByteArray(PVScalarArray *pv, int offset, int length,
        epicsInt8 from[], int fromOffset)
    {
        throw std::logic_error(notImplemented);
    }

    int Convert::fromShortArray(PVScalarArray *pv, int offset, int length,
        epicsInt16 from[], int fromOffset)
    {
        throw std::logic_error(notImplemented);
    }

    int Convert::fromIntArray(PVScalarArray *pv, int offset, int length,
        epicsInt32 from[], int fromOffset)
    {
        throw std::logic_error(notImplemented);
    }

    int Convert::fromLongArray(PVScalarArray *pv, int offset, int length,
        epicsInt64 from[], int fromOffset)
    {
        throw std::logic_error(notImplemented);
    }

    int Convert::fromFloatArray(PVScalarArray *pv, int offset, int length,
        float from[], int fromOffset)
    {
        throw std::logic_error(notImplemented);
    }

    int Convert::fromDoubleArray(PVScalarArray *pv, int offset, int length,
        double from[], int fromOffset)
    {
        throw std::logic_error(notImplemented);
    }

    void Convert::newLine(StringBuilder buffer, int indentLevel)
    {
        *buffer += "\n";
        for(int i=0; i<indentLevel; i++) *buffer += "    ";
    }


    static Convert* instance = 0;


    class ConvertExt : public Convert {
    public:
        ConvertExt(): Convert(){};
    };

    Convert * getConvert() {
        static Mutex *mutex = new Mutex();
        Lock xx(mutex);

        if(instance==0) instance = new ConvertExt();
        return instance;
    }
    
}}
