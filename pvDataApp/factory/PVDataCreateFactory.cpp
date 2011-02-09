/*PVDataCreateFactory.cpp*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "lock.h"
#include "pvIntrospect.h"
#include "pvData.h"
#include "convert.h"
#include "factory.h"
#include "PVField.cpp"
#include "PVScalar.cpp"
#include "PVArray.cpp"
#include "PVScalarArray.cpp"
#include "PVStructure.cpp"
#include "DefaultPVStructureArray.cpp"

namespace epics { namespace pvData {

static Convert* convert = 0;
static FieldCreate * fieldCreate = 0;
static PVDataCreate* pvDataCreate = 0;

/** Default storage for scalar values
 */
template<typename T>
class BasePVScalar : public PVScalarValue<T> {
public:
    typedef T  value_type;
    typedef T* pointer;
    typedef const T* const_pointer;

    BasePVScalar(PVStructure *parent,ScalarConstPtr scalar);
    virtual ~BasePVScalar();
    virtual T get();
    virtual void put(T val);
    virtual void serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher);
    virtual void deserialize(ByteBuffer *pbuffer,
        DeserializableControl *pflusher);
    virtual bool operator==(PVField& pv) ;
    virtual bool operator!=(PVField& pv) ;
private:
    T value;
};

template<typename T>
BasePVScalar<T>::BasePVScalar(PVStructure *parent,ScalarConstPtr scalar)
    : PVScalarValue<T>(parent,scalar),value(0)
{}
//Note: '0' is a suitable default for all POD types (not String)

template<typename T>
BasePVScalar<T>::~BasePVScalar() {}

template<typename T>
T BasePVScalar<T>::get() { return value;}

template<typename T>
void BasePVScalar<T>::put(T val){value = val;}

template<typename T>
void BasePVScalar<T>::serialize(ByteBuffer *pbuffer,
    SerializableControl *pflusher) {
    pflusher->ensureBuffer(1);
    pbuffer->putBoolean(value);
}

template<typename T>
void BasePVScalar<T>::deserialize(ByteBuffer *pbuffer,
    DeserializableControl *pflusher)
{
    pflusher->ensureData(1);
    value = pbuffer->getBoolean();
}

template<typename T>
bool BasePVScalar<T>::operator==(PVField& pvField)
{
    return getConvert()->equals(this, &pvField);
}

template<typename T>
bool BasePVScalar<T>::operator!=(PVField& pvField)
{
    return !(getConvert()->equals(this, &pvField));
}

// Specializations 

template<>
BasePVScalar<String>::BasePVScalar(PVStructure *parent,ScalarConstPtr scalar)
: PVScalarValue<String>(parent,scalar),value()
{}

template<>
void BasePVScalar<String>::serialize(ByteBuffer *pbuffer,
    SerializableControl *pflusher) {
    SerializeHelper::serializeString(value, pbuffer, pflusher);
}

template<>
void BasePVScalar<String>::deserialize(ByteBuffer *pbuffer,
    DeserializableControl *pflusher) {
    value = SerializeHelper::deserializeString(pbuffer, pflusher);
}

template<>
void BasePVScalar<bool>::serialize(ByteBuffer *pbuffer,
    SerializableControl *pflusher) {
    pflusher->ensureBuffer(1);
    pbuffer->putBoolean(value);
}

template<>
void BasePVScalar<bool>::deserialize(ByteBuffer *pbuffer,
    DeserializableControl *pflusher) {
    pflusher->ensureData(1);
    value = pbuffer->getBoolean();
}

template<>
void BasePVScalar<int8>::serialize(ByteBuffer *pbuffer,
    SerializableControl *pflusher) {
    pflusher->ensureBuffer(1);
    pbuffer->putByte(value);
}

template<>
void BasePVScalar<int8>::deserialize(ByteBuffer *pbuffer,
    DeserializableControl *pflusher) {
    pflusher->ensureData(1);
    value = pbuffer->getByte();
}

template<>
void BasePVScalar<int16>::serialize(ByteBuffer *pbuffer,
    SerializableControl *pflusher) {
    pflusher->ensureBuffer(2);
    pbuffer->putShort(value);
}

template<>
void BasePVScalar<int16>::deserialize(ByteBuffer *pbuffer,
    DeserializableControl *pflusher) {
    pflusher->ensureData(2);
    value = pbuffer->getShort();
}

template<>
void BasePVScalar<int32>::serialize(ByteBuffer *pbuffer,
    SerializableControl *pflusher) {
    pflusher->ensureBuffer(4);
    pbuffer->putInt(value);
}

template<>
void BasePVScalar<int32>::deserialize(ByteBuffer *pbuffer,
    DeserializableControl *pflusher) {
    pflusher->ensureData(4);
    value = pbuffer->getInt();
}

template<>
void BasePVScalar<int64>::serialize(ByteBuffer *pbuffer,
    SerializableControl *pflusher) {
    pflusher->ensureBuffer(8);
    pbuffer->putLong(value);
}

template<>
void BasePVScalar<int64>::deserialize(ByteBuffer *pbuffer,
    DeserializableControl *pflusher) {
    pflusher->ensureData(8);
    value = pbuffer->getLong();
}

template<>
void BasePVScalar<float>::serialize(ByteBuffer *pbuffer,
    SerializableControl *pflusher) {
    pflusher->ensureBuffer(4);
    pbuffer->putFloat(value);
}

template<>
void BasePVScalar<float>::deserialize(ByteBuffer *pbuffer,
    DeserializableControl *pflusher) {
    pflusher->ensureData(4);
    value = pbuffer->getFloat();
}

template<>
void BasePVScalar<double>::serialize(ByteBuffer *pbuffer,
    SerializableControl *pflusher) {
    pflusher->ensureBuffer(8);
    pbuffer->putDouble(value);
}

template<>
void BasePVScalar<double>::deserialize(ByteBuffer *pbuffer,
    DeserializableControl *pflusher) {
    pflusher->ensureData(8);
    value = pbuffer->getDouble();
}

typedef BasePVScalar<bool> BasePVBoolean;
typedef BasePVScalar<int8> BasePVByte;
typedef BasePVScalar<int16> BasePVShort;
typedef BasePVScalar<int32> BasePVInt;
typedef BasePVScalar<int64> BasePVLong;
typedef BasePVScalar<float> BasePVFloat;
typedef BasePVScalar<double> BasePVDouble;
typedef BasePVScalar<String> BasePVString;


/** Default storage for arrays
 */
template<typename T>
class DefaultPVArray : public PVValueArray<T> {
public:
    typedef T  value_type;
    typedef T* pointer;
    typedef const T* const_pointer;

    DefaultPVArray(PVStructure *parent,ScalarArrayConstPtr scalarArray);
    virtual ~DefaultPVArray();
    virtual void setCapacity(int capacity);
    virtual int get(int offset, int length, PVArrayData<T> *data) ;
    virtual int put(int offset,int length, pointer from,
       int fromOffset);
    virtual void shareData(pointer value,int capacity,int length);
    // from Serializable
    virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) ;
    virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher);
    virtual void serialize(ByteBuffer *pbuffer,
         SerializableControl *pflusher, int offset, int count) ;
    virtual bool operator==(PVField& pv) ;
    virtual bool operator!=(PVField& pv) ;
private:
    pointer value;
};

template<typename T>
DefaultPVArray<T>::DefaultPVArray(PVStructure *parent,
    ScalarArrayConstPtr scalarArray)
: PVValueArray<T>(parent,scalarArray),value(new T[0])
{ }

template<typename T>
DefaultPVArray<T>::~DefaultPVArray()
{
    delete[] value;
}

template<typename T>
void DefaultPVArray<T>::setCapacity(int capacity)
{
    if(PVArray::getCapacity()==capacity) return;
    if(!PVArray::isCapacityMutable()) {
        std::string message("not capacityMutable");
        PVField::message(message, errorMessage);
        return;
    }
    int length = PVArray::getLength();
    if(length>capacity) length = capacity;
    T *newValue = new T[capacity];
    for(int i=0; i<length; i++) newValue[i] = value[i];
    delete[]value;
    value = newValue;
    PVArray::setCapacityLength(capacity,length);
}

template<typename T>
int DefaultPVArray<T>::get(int offset, int len, PVArrayData<T> *data)
{
    int n = len;
    int length = PVArray::getLength();
    if(offset+len > length) {
        n = length-offset;
        if(n<0) n = 0;
    }
    data->data = value;
    data->offset = offset;
    return n;
}

template<typename T>
int DefaultPVArray<T>::put(int offset,int len,
    pointer from,int fromOffset)
{
    if(PVField::isImmutable()) {
        PVField::message("field is immutable",errorMessage);
        return 0;
    }
    if(from==value) return len;
    if(len<1) return 0;
    int length = PVArray::getLength();
    int capacity = PVArray::getCapacity();
    if(offset+len > length) {
        int newlength = offset + len;
        if(newlength>capacity) {
            setCapacity(newlength);
            newlength = PVArray::getCapacity();
            len = newlength - offset;
            if(len<=0) return 0;
        }
        length = newlength;
    }
    for(int i=0;i<len;i++) {
       value[i+offset] = from[i+fromOffset];
    }
    PVArray::setLength(length);
    this->postPut();
    return len;
}

template<typename T>
void DefaultPVArray<T>::shareData(pointer shareValue,int capacity,int length)
{
    delete[] value;
    value = shareValue;
    PVArray::setCapacityLength(capacity,length);
}

template<typename T>
void DefaultPVArray<T>::serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher) {
    serialize(pbuffer, pflusher, 0, PVArray::getLength());
}

template<typename T>
bool DefaultPVArray<T>::operator==(PVField& pv)
{
    return getConvert()->equals(this, &pv);
}

template<typename T>
bool DefaultPVArray<T>::operator!=(PVField& pv)
{
    return !(getConvert()->equals(this, &pv));
}

// specializations for String


#define DEFAULTPVARRAY_SERIALIZATION(T, NAME) \
template<> \
void DefaultPVArray<T>::deserialize(ByteBuffer *pbuffer, \
        DeserializableControl *pcontrol) { \
    int size = SerializeHelper::readSize(pbuffer, pcontrol); \
    if(size>=0) { \
        if(size>getCapacity()) setCapacity(size); \
        int i = 0; \
        while(true) { \
            int maxIndex = std::min(size-i, pbuffer->getRemaining())+i; \
            for(; i<maxIndex; i++) \
                value[i] = pbuffer->get ## NAME (); \
            if(i<size) \
                pcontrol->ensureData(sizeof(T)); \
            else \
                break; \
        } \
        PVArray::setLength(size); \
        PVField::postPut(); \
    } \
} \
\
template<> \
void DefaultPVArray<T>::serialize(ByteBuffer *pbuffer, \
        SerializableControl *pflusher, int offset, int count) { \
    int length = getLength(); \
\
    if(offset<0) \
        offset = 0; \
    else if(offset>length) offset = length; \
    if(count<0) count = length; \
\
    int maxCount = length-offset; \
    if(count>maxCount) count = maxCount; \
\
    SerializeHelper::writeSize(count, pbuffer, pflusher); \
    int end = offset+count; \
    int i = offset; \
    while(true) { \
        int maxIndex = std::min<int>(end-i, pbuffer->getRemaining())+i; \
        for(; i<maxIndex; i++) \
            pbuffer->put ## NAME (value[i]); \
        if(i<end) \
            pflusher->flushSerializeBuffer(); \
        else \
            break; \
    } \
}


DEFAULTPVARRAY_SERIALIZATION(bool, Boolean);
DEFAULTPVARRAY_SERIALIZATION(int8, Byte);
DEFAULTPVARRAY_SERIALIZATION(int16, Short);
DEFAULTPVARRAY_SERIALIZATION(int32, Int);
DEFAULTPVARRAY_SERIALIZATION(int64, Long);
DEFAULTPVARRAY_SERIALIZATION(float, Float);
DEFAULTPVARRAY_SERIALIZATION(double, Double);


// TODO null arrays (size == -1) not supported



template<>
void DefaultPVArray<String>::deserialize(ByteBuffer *pbuffer,
        DeserializableControl *pcontrol) {
    int size = SerializeHelper::readSize(pbuffer, pcontrol);
    if(size>=0) {
        // prepare array, if necessary
        if(size>getCapacity()) setCapacity(size);
        // retrieve value from the buffer
        for(int i = 0; i<size; i++)
            value[i] = SerializeHelper::deserializeString(pbuffer,
                    pcontrol);
        // set new length
        setLength(size);
        postPut();
    }
}

template<>
void DefaultPVArray<String>::serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher, int offset, int count) {
    int length = getLength();

    // check bounds
    if(offset<0)
        offset = 0;
    else if(offset>length) offset = length;
    if(count<0) count = length;

    int maxCount = length-offset;
    if(count>maxCount) count = maxCount;

    // write
    SerializeHelper::writeSize(count, pbuffer, pflusher);
    int end = offset+count;
    for(int i = offset; i<end; i++)
        SerializeHelper::serializeString(value[i], pbuffer, pflusher);
}

typedef DefaultPVArray<bool> DefaultPVBooleanArray;
typedef DefaultPVArray<int8> BasePVByteArray;
typedef DefaultPVArray<int16> BasePVShortArray;
typedef DefaultPVArray<int32> BasePVIntArray;
typedef DefaultPVArray<int64> BasePVLongArray;
typedef DefaultPVArray<float> BasePVFloatArray;
typedef DefaultPVArray<double> BasePVDoubleArray;
typedef DefaultPVArray<String> BasePVStringArray;

// Factory

PVDataCreate::PVDataCreate(){ }

PVField *PVDataCreate::createPVField(PVStructure *parent,
        FieldConstPtr field)
{
     switch(field->getType()) {
     case scalar:
         return createPVScalar(parent,(ScalarConstPtr)field);
     case scalarArray:
         return (PVField *)createPVScalarArray(parent,
             (ScalarArrayConstPtr)field);
     case structure:
         return (PVField *)createPVStructure(parent,
             (StructureConstPtr)field);
     case structureArray:
         return createPVStructureArray(parent,
             (StructureArrayConstPtr)field);
     }
     String  message("PVDataCreate::createPVField should never get here");
     throw std::logic_error(message);
}

PVField *PVDataCreate::createPVField(PVStructure *parent,
        String fieldName,PVField * fieldToClone)
{
     switch(fieldToClone->getField()->getType()) {
     case scalar:
         return createPVScalar(parent,fieldName,(PVScalar*)fieldToClone);
     case scalarArray:
         return (PVField *)createPVScalarArray(parent,fieldName,
             (PVScalarArray *)fieldToClone);
     case structure:
         return (PVField *)createPVStructure(parent,fieldName,
             (PVStructure *)fieldToClone);
     case structureArray:
        String message(
        "PVDataCreate::createPVField structureArray not valid fieldToClone");
        throw std::invalid_argument(message);
     }
     String  message("PVDataCreate::createPVField should never get here");
     throw std::logic_error(message);
}

PVScalar *PVDataCreate::createPVScalar(PVStructure *parent,ScalarConstPtr scalar)
{
     ScalarType scalarType = scalar->getScalarType();
     switch(scalarType) {
     case pvBoolean:
         return new BasePVBoolean(parent,scalar);
     case pvByte:
         return new BasePVByte(parent,scalar);
     case pvShort:
         return new BasePVShort(parent,scalar);
     case pvInt:
         return new BasePVInt(parent,scalar);
     case pvLong:
         return new BasePVLong(parent,scalar);
     case pvFloat:
         return new BasePVFloat(parent,scalar);
     case pvDouble:
         return new BasePVDouble(parent,scalar);
     case pvString:
         return new BasePVString(parent,scalar);
     }
     String  message("PVDataCreate::createPVScalar should never get here");
     throw std::logic_error(message);
}

PVScalar *PVDataCreate::createPVScalar(PVStructure *parent,
        String fieldName,ScalarType scalarType)
{
     ScalarConstPtr scalar = fieldCreate->createScalar(fieldName,scalarType);
     return createPVScalar(parent,scalar);
}


PVScalar *PVDataCreate::createPVScalar(PVStructure *parent,
        String fieldName,PVScalar * scalarToClone)
{
     scalarToClone->getField()->incReferenceCount();
     PVScalar *pvScalar = createPVScalar(parent,fieldName,
         scalarToClone->getScalar()->getScalarType());
     convert->copyScalar(scalarToClone, pvScalar);
     PVScalarMap attributes = scalarToClone->getPVAuxInfo()->getInfos();
     PVAuxInfo *pvAttribute = pvScalar->getPVAuxInfo();
     PVScalarMapIter p;
     for(p=attributes.begin(); p!=attributes.end(); p++) {
          String key = p->first;
          PVScalar *fromAttribute = p->second;
          PVScalar *toAttribute = pvAttribute->createInfo(key,
              fromAttribute->getScalar()->getScalarType());
          convert->copyScalar(fromAttribute,toAttribute);
     }
     return pvScalar;
}

PVScalarArray *PVDataCreate::createPVScalarArray(PVStructure *parent,
        ScalarArrayConstPtr scalarArray)
{
     switch(scalarArray->getElementType()) {
     case pvBoolean:
           return new DefaultPVBooleanArray(parent,scalarArray);
     case pvByte:
           return new BasePVByteArray(parent,scalarArray);
     case pvShort:
           return new BasePVShortArray(parent,scalarArray);
     case pvInt:
           return new BasePVIntArray(parent,scalarArray);
     case pvLong:
           return new BasePVLongArray(parent,scalarArray);
     case pvFloat:
           return new BasePVFloatArray(parent,scalarArray);
     case pvDouble:
           return new BasePVDoubleArray(parent,scalarArray);
     case pvString:
           return new BasePVStringArray(parent,scalarArray);
     }
     String  message("PVDataCreate::createPVScalarArray should never get here");
     throw std::logic_error(message);
     
}

PVScalarArray *PVDataCreate::createPVScalarArray(PVStructure *parent,
        String fieldName,ScalarType elementType)
{
     return createPVScalarArray(parent,
         fieldCreate->createScalarArray(fieldName, elementType));
}

PVScalarArray *PVDataCreate::createPVScalarArray(PVStructure *parent,
        String fieldName,PVScalarArray * arrayToClone)
{
     arrayToClone->getField()->incReferenceCount();
     PVScalarArray *pvArray = createPVScalarArray(parent,fieldName,
          arrayToClone->getScalarArray()->getElementType());
     convert->copyScalarArray(arrayToClone,0, pvArray,0,arrayToClone->getLength());
     PVScalarMap attributes = arrayToClone->getPVAuxInfo()->getInfos();
     PVAuxInfo *pvAttribute = pvArray->getPVAuxInfo();
     PVScalarMapIter p;
     for(p=attributes.begin(); p!=attributes.end(); p++) {
          String key = p->first;
          PVScalar *fromAttribute = p->second;
          PVScalar *toAttribute = pvAttribute->createInfo(key,
              fromAttribute->getScalar()->getScalarType());
          convert->copyScalar(fromAttribute,toAttribute);
     }
    return pvArray;
}

PVStructureArray *PVDataCreate::createPVStructureArray(PVStructure *parent,
        StructureArrayConstPtr structureArray)
{
     return new BasePVStructureArray(parent,structureArray);
}

PVStructure *PVDataCreate::createPVStructure(PVStructure *parent,
        StructureConstPtr structure)
{
     PVStructure *pvStructure = new BasePVStructure(parent,structure);
     return pvStructure;
}

PVStructure *PVDataCreate::createPVStructure(PVStructure *parent,
        String fieldName,int numberFields,FieldConstPtrArray fields)
{
     StructureConstPtr structure = fieldCreate->createStructure(
         fieldName,numberFields, fields);
     return new BasePVStructure(parent,structure);
}

PVStructure *PVDataCreate::createPVStructure(PVStructure *parent,
        String fieldName,PVStructure *structToClone)
{
    FieldConstPtrArray fields = 0;
    int numberFields = 0;
    PVStructure *pvStructure = 0;;
    if(structToClone==0) {
        fields = new FieldConstPtr[0];
        StructureConstPtr structure = fieldCreate->createStructure(
            fieldName,numberFields,fields);
        pvStructure = new BasePVStructure(parent,structure);
    } else {
       StructureConstPtr structure = structToClone->getStructure();
       structure->incReferenceCount();
       pvStructure = new BasePVStructure(parent,structure);
       convert->copyStructure(structToClone,pvStructure);
    }
    return pvStructure;
}

 PVDataCreate * getPVDataCreate() {
     static Mutex mutex;
     Lock xx(&mutex);

     if(pvDataCreate==0){
          pvDataCreate = new PVDataCreate();
          convert = getConvert();
          fieldCreate = getFieldCreate();
     }
     return pvDataCreate;
 }

}}
