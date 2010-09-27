/* pvData.h */
#include <string>
#include <stdexcept>
#include <map>
#ifndef PVDATA_H
#define PVDATA_H
#include "pvIntrospect.h"
#include "noDefaultMethods.h"
#include "requester.h"
#include "byteBuffer.h"
#include "serialize.h"
namespace epics { namespace pvData { 

    class PVField;
    class PVScalar;
    class PVBoolean;
    class PVByte;
    class PVShort;
    class PVInt;
    class PVLong;
    class PVFloat;
    class PVDouble;
    class PVString;

    class PVScalarArray;
    class PVBooleanArray;
    class PVByteArray;
    class PVShortArray;
    class PVIntArray;
    class PVLongArray;
    class PVFloatArray;
    class PVDoubleArray;
    class PVStringArray;

    class PVStructure;
    class PVStructureArray;

    // NOTE this prevents compiler from generating default methods for this and 
    // derived classes
    class PVAuxInfo : private NoDefaultMethods {
    public:
        PVAuxInfo(PVField *pvField);
        ~PVAuxInfo();
        PVField * getPVField();
        PVScalar * createInfo(StringConst key,ScalarType scalarType);
        std::map<StringConst, PVScalar * > *getInfos();
        PVScalar * getInfo(StringConst key);
        void toString(StringBuilder buf);
        void toString(StringBuilder buf,int indentLevel);
    private:
        static void init();
        class PVAuxInfoPvt *pImpl;
    };

    class PostHandler {
        virtual void postPut() = 0;
    };

    class PVField : public Requester, public Serializable ,private NoDefaultMethods{
    public:
        virtual ~PVField();
        StringConst getRequesterName() const;
        virtual void message(StringConst message,MessageType messageType) const;
        virtual void setRequester(Requester *prequester);
        int getFieldOffset() const;
        int getNextFieldOffset() const;
        int getNumberFields() const;
        PVAuxInfo * getPVAuxInfo();
        epicsBoolean isImmutable() const;
        void setImmutable();
        FieldConstPtr getField() const;
        PVStructure * getParent() const;
        void replacePVField(PVField * newPVField);
        void renameField(StringConst  newName);
        void postPut() const;
        void setPostHandler(PostHandler *postHandler);
        virtual void toString(StringBuilder buf) const;
        virtual void toString(StringBuilder buf,int indentLevel) const;
    protected:
        PVField(PVStructure *parent,FieldConstPtr field);
        void replaceStructure();
    private:
        class PVFieldPvt *pImpl;
        static void computeOffset(const PVField * const pvField);
        static void computeOffset(const PVField * const pvField,int offset);
    };

    class PVScalar : public PVField {
    public:
        virtual ~PVScalar();
        ScalarConstPtr getScalar() const;
        virtual void toString(StringBuilder buf) const = 0;
        virtual void toString(StringBuilder buf,int indentLevel) const = 0;
    protected:
        PVScalar(PVStructure *parent,ScalarConstPtr scalar);
    };

    class PVArray : public PVField, public SerializableArray {
    public:
        virtual ~PVArray();
        int getLength() const;
        void setLength(int length);
        int getCapacity() const;
        epicsBoolean isCapacityImmutable() const;
        void setCapacityImmutable(epicsBoolean isMutable);
        virtual void setCapacity(int capacity) = 0;
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher) const = 0;
        virtual void deserialize(ByteBuffer *pbuffer,
            DeserializableControl *pflusher) = 0;
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher, int offset, int count) const = 0;
        virtual void toString(StringBuilder buf) const;
        virtual void toString(StringBuilder buf,int indentLevel) const = 0;
    protected:
        PVArray(PVStructure *parent,FieldConstPtr field);
    private:
        class PVArrayPvt *pImpl;
    };


    class PVScalarArray : public PVArray {
    public:
        virtual ~PVScalarArray();
        ScalarArrayConstPtr getScalarArray() const;
        virtual void setCapacity(int capacity) = 0;
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher) const = 0;
        virtual void deserialize(ByteBuffer *pbuffer,
            DeserializableControl *pflusher) = 0;
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher, int offset, int count) const = 0;
        virtual void toString(StringBuilder buf) const = 0;
        virtual void toString(StringBuilder buf,int indentLevel) const = 0;
    protected:
        PVScalarArray(PVStructure *parent,ScalarArrayConstPtr scalarArray);
    private:
    };

    typedef PVStructure * PVStructurePtr;
    typedef PVStructurePtr * PVStructureArrayPtr;

    class StructureArrayData {
    public:
        PVStructureArrayPtr data;
        int offset;
    };

    class PVStructureArray : public PVArray {
    public:
        virtual ~PVStructureArray();
        virtual StructureArrayConstPtr getStructureArray() const;
        virtual int get(int offset, int length,
            StructureArrayData *data) const;
        virtual int put(int offset,int length,
            PVStructureArrayPtr  from, int fromOffset);
        virtual void shareData(PVStructureArrayPtr from);
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher) const;
        virtual void deserialize(ByteBuffer *pbuffer,
            DeserializableControl *pflusher);
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher, int offset, int count) const;
        virtual void toString(StringBuilder buf) const;
        virtual void toString(StringBuilder buf,int indentLevel) const;
    protected:
        PVStructureArray(PVStructure *parent,
            StructureArrayConstPtr structureArray);
    private:
        PVStructureArray(); // not implemented
        PVStructureArray(PVStructureArray const & ); // not implemented
        PVStructureArray & operator=(PVStructureArray const &); //not implemented
        class PVStructureArrayPvt *pImpl;
    };
    
    typedef PVField * PVFieldPtr;
    typedef PVFieldPtr * PVFieldArrayPtr;

    class PVStructure : public PVField,public BitSetSerializable {
    public:
        virtual ~PVStructure();
        StructureConstPtr getStructure();
        PVFieldArrayPtr getPVFields();
        PVFieldPtr getSubField(StringConst fieldName);
        PVFieldPtr getSubField(int fieldOffset);
        void appendPVField(PVField * pvField);
        void appendPVFields(PVFieldArrayPtr pvFields);
        void removePVField(StringConst fieldName);
        PVBoolean *getBooleanField(StringConst fieldName);
        PVByte *getByteField(StringConst fieldName);
        PVShort *getShortField(StringConst fieldName);
        PVInt *getIntField(StringConst fieldName);
        PVLong *getLongField(StringConst fieldName);
        PVFloat *getFloatField(StringConst fieldName);
        PVDouble *getDoubleField(StringConst fieldName);
        PVString *getStringField(StringConst fieldName);
        PVStructure *getStructureField(StringConst fieldName);
        PVScalarArray *getScalarArrayField(
            StringConst fieldName,ScalarType elementType);
        PVStructureArray *getStructureArrayField(StringConst fieldName);
        StringConst getExtendsStructureName();
        epicsBoolean putExtendsStructureName(
            StringConst extendsStructureName);
        virtual void toString(StringBuilder buf) const;
        virtual void toString(StringBuilder buf,int indentLevel) const;
        virtual void serialize(
            ByteBuffer *pbuffer,SerializableControl *pflusher) const;
        virtual void deserialize(
            ByteBuffer *pbuffer,DeserializableControl *pflusher);
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher, int offset, int count) const;
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher,BitSet *pbitSet) const;
        virtual void deserialize(ByteBuffer *pbuffer,
            DeserializableControl*pflusher,BitSet *pbitSet);
    protected:
        PVStructure(PVStructure *parent,StructureConstPtr structure);
    private:
        class PVStructurePvt *pImpl;
    };

    class PVBoolean : public PVScalar {
    public:
        virtual ~PVBoolean();
        virtual epicsBoolean get() const = 0;
        virtual void put(epicsBoolean value) = 0;
    protected:
        PVBoolean(PVStructure *parent,ScalarConstPtr scalar)
        : PVScalar(parent,scalar) {}
    private:
    };

    class PVByte : public PVScalar {
    public:
        virtual ~PVByte();
        virtual epicsInt8 get() const = 0;
        virtual void put(epicsInt8 value) = 0;
    protected:
        PVByte(PVStructure *parent,ScalarConstPtr scalar)
        : PVScalar(parent,scalar) {}
    private:
    };

    class PVShort : public PVScalar {
    public:
        virtual ~PVShort();
        virtual epicsInt16 get() const = 0;
        virtual void put(epicsInt16 value) = 0;
    protected:
        PVShort(PVStructure *parent,ScalarConstPtr scalar)
        : PVScalar(parent,scalar) {}
    private:
    };

    class PVInt : public PVScalar{
    public:
        virtual ~PVInt();
        virtual epicsInt32 get() const = 0;
        virtual void put(epicsInt32 value) = 0;
    protected:
        PVInt(PVStructure *parent,ScalarConstPtr scalar)
        : PVScalar(parent,scalar) {}
    private:
    };

    class PVLong : public PVScalar {
    public:
        virtual ~PVLong();
        virtual epicsInt64 get() const = 0;
        virtual void put(epicsInt64 value) = 0;
    protected:
        PVLong(PVStructure *parent,ScalarConstPtr scalar)
        : PVScalar(parent,scalar) {}
    private:
    };

    class PVFloat : public PVScalar {
    public:
        virtual ~PVFloat();
        virtual float get() const = 0;
        virtual void put(float value) = 0;
    protected:
        PVFloat(PVStructure *parent,ScalarConstPtr scalar)
        : PVScalar(parent,scalar) {}
    private:
    };

    class PVDouble : public PVScalar {
    public:
        virtual ~PVDouble();
        virtual double get() const = 0;
        virtual void put(double value) = 0;
    protected:
        PVDouble(PVStructure *parent,ScalarConstPtr scalar)
        : PVScalar(parent,scalar) {}
    private:
    };

    class PVString : public PVScalar {
    public:
        virtual ~PVString();
        virtual StringConst get() const = 0;
        virtual void put(StringConst value) = 0;
    protected:
        PVString(PVStructure *parent,ScalarConstPtr scalar)
        : PVScalar(parent,scalar) {}
    private:
    };

    typedef epicsBoolean * EpicsBooleanArrayPtr;

    class BooleanArrayData {
    public:
        EpicsBooleanArrayPtr data;
        int offset;
    };

    class PVBooleanArray : public PVScalarArray {
    public:
        virtual ~PVBooleanArray();
        virtual void toString(StringBuilder buf) const;
        virtual void toString(StringBuilder buf,int indentLevel) const;
        virtual int get(int offset, int length, BooleanArrayData *data) const;
        virtual int put(int offset,int length, EpicsBooleanArrayPtr  from, int fromOffset);
        virtual void shareData(EpicsBooleanArrayPtr from);
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) const;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher);
    protected:
        PVBooleanArray(PVStructure *parent,ScalarConstPtr scalar);
    private:
    };

    typedef signed char * ByteArrayPtr;

    class ByteArrayData {
    public:
        ByteArrayPtr data;
        int offset;
    };

    class PVByteArray : public PVScalarArray {
    public:
        virtual ~PVByteArray();
        virtual void toString(StringBuilder buf) const;
        virtual void toString(StringBuilder buf,int indentLevel) const;
        virtual int get(int offset, int length, ByteArrayData *data) const;
        virtual int put(int offset,int length, ByteArrayPtr  from, int fromOffset);
        virtual void shareData(ByteArrayPtr from);
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) const;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher);
    protected:
        PVByteArray(PVStructure *parent,ScalarConstPtr scalar);
    private:
    };

    typedef short * ShortArrayPtr;

    class ShortArrayData {
    public:
        ShortArrayPtr data;
        int offset;
    };

    class PVShortArray : public PVScalarArray {
    public:
        virtual ~PVShortArray();
        virtual void toString(StringBuilder buf) const;
        virtual void toString(StringBuilder buf,int indentLevel) const;
        virtual int get(int offset, int length, ShortArrayData *data) const;
        virtual int put(int offset,int length, ShortArrayPtr  from, int fromOffset);
        virtual void shareData(ShortArrayPtr from);
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) const;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher);
    protected:
        PVShortArray(PVStructure *parent,ScalarConstPtr scalar);
    private:
    };

    typedef int * IntArrayPtr;

    class IntArrayData {
    public:
        IntArrayPtr data;
        int offset;
    };

    class PVIntArray : public PVScalarArray {
    public:
        virtual ~PVIntArray();
        virtual void toString(StringBuilder buf) const;
        virtual void toString(StringBuilder buf,int indentLevel) const;
        virtual int get(int offset, int length, IntArrayData *data) const;
        virtual int put(int offset,int length, IntArrayPtr  from, int fromOffset);
        virtual void shareData(IntArrayPtr from);
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) const;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher);
    protected:
        PVIntArray(PVStructure *parent,ScalarConstPtr scalar);
    private:
    };

    typedef long * LongArrayPtr;

    class LongArrayData {
    public:
        LongArrayPtr data;
        int offset;
    };

    class PVLongArray : public PVScalarArray {
    public:
        virtual ~PVLongArray();
        virtual void toString(StringBuilder buf) const;
        virtual void toString(StringBuilder buf,int indentLevel) const;
        virtual int get(int offset, int length, LongArrayData *data) const;
        virtual int put(int offset,int length, LongArrayPtr  from, int fromOffset);
        virtual void shareData(LongArrayPtr from);
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) const;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher);
    protected:
        PVLongArray(PVStructure *parent,ScalarConstPtr scalar);
    private:
    };


    typedef float * FloatArrayPtr;

    class FloatArrayData {
    public:
        FloatArrayPtr data;
        int offset;
    };

    class PVFloatArray : public PVScalarArray {
    public:
        virtual ~PVFloatArray();
        virtual void toString(StringBuilder buf) const;
        virtual void toString(StringBuilder buf,int indentLevel) const;
        virtual int get(int offset, int length, FloatArrayData *data) const;
        virtual int put(int offset,int length, FloatArrayPtr  from, int fromOffset);
        virtual void shareData(FloatArrayPtr from);
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) const;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher);
    protected:
        PVFloatArray(PVStructure *parent,ScalarConstPtr scalar);
    private:
    };

    typedef double * DoubleArrayPtr;

    class DoubleArrayData {
    public:
        DoubleArrayPtr data;
        int offset;
    };

    class PVDoubleArray : public PVScalarArray {
    public:
        virtual ~PVDoubleArray();
        virtual void toString(StringBuilder buf) const = 0;
        virtual void toString(StringBuilder buf,int indentLevel) const = 0;
        virtual int get(int offset, int length, DoubleArrayData *data) const = 0;
        virtual int put(int offset,int length, DoubleArrayPtr  from, int fromOffset) = 0;
        virtual void shareData(DoubleArrayPtr from) = 0;
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) const = 0;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher) = 0;
    protected:
        PVDoubleArray(PVStructure *parent,ScalarArrayConstPtr scalar);
    private:
    };

    typedef std::string * StringArrayPtr;

    class StringArrayData {
    public:
        StringArrayPtr data;
        int offset;
    };

    class PVStringArray : public PVScalarArray {
    public:
        virtual ~PVStringArray();
        virtual void toString(StringBuilder buf) const;
        virtual void toString(StringBuilder buf,int indentLevel) const;
        virtual int get(int offset, int length, StringArrayData *data) const;
        virtual int put(int offset,int length, StringArrayPtr  from, int fromOffset);
        virtual void shareData(StringArrayPtr from);
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) const;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher);
    protected:
        PVStringArray(PVStructure *parent,ScalarConstPtr scalar);
    private:
    };

    class PVDataCreate {
    public:
       PVField *createPVField(PVStructure *parent,
           FieldConstPtr field);
       PVField *createPVField(PVStructure *parent,
           StringConst fieldName,PVField * fieldToClone);
       PVScalar *createPVScalar(PVStructure *parent,ScalarConstPtr scalar);
       PVScalar *createPVScalar(PVStructure *parent,
           StringConst fieldName,ScalarType scalarType);
       PVScalar *createPVScalar(PVStructure *parent,
           StringConst fieldName,PVScalar * scalarToClone);
       PVScalarArray *createPVScalarArray(PVStructure *parent,
           ScalarArrayConstPtr scalarArray);
       PVScalarArray *createPVScalarArray(PVStructure *parent,
           StringConst fieldName,ScalarType elementType);
       PVScalarArray *createPVScalarArray(PVStructure *parent,
           StringConst fieldName,PVScalarArray * scalarArrayToClone);
       PVStructureArray *createPVStructureArray(PVStructure *parent,
           StructureArrayConstPtr structureArray);
       PVStructure *createPVStructure(PVStructure *parent,
           StructureConstPtr structure);
       PVStructure *createPVStructure(PVStructure *parent,
           StringConst fieldName,FieldConstPtrArray fields);
       PVStructure *createPVStructure(PVStructure *parent,
           StringConst fieldName,PVStructure *structToClone);
    protected:
       PVDataCreate();
    };

    extern PVDataCreate * getPVDataCreate();
    
}}
#endif  /* PVDATA_H */
