/* pvData.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <string>
#include <stdexcept>
#ifndef PVDATA_H
#define PVDATA_H
#include "pvType.h"
#include "pvIntrospect.h"
#include "noDefaultMethods.h"
#include "requester.h"
#include "byteBuffer.h"
#include "serialize.h"
namespace epics { namespace pvData { 

class PVAuxInfo;
class PostHandler;

class PVField;
class PVScalar;

class PVScalarArray;

class PVStructure;
class PVStructureArray;

typedef PVStructure * PVStructurePtr;
typedef PVStructurePtr* PVStructurePtrArray;
typedef PVField* PVFieldPtr;
typedef PVFieldPtr * PVFieldPtrArray;

class PVAuxInfo : private NoDefaultMethods {
public:
    PVAuxInfo(PVField *pvField);
    ~PVAuxInfo();
    PVField * getPVField();
    PVScalar * createInfo(String key,ScalarType scalarType);
    int getNumberInfo();
    PVScalar * getInfo(String name);
    PVScalar * getInfo(int index);
    void toString(StringBuilder buf);
    void toString(StringBuilder buf,int indentLevel);
private:
    PVField *pvField;
    int lengthInfo;
    int numberInfo;
    PVScalar **pvInfos; // ptr to array of PVscalar *
    friend class PVDataCreate;
};

class PostHandler {
public:
    virtual void postPut() = 0;
};

class PVField
: virtual public Requester,
  virtual public Serializable,
  private NoDefaultMethods
{
public:
    virtual ~PVField();
    String getRequesterName() ;
    virtual void message(String message,MessageType messageType) ;
    virtual void setRequester(Requester *prequester);
    int getFieldOffset() ;
    int getNextFieldOffset() ;
    int getNumberFields() ;
    PVAuxInfo * getPVAuxInfo();
    bool isImmutable() ;
    void setImmutable();
    FieldConstPtr getField() ;
    PVStructure * getParent() ;
    bool renameField(String  newName);
    void postPut() ;
    void setPostHandler(PostHandler *postHandler);
    virtual void toString(StringBuilder buf) ;
    virtual void toString(StringBuilder buf,int indentLevel) ;
    virtual bool operator==(PVField &pv) = 0;
    virtual bool operator!=(PVField &pv) = 0;
protected:
    PVField(PVStructure *parent,FieldConstPtr field);
    void setParent(PVStructure * parent);
private:
    void message(String fieldName,String message,MessageType messageType);
    class PVFieldPvt *pImpl;
    static void computeOffset(PVField *pvField);
    static void computeOffset(PVField *pvField,int offset);
    friend class PVDataCreate;
    friend class PVStructure;
};

class PVScalar : public PVField {
public:
    virtual ~PVScalar();
    ScalarConstPtr getScalar() ;
protected:
    PVScalar(PVStructure *parent,ScalarConstPtr scalar);
};

template<typename T>
class PVScalarValue : public PVScalar {
public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;

    virtual ~PVScalarValue() {}
    virtual T get() = 0;
    virtual void put(T value) = 0;
protected:
    PVScalarValue(PVStructure *parent,ScalarConstPtr scalar)
    : PVScalar(parent,scalar) {}
private:
};

typedef PVScalarValue<bool> PVBoolean;
typedef PVScalarValue<int8> PVByte;
typedef PVScalarValue<int16> PVShort;
typedef PVScalarValue<int32> PVInt;
typedef PVScalarValue<int64> PVLong;
typedef PVScalarValue<float> PVFloat;
typedef PVScalarValue<double> PVDouble;

// BasePVString is special case, since it implements SerializableArray
class PVString : public PVScalarValue<String>, SerializableArray {
public:
    virtual ~PVString() {}
protected:
    PVString(PVStructure *parent,ScalarConstPtr scalar)
    : PVScalarValue<String>(parent,scalar) {}
};


class PVArray : public PVField, public SerializableArray {
public:
    virtual ~PVArray();
    int getLength() const;
    void setLength(int length);
    int getCapacity() const;
    bool isCapacityMutable();
    void setCapacityMutable(bool isMutable);
    virtual void setCapacity(int capacity) = 0;

protected:
    PVArray(PVStructure *parent,FieldConstPtr field);
    void setCapacityLength(int capacity,int length);
private:
    class PVArrayPvt * pImpl;
};

template<typename T>
class PVArrayData {
public:
    typedef T  value_type;
    typedef T* pointer;
    typedef const T* const_pointer;

    pointer data;
    int offset;
};


class PVScalarArray : public PVArray {
public:
    virtual ~PVScalarArray();
    ScalarArrayConstPtr getScalarArray() ;

protected:
    PVScalarArray(PVStructure *parent,ScalarArrayConstPtr scalarArray);
private:
};

typedef PVArrayData<PVStructurePtr> StructureArrayData;

class PVStructureArray : public PVArray {
public:
    virtual ~PVStructureArray() {}
    virtual StructureArrayConstPtr getStructureArray() = 0;
    virtual int append(int number) = 0;
    virtual bool remove(int offset,int number) = 0;
    virtual void compress() = 0;
    virtual int get(int offset, int length,
        StructureArrayData *data) = 0;
    virtual int put(int offset,int length,
        PVStructurePtrArray from, int fromOffset) = 0;
    virtual void shareData( PVStructurePtrArray value,int capacity,int length) = 0;

protected:
    PVStructureArray(PVStructure *parent, StructureArrayConstPtr structureArray)
    : PVArray(parent,structureArray) {}
private:
};


class PVStructure : public PVField,public BitSetSerializable {
public:
    virtual ~PVStructure();
    StructureConstPtr getStructure();
    PVFieldPtrArray getPVFields();
    PVField *getSubField(String fieldName);
    PVField *getSubField(int fieldOffset);
    void appendPVField(PVField *pvField);
    void appendPVFields(int numberFields,PVFieldPtrArray pvFields);
    void removePVField(String fieldName);
    PVBoolean *getBooleanField(String fieldName);
    PVByte *getByteField(String fieldName);
    PVShort *getShortField(String fieldName);
    PVInt *getIntField(String fieldName);
    PVLong *getLongField(String fieldName);
    PVFloat *getFloatField(String fieldName);
    PVDouble *getDoubleField(String fieldName);
    PVString *getStringField(String fieldName);
    PVStructure *getStructureField(String fieldName);
    PVScalarArray *getScalarArrayField(
        String fieldName,ScalarType elementType);
    PVStructureArray *getStructureArrayField(String fieldName);
    String getExtendsStructureName();
    bool putExtendsStructureName(
        String extendsStructureName);
    virtual bool operator==(PVField &pv) ;
    virtual bool operator!=(PVField &pv) ;
    virtual void serialize(
        ByteBuffer *pbuffer,SerializableControl *pflusher) const;
    virtual void deserialize(
        ByteBuffer *pbuffer,DeserializableControl *pflusher);
    virtual void serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher,BitSet *pbitSet) const;
    virtual void deserialize(ByteBuffer *pbuffer,
        DeserializableControl*pflusher,BitSet *pbitSet);
    PVStructure(PVStructure *parent,StructureConstPtr structure);
    PVStructure(PVStructure *parent,StructureConstPtr structure,PVFieldPtrArray pvFields);
private:
    void setParentPvt(PVField *pvField,PVStructure *parent);
    class PVStructurePvt * pImpl;
};

template<typename T>
class PVValueArray : public PVScalarArray {
public:
    typedef T  value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef PVArrayData<T> ArrayDataType;

    virtual ~PVValueArray() {}
    virtual int get(int offset, int length, ArrayDataType *data) = 0;
    virtual int put(int offset,int length, pointer from, int fromOffset) = 0;
    virtual void shareData(pointer value,int capacity,int length) = 0;
protected:
    PVValueArray(PVStructure *parent,ScalarArrayConstPtr scalar)
    : PVScalarArray(parent,scalar) {}
private:
};

typedef PVArrayData<bool> BooleanArrayData;
typedef PVValueArray<bool> PVBooleanArray;

typedef PVArrayData<int8> ByteArrayData;
typedef PVValueArray<int8> PVByteArray;

typedef PVArrayData<int16> ShortArrayData;
typedef PVValueArray<int16> PVShortArray;

typedef PVArrayData<int32> IntArrayData;
typedef PVValueArray<int32> PVIntArray;

typedef PVArrayData<int64> LongArrayData;
typedef PVValueArray<int64> PVLongArray;


typedef PVArrayData<float> FloatArrayData;
typedef PVValueArray<float> PVFloatArray;

typedef PVArrayData<double> DoubleArrayData;
typedef PVValueArray<double> PVDoubleArray;

typedef PVArrayData<String> StringArrayData;
typedef PVValueArray<String> PVStringArray;

class PVDataCreate {
public:
   PVField *createPVField(PVStructure *parent,
       FieldConstPtr field);
   PVField *createPVField(PVStructure *parent,
       String fieldName,PVField * fieldToClone);
   PVScalar *createPVScalar(PVStructure *parent,ScalarConstPtr scalar);
   PVScalar *createPVScalar(PVStructure *parent,
       String fieldName,ScalarType scalarType);
   PVScalar *createPVScalar(PVStructure *parent,
       String fieldName,PVScalar * scalarToClone);
   PVScalarArray *createPVScalarArray(PVStructure *parent,
       ScalarArrayConstPtr scalarArray);
   PVScalarArray *createPVScalarArray(PVStructure *parent,
       String fieldName,ScalarType elementType);
   PVScalarArray *createPVScalarArray(PVStructure *parent,
       String fieldName,PVScalarArray * scalarArrayToClone);
   PVStructureArray *createPVStructureArray(PVStructure *parent,
       StructureArrayConstPtr structureArray);
   PVStructure *createPVStructure(PVStructure *parent,
       StructureConstPtr structure);
   PVStructure *createPVStructure(PVStructure *parent,
       String fieldName,int numberFields,FieldConstPtrArray fields);
   PVStructure *createPVStructure(PVStructure *parent,
       String fieldName,int numberFields,PVFieldPtrArray pvFields);
   PVStructure *createPVStructure(PVStructure *parent,
       String fieldName,PVStructure *structToClone);
protected:
   PVDataCreate();
   friend PVDataCreate * getPVDataCreate();
};

extern PVDataCreate * getPVDataCreate();
    
}}
#endif  /* PVDATA_H */
