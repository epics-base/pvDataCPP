/* pvData.h */
#include <string>
#include <stdexcept>
#include <map>
#ifndef PVDATA_H
#define PVDATA_H
#include "pvIntrospect.h"
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

    class PVAuxInfo {
    public:
        PVAuxInfo(PVField *pvField);
        ~PVAuxInfo();
        PVField * getPVField();
        PVScalar * createInfo(StringConstPtr key,ScalarType scalarType);
        std::map<StringConstPtr, PVScalar * > *getInfos();
        PVScalar * getInfo(StringConstPtr key);
        void toString(StringPtr buf);
        void toString(StringPtr buf,int indentLevel);
    private:
        static void init();
        PVAuxInfo(); // not implemented
        PVAuxInfo(PVAuxInfo const & ); // not implemented
        PVAuxInfo & operator=(PVAuxInfo const &); //not implemented
        class PVAuxInfoPvt *pImpl;
    };

    class PostHandler {
        virtual void postPut() = 0;
    };

    class PVField : public Requester, public Serializable {
    public:
        virtual ~PVField();
        StringConstPtr getRequesterName() const;
        virtual void message(StringConstPtr message,MessageType messageType) const;
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
        void renameField(StringConstPtr  newName);
        void postPut() const;
        void setPostHandler(PostHandler *postHandler);
        virtual void toString(StringPtr buf) const;
        virtual void toString(StringPtr buf,int indentLevel) const;
    protected:
        PVField(PVStructure *parent,FieldConstPtr field);
        void replaceStructure();
    private:
        PVField();//not implemented
        PVField(PVField const & ); // not implemented
        PVField & operator=(PVField const &); //not implemented
        class PVFieldPvt *pImpl;
        static void computeOffset(const PVField * const pvField);
        static void computeOffset(const PVField * const pvField,int offset);
    };

    class PVScalar : public PVField {
    public:
        virtual ~PVScalar();
        ScalarConstPtr getScalar() const;
        virtual void toString(StringPtr buf) const = 0;
        virtual void toString(StringPtr buf,int indentLevel) const = 0;
    protected:
        PVScalar(PVStructure *parent,ScalarConstPtr scalar);
    private:
        PVScalar(); // not implemented
        PVScalar(PVScalar const & ); // not implemented
        PVScalar & operator=(PVScalar const &); //not implemented
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
        virtual void toString(StringPtr buf) const;
        virtual void toString(StringPtr buf,int indentLevel) const = 0;
    protected:
        PVArray(PVStructure *parent,FieldConstPtr field);
    private:
        PVArray(); // not implemented
        PVArray(PVArray const & ); // not implemented
        PVArray & operator=(PVArray const &); //not implemented
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
        virtual void toString(StringPtr buf) const = 0;
        virtual void toString(StringPtr buf,int indentLevel) const = 0;
    protected:
        PVScalarArray(PVStructure *parent,ScalarArrayConstPtr scalarArray);
    private:
        PVScalarArray(); // not implemented
        PVScalarArray(PVScalarArray const & ); // not implemented
        PVScalarArray & operator=(PVScalarArray const &); //not implemented
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
        virtual void toString(StringPtr buf) const;
        virtual void toString(StringPtr buf,int indentLevel) const;
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

    class PVStructure : public PVField, public BitSetSerializable {
    public:
        virtual ~PVStructure();
        StructureConstPtr getStructure();
        PVFieldArrayPtr getPVFields();
        PVFieldPtr getSubField(StringConstPtr fieldName);
        PVFieldPtr getSubField(int fieldOffset);
        void appendPVField(PVField * pvField);
        void appendPVFields(PVFieldArrayPtr pvFields);
        void removePVField(StringConstPtr fieldName);
        PVBoolean *getBooleanField(StringConstPtr fieldName);
        PVByte *getByteField(StringConstPtr fieldName);
        PVShort *getShortField(StringConstPtr fieldName);
        PVInt *getIntField(StringConstPtr fieldName);
        PVLong *getLongField(StringConstPtr fieldName);
        PVFloat *getFloatField(StringConstPtr fieldName);
        PVDouble *getDoubleField(StringConstPtr fieldName);
        PVString *getStringField(StringConstPtr fieldName);
        PVStructure *getStructureField(StringConstPtr fieldName);
        PVScalarArray *getScalarArrayField(
            StringConstPtr fieldName,ScalarType elementType);
        PVStructureArray *getStructureArrayField(StringConstPtr fieldName);
        StringConstPtr getExtendsStructureName();
        epicsBoolean putExtendsStructureName(
            StringConstPtr extendsStructureName);
        virtual void toString(StringPtr buf) const;
        virtual void toString(StringPtr buf,int indentLevel) const;
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
        PVStructure(); // not implemented
        PVStructure(PVStructure const & ); // not implemented
        PVStructure& operator=(PVStructure const &); //not implemented
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
        PVBoolean(); // not implemented
        PVBoolean(PVBoolean const & ); // not implemented
        PVBoolean & operator=(PVBoolean const &); //not implemented
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
        PVByte(); // not implemented
        PVByte(PVByte const & ); // not implemented
        PVByte & operator=(PVByte const &); //not implemented
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
        PVShort(); // not implemented
        PVShort(PVShort const & ); // not implemented
        PVShort & operator=(PVShort const &); //not implemented
    };

    class PVInt : public PVScalar {
    public:
        virtual ~PVInt();
        virtual epicsInt32 get() const = 0;
        virtual void put(epicsInt32 value) = 0;
    protected:
        PVInt(PVStructure *parent,ScalarConstPtr scalar)
        : PVScalar(parent,scalar) {}
    private:
        PVInt(); // not implemented
        PVInt(PVInt const & ); // not implemented
        PVInt & operator=(PVInt const &); //not implemented
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
        PVLong(); // not implemented
        PVLong(PVLong const & ); // not implemented
        PVLong & operator=(PVLong const &); //not implemented
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
        PVFloat(); // not implemented
        PVFloat(PVFloat const & ); // not implemented
        PVFloat & operator=(PVFloat const &); //not implemented
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
        PVDouble(); // not implemented
        PVDouble(PVScalar const & ); // not implemented
        PVDouble & operator=(PVScalar const &); //not implemented
    };

    class PVString : public PVScalar {
    public:
        virtual ~PVString();
        virtual StringConstPtr get() const = 0;
        virtual void put(StringConstPtr value) = 0;
    protected:
        PVString(PVStructure *parent,ScalarConstPtr scalar)
        : PVScalar(parent,scalar) {}
    private:
        PVString(); // not implemented
        PVString(PVString const & ); // not implemented
        PVString & operator=(PVString const &); //not implemented
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
        virtual void toString(StringPtr buf) const;
        virtual void toString(StringPtr buf,int indentLevel) const;
        virtual int get(int offset, int length, BooleanArrayData *data) const;
        virtual int put(int offset,int length, EpicsBooleanArrayPtr  from, int fromOffset);
        virtual void shareData(EpicsBooleanArrayPtr from);
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) const;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher);
    protected:
        PVBooleanArray(PVStructure *parent,ScalarConstPtr scalar);
    private:
        PVBooleanArray(); // not implemented
        PVBooleanArray(PVBooleanArray const & ); // not implemented
        PVBooleanArray & operator=(PVBooleanArray const &); //not implemented
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
        virtual void toString(StringPtr buf) const;
        virtual void toString(StringPtr buf,int indentLevel) const;
        virtual int get(int offset, int length, ByteArrayData *data) const;
        virtual int put(int offset,int length, ByteArrayPtr  from, int fromOffset);
        virtual void shareData(ByteArrayPtr from);
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) const;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher);
    protected:
        PVByteArray(PVStructure *parent,ScalarConstPtr scalar);
    private:
        PVByteArray(); // not implemented
        PVByteArray(PVByteArray const & ); // not implemented
        PVByteArray & operator=(PVByteArray const &); //not implemented
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
        virtual void toString(StringPtr buf) const;
        virtual void toString(StringPtr buf,int indentLevel) const;
        virtual int get(int offset, int length, ShortArrayData *data) const;
        virtual int put(int offset,int length, ShortArrayPtr  from, int fromOffset);
        virtual void shareData(ShortArrayPtr from);
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) const;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher);
    protected:
        PVShortArray(PVStructure *parent,ScalarConstPtr scalar);
    private:
        PVShortArray(); // not implemented
        PVShortArray(PVShortArray const & ); // not implemented
        PVShortArray & operator=(PVShortArray const &); //not implemented
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
        virtual void toString(StringPtr buf) const;
        virtual void toString(StringPtr buf,int indentLevel) const;
        virtual int get(int offset, int length, IntArrayData *data) const;
        virtual int put(int offset,int length, IntArrayPtr  from, int fromOffset);
        virtual void shareData(IntArrayPtr from);
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) const;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher);
    protected:
        PVIntArray(PVStructure *parent,ScalarConstPtr scalar);
    private:
        PVIntArray(); // not implemented
        PVIntArray(PVIntArray const & ); // not implemented
        PVIntArray & operator=(PVIntArray const &); //not implemented
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
        virtual void toString(StringPtr buf) const;
        virtual void toString(StringPtr buf,int indentLevel) const;
        virtual int get(int offset, int length, LongArrayData *data) const;
        virtual int put(int offset,int length, LongArrayPtr  from, int fromOffset);
        virtual void shareData(LongArrayPtr from);
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) const;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher);
    protected:
        PVLongArray(PVStructure *parent,ScalarConstPtr scalar);
    private:
        PVLongArray(); // not implemented
        PVLongArray(PVLongArray const & ); // not implemented
        PVLongArray & operator=(PVLongArray const &); //not implemented
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
        virtual void toString(StringPtr buf) const;
        virtual void toString(StringPtr buf,int indentLevel) const;
        virtual int get(int offset, int length, FloatArrayData *data) const;
        virtual int put(int offset,int length, FloatArrayPtr  from, int fromOffset);
        virtual void shareData(FloatArrayPtr from);
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) const;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher);
    protected:
        PVFloatArray(PVStructure *parent,ScalarConstPtr scalar);
    private:
        PVFloatArray(); // not implemented
        PVFloatArray(PVFloatArray const & ); // not implemented
        PVFloatArray & operator=(PVFloatArray const &); //not implemented
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
        virtual void toString(StringPtr buf) const = 0;
        virtual void toString(StringPtr buf,int indentLevel) const = 0;
        virtual int get(int offset, int length, DoubleArrayData *data) const = 0;
        virtual int put(int offset,int length, DoubleArrayPtr  from, int fromOffset) = 0;
        virtual void shareData(DoubleArrayPtr from) = 0;
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) const = 0;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher) = 0;
    protected:
        PVDoubleArray(PVStructure *parent,ScalarArrayConstPtr scalar);
    private:
        PVDoubleArray(); // not implemented
        PVDoubleArray(PVDoubleArray const & ); // not implemented
        PVDoubleArray & operator=(PVDoubleArray const &); //not implemented
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
        virtual void toString(StringPtr buf) const;
        virtual void toString(StringPtr buf,int indentLevel) const;
        virtual int get(int offset, int length, StringArrayData *data) const;
        virtual int put(int offset,int length, StringArrayPtr  from, int fromOffset);
        virtual void shareData(StringArrayPtr from);
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) const;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher);
    protected:
        PVStringArray(PVStructure *parent,ScalarConstPtr scalar);
    private:
        PVStringArray(); // not implemented
        PVStringArray(PVStringArray const & ); // not implemented
        PVStringArray & operator=(PVStringArray const &); //not implemented
    };

    class PVDataCreate {
    public:
       PVField *createPVField(PVStructure *parent,
           FieldConstPtr field);
       PVField *createPVField(PVStructure *parent,
           StringConstPtr fieldName,PVField * fieldToClone);
       PVScalar *createPVScalar(PVStructure *parent,ScalarConstPtr scalar);
       PVScalar *createPVScalar(PVStructure *parent,
           StringConstPtr fieldName,ScalarType scalarType);
       PVScalar *createPVScalar(PVStructure *parent,
           StringConstPtr fieldName,PVScalar * scalarToClone);
       PVScalarArray *createPVScalarArray(PVStructure *parent,
           ScalarArrayConstPtr scalarArray);
       PVScalarArray *createPVScalarArray(PVStructure *parent,
           StringConstPtr fieldName,ScalarType elementType);
       PVScalarArray *createPVScalarArray(PVStructure *parent,
           StringConstPtr fieldName,PVScalarArray * scalarArrayToClone);
       PVStructureArray *createPVStructureArray(PVStructure *parent,
           StructureArrayConstPtr structureArray);
       PVStructure *createPVStructure(PVStructure *parent,
           StructureConstPtr structure);
       PVStructure *createPVStructure(PVStructure *parent,
           StringConstPtr fieldName,FieldConstPtrArray fields);
       PVStructure *createPVStructure(PVStructure *parent,
           StringConstPtr fieldName,PVStructure *structToClone);
    protected:
       PVDataCreate();
    };

    extern PVDataCreate * getPVDataCreate();
    
}}
#endif  /* PVDATA_H */
