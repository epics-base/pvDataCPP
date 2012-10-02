/*PVDataCreateFactory.cpp*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#ifdef _WIN32
#define NOMINMAX
#endif
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <pv/lock.h>
#include <pv/pvIntrospect.h>
#include <pv/pvData.h>
#include <pv/convert.h>
#include <pv/factory.h>
#include <pv/serializeHelper.h>

using std::tr1::static_pointer_cast;
using std::size_t;
using std::min;

namespace epics { namespace pvData {

static ConvertPtr convert = getConvert();
static FieldCreatePtr fieldCreate = getFieldCreate();

/** Default storage for scalar values
 */
template<typename T>
class BasePVScalar : public PVScalarValue<T> {
public:
    typedef T  value_type;
    typedef T* pointer;
    typedef const T* const_pointer;

    BasePVScalar(ScalarConstPtr const & scalar);
    virtual ~BasePVScalar();
    virtual T get() const ;
    virtual void put(T val);
    virtual void serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher) const;
    virtual void deserialize(ByteBuffer *pbuffer,
        DeserializableControl *pflusher);
private:
    T value;
};

template<typename T>
BasePVScalar<T>::BasePVScalar(ScalarConstPtr const & scalar)
    : PVScalarValue<T>(scalar),value(0)
{}
//Note: '0' is a suitable default for all POD types (not String)

template<typename T>
BasePVScalar<T>::~BasePVScalar() {}

template<typename T>
T BasePVScalar<T>::get() const  { return value;}

template<typename T>
void BasePVScalar<T>::put(T val){value = val;}

template<typename T>
void BasePVScalar<T>::serialize(ByteBuffer *pbuffer,
    SerializableControl *pflusher) const {
    pflusher->ensureBuffer(sizeof(T));
    pbuffer->put(value);
}

template<typename T>
void BasePVScalar<T>::deserialize(ByteBuffer *pbuffer,
    DeserializableControl *pflusher)
{
    pflusher->ensureData(sizeof(T));
    value = pbuffer->GET(T);
}

typedef BasePVScalar<boolean> BasePVBoolean;
typedef BasePVScalar<int8> BasePVByte;
typedef BasePVScalar<int16> BasePVShort;
typedef BasePVScalar<int32> BasePVInt;
typedef BasePVScalar<int64> BasePVLong;
typedef BasePVScalar<uint8> BasePVUByte;
typedef BasePVScalar<uint16> BasePVUShort;
typedef BasePVScalar<uint32> BasePVUInt;
typedef BasePVScalar<uint64> BasePVULong;
typedef BasePVScalar<float> BasePVFloat;
typedef BasePVScalar<double> BasePVDouble;

// BasePVString is special case, since it implements SerializableArray
class BasePVString : public PVString {
public:
    typedef String value_type;
    typedef String* pointer;
    typedef const String* const_pointer;

    BasePVString(ScalarConstPtr const & scalar);
    virtual ~BasePVString();
    virtual String get() const ;
    virtual void put(String val);
    virtual void serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher) const;
    virtual void deserialize(ByteBuffer *pbuffer,
        DeserializableControl *pflusher);
    virtual void serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher, size_t offset, size_t count) const;
private:
    String value;
};

BasePVString::BasePVString(ScalarConstPtr const & scalar)
    : PVString(scalar),value()
{}

BasePVString::~BasePVString() {}

String BasePVString::get() const  { return value;}

void BasePVString::put(String val){value = val;}

void BasePVString::serialize(ByteBuffer *pbuffer,
    SerializableControl *pflusher) const
{
    SerializeHelper::serializeString(value, pbuffer, pflusher);
}

void BasePVString::deserialize(ByteBuffer *pbuffer,
    DeserializableControl *pflusher)
{
    value = SerializeHelper::deserializeString(pbuffer, pflusher);
}

void BasePVString::serialize(ByteBuffer *pbuffer,
    SerializableControl *pflusher, size_t offset, size_t count) const
{
	// check bounds
	const size_t length = /*(value == null) ? 0 :*/ value.length();
	if (offset < 0) offset = 0;
	else if (offset > length) offset = length;
	if (count < 0) count = length;

	const size_t maxCount = length - offset;
	if (count > maxCount)
		count = maxCount;
	
	// write
	SerializeHelper::serializeSubstring(value, offset, count, pbuffer, pflusher);
}

/** Default storage for arrays
 */
template<typename T>
class DefaultPVArray : public PVValueArray<T> {
public:
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef std::vector<T> vector;
    typedef const std::vector<T> const_vector;
    typedef std::tr1::shared_ptr<vector> shared_vector;

    DefaultPVArray(ScalarArrayConstPtr const & scalarArray);
    virtual ~DefaultPVArray();
    virtual void setCapacity(size_t capacity);
    virtual void setLength(size_t length);
    virtual size_t get(size_t offset, size_t length, PVArrayData<T> &data) ;
    virtual size_t put(size_t offset,size_t length, const_pointer from,
       size_t fromOffset);
    virtual void shareData(
        std::tr1::shared_ptr<std::vector<T> > const & value,
         std::size_t capacity,
         std::size_t length);
    virtual pointer get() ;
    virtual pointer get() const ;
    virtual vector const & getVector() { return *value.get(); }
    virtual shared_vector const & getSharedVector(){return value;};
    // from Serializable
    virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) const;
    virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher);
    virtual void serialize(ByteBuffer *pbuffer,
         SerializableControl *pflusher, size_t offset, size_t count) const;
private:
    shared_vector value;
};

template<typename T>
T *DefaultPVArray<T>::get() 
{
     std::vector<T> *vec = value.get();
     T *praw = &((*vec)[0]);
     return praw;
}

template<typename T>
T *DefaultPVArray<T>::get() const
{
     std::vector<T> *vec = value.get();
     T *praw = &((*vec)[0]);
     return praw;
}


template<typename T>
DefaultPVArray<T>::DefaultPVArray(ScalarArrayConstPtr const & scalarArray)
: PVValueArray<T>(scalarArray),
    value(std::tr1::shared_ptr<std::vector<T> >(new std::vector<T>()))
  
{ }

template<typename T>
DefaultPVArray<T>::~DefaultPVArray()
{ }

template<typename T>
void DefaultPVArray<T>::setCapacity(size_t capacity)
{
    if(PVArray::getCapacity()==capacity) return;
    if(!PVArray::isCapacityMutable()) {
        std::string message("not capacityMutable");
        PVField::message(message, errorMessage);
        return;
    }
    size_t length = PVArray::getLength();
    if(length>capacity) length = capacity;
    size_t oldCapacity = PVArray::getCapacity();
    if(oldCapacity>capacity) {
        std::vector<T> array;
        array.reserve(capacity);
        array.resize(length);
        T * from = get();
        for (size_t i=0; i<length; i++) array[i] = from[i];
        value->swap(array);
    } else {
        value->reserve(capacity);
    }
    PVArray::setCapacityLength(capacity,length);
}

template<typename T>
void DefaultPVArray<T>::setLength(size_t length)
{
    if(PVArray::getLength()==length) return;
    size_t capacity = PVArray::getCapacity();
    if(length>capacity) {
        if(!PVArray::isCapacityMutable()) {
            std::string message("not capacityMutable");
            PVField::message(message, errorMessage);
            return;
        }
        setCapacity(length);
    }
    value->resize(length);
    PVArray::setCapacityLength(capacity,length);
}

template<typename T>
size_t DefaultPVArray<T>::get(size_t offset, size_t len, PVArrayData<T> &data)
{
    size_t n = len;
    size_t length = this->getLength();
    if(offset+len > length) {
        n = length-offset;
        if(n<0) n = 0;
    }
    data.data = *value.get();
    data.offset = offset;
    return n;
}

template<typename T>
size_t DefaultPVArray<T>::put(size_t offset,size_t len,
    const_pointer from,size_t fromOffset)
{
    if(PVField::isImmutable()) {
        PVField::message("field is immutable",errorMessage);
        return 0;
    }
    T * pvalue = get();
    if(from==pvalue) return len;
    if(len<1) return 0;
    size_t length = this->getLength();
    size_t capacity = this->getCapacity();
    if(offset+len > length) {
        size_t newlength = offset + len;
        if(newlength>capacity) {
            setCapacity(newlength);
            newlength = this->getCapacity();
            len = newlength - offset;
            if(len<=0) return 0;
        }
        length = newlength;
        setLength(length);
    }
    pvalue = get();
    for(size_t i=0;i<len;i++) {
       pvalue[i+offset] = from[i+fromOffset];
    }
    this->setLength(length);
    this->postPut();
    return len;
}

template<typename T>
void DefaultPVArray<T>::shareData(
         std::tr1::shared_ptr<std::vector<T> > const & sharedValue,
         std::size_t capacity,
         std::size_t length)
{
    value = sharedValue;
    PVArray::setCapacityLength(capacity,length);
}

template<typename T>
void DefaultPVArray<T>::serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher) const {
    serialize(pbuffer, pflusher, 0, this->getLength());
}

template<typename T>
void DefaultPVArray<T>::deserialize(ByteBuffer *pbuffer,
        DeserializableControl *pcontrol) {
    size_t size = SerializeHelper::readSize(pbuffer, pcontrol);
    // if (size>0) { pcontrol->ensureData(sizeof(T)-1); pbuffer->align(sizeof(T)); }
    if(size>=0) {
        // prepare array, if necessary
        if(size>this->getCapacity()) this->setCapacity(size);
        // set new length
        this->setLength(size);
        // retrieve value from the buffer
        size_t i = 0;
        while(true) {
            /*
            size_t maxIndex = min(size-i, (int)(pbuffer->getRemaining()/sizeof(T)))+i;
            for(; i<maxIndex; i++)
                value[i] = pbuffer->get<T>();
              */  
            size_t maxCount = min(size-i, (pbuffer->getRemaining()/sizeof(T)));
            pbuffer->getArray(get()+i, maxCount);
            i += maxCount;
            
            if(i<size)
                pcontrol->ensureData(sizeof(T)); // this is not OK since can exceen max local buffer (size-i)*sizeof(T));
            else
                break;
        }
        // inform about the change?
        PVField::postPut();
    }
    // TODO null arrays (size == -1) not supported
}

template<typename T>
void DefaultPVArray<T>::serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher, size_t offset, size_t count) const {
    // cache
    size_t length = this->getLength();

    // check bounds
    if(offset<0)
        offset = 0;
    else if(offset>length) offset = length;
    if(count<0) count = length;

    size_t maxCount = length-offset;
    if(count>maxCount) count = maxCount;

    // write
    SerializeHelper::writeSize(count, pbuffer, pflusher);
    //if (count == 0) return; pcontrol->ensureData(sizeof(T)-1); pbuffer->align(sizeof(T));
    size_t end = offset+count;
    size_t i = offset;
    while(true) {
        
        /*
        size_t maxIndex = min<int>(end-i, (int)(pbuffer->getRemaining()/sizeof(T)))+i;
        for(; i<maxIndex; i++)
            pbuffer->put<T>(value[i]);
        */
        
        size_t maxCount = min<int>(end-i, (int)(pbuffer->getRemaining()/sizeof(T)));
        T * pvalue = const_cast<T *>(get());
        pbuffer->putArray(pvalue+i, maxCount);
        i += maxCount;
        
        if(i<end)
            pflusher->flushSerializeBuffer();
        else
            break;
    }
}

// specializations for String

template<>
void DefaultPVArray<String>::deserialize(ByteBuffer *pbuffer,
        DeserializableControl *pcontrol) {
    size_t size = SerializeHelper::readSize(pbuffer, pcontrol);
    if(size>=0) {
        // prepare array, if necessary
        if(size>getCapacity()) setCapacity(size);
        // set new length
        setLength(size);
        // retrieve value from the buffer
        String * pvalue = get();
        for(size_t i = 0; i<size; i++) {
            pvalue[i] = SerializeHelper::deserializeString(pbuffer,
                    pcontrol);
        }
        // inform about the change?
        postPut();
    }
    // TODO null arrays (size == -1) not supported
}

template<>
void DefaultPVArray<String>::serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher, size_t offset, size_t count) const {
    size_t length = getLength();

    // check bounds
    if(offset<0)
        offset = 0;
    else if(offset>length) offset = length;
    if(count<0) count = length;

    size_t maxCount = length-offset;
    if(count>maxCount) count = maxCount;

    // write
    SerializeHelper::writeSize(count, pbuffer, pflusher);
    size_t end = offset+count;
    String * pvalue = get();
    for(size_t i = offset; i<end; i++) {
        SerializeHelper::serializeString(pvalue[i], pbuffer, pflusher);
    }
}

typedef DefaultPVArray<boolean> DefaultPVBooleanArray;
typedef DefaultPVArray<int8> BasePVByteArray;
typedef DefaultPVArray<int16> BasePVShortArray;
typedef DefaultPVArray<int32> BasePVIntArray;
typedef DefaultPVArray<int64> BasePVLongArray;
typedef DefaultPVArray<uint8> BasePVUByteArray;
typedef DefaultPVArray<uint16> BasePVUShortArray;
typedef DefaultPVArray<uint32> BasePVUIntArray;
typedef DefaultPVArray<uint64> BasePVULongArray;
typedef DefaultPVArray<float> BasePVFloatArray;
typedef DefaultPVArray<double> BasePVDoubleArray;
typedef DefaultPVArray<String> BasePVStringArray;

// Factory

PVDataCreate::PVDataCreate(){ }

PVFieldPtr PVDataCreate::createPVField(FieldConstPtr const & field)
{
     switch(field->getType()) {
     case scalar: {
         ScalarConstPtr xx = static_pointer_cast<const Scalar>(field);
         return createPVScalar(xx);
     }
     case scalarArray: {
         ScalarArrayConstPtr xx = static_pointer_cast<const ScalarArray>(field);
         return createPVScalarArray(xx);
     }
     case structure: {
         StructureConstPtr xx = static_pointer_cast<const Structure>(field);
         return createPVStructure(xx);
     }
     case structureArray: {
         StructureArrayConstPtr xx = static_pointer_cast<const StructureArray>(field);
         return createPVStructureArray(xx);
     }
     }
     throw std::logic_error("PVDataCreate::createPVField should never get here");
}

PVFieldPtr PVDataCreate::createPVField(PVFieldPtr const & fieldToClone)
{
     switch(fieldToClone->getField()->getType()) {
     case scalar:
        {
            PVScalarPtr pvScalar = static_pointer_cast<PVScalar>(fieldToClone);
            return createPVScalar(pvScalar);
        }
     case scalarArray:
        {
             PVScalarArrayPtr pvScalarArray
                 = static_pointer_cast<PVScalarArray>(fieldToClone);
             return createPVScalarArray(pvScalarArray);
        }
     case structure:
         {
             PVStructurePtr pvStructure
                   = static_pointer_cast<PVStructure>(fieldToClone);
             StringArray fieldNames = pvStructure->getStructure()->getFieldNames();
             PVFieldPtrArray pvFieldPtrArray = pvStructure->getPVFields();
             return createPVStructure(fieldNames,pvFieldPtrArray);
         }
     case structureArray:
         {
             PVStructureArrayPtr from
                 = static_pointer_cast<PVStructureArray>(fieldToClone);
             StructureArrayConstPtr structureArray = from->getStructureArray();
             PVStructureArrayPtr to = createPVStructureArray(
                 structureArray);
             convert->copyStructureArray(from, to);
             return to;
         }
     }
     throw std::logic_error("PVDataCreate::createPVField should never get here");
}

PVScalarPtr PVDataCreate::createPVScalar(ScalarConstPtr const & scalar)
{
     ScalarType scalarType = scalar->getScalarType();
     switch(scalarType) {
     case pvBoolean:
         return PVScalarPtr(new BasePVBoolean(scalar));
     case pvByte:
         return PVScalarPtr(new BasePVByte(scalar));
     case pvShort:
         return PVScalarPtr(new BasePVShort(scalar));
     case pvInt:
         return PVScalarPtr(new BasePVInt(scalar));
     case pvLong:
         return PVScalarPtr(new BasePVLong(scalar));
     case pvUByte:
         return PVScalarPtr(new BasePVUByte(scalar));
     case pvUShort:
         return PVScalarPtr(new BasePVUShort(scalar));
     case pvUInt:
         return PVScalarPtr(new BasePVUInt(scalar));
     case pvULong:
         return PVScalarPtr(new BasePVULong(scalar));
     case pvFloat:
         return PVScalarPtr(new BasePVFloat(scalar));
     case pvDouble:
         return PVScalarPtr(new BasePVDouble(scalar));
     case pvString:
         return PVScalarPtr(new BasePVString(scalar));
     }
     throw std::logic_error("PVDataCreate::createPVScalar should never get here");
}

PVScalarPtr PVDataCreate::createPVScalar(ScalarType scalarType)
{
     ScalarConstPtr scalar = fieldCreate->createScalar(scalarType);
     return createPVScalar(scalar);
}


PVScalarPtr PVDataCreate::createPVScalar(PVScalarPtr const & scalarToClone)
{
     ScalarType scalarType = scalarToClone->getScalar()->getScalarType();
     PVScalarPtr pvScalar = createPVScalar(scalarType);
     convert->copyScalar(scalarToClone, pvScalar);
     PVAuxInfoPtr from = scalarToClone->getPVAuxInfo();
     PVAuxInfoPtr to = pvScalar->getPVAuxInfo();
     PVAuxInfo::PVInfoMap & map = from->getInfoMap();
     for(PVAuxInfo::PVInfoIter iter = map.begin(); iter!= map.end(); ++iter) {
         String key = iter->first;
         PVScalarPtr pvFrom = iter->second;
         ScalarConstPtr scalar = pvFrom->getScalar();
         PVScalarPtr pvTo = to->createInfo(key,scalar->getScalarType());
         convert->copyScalar(pvFrom,pvTo);
     }
     return pvScalar;
}

PVScalarArrayPtr PVDataCreate::createPVScalarArray(
        ScalarArrayConstPtr const & scalarArray)
{
     switch(scalarArray->getElementType()) {
     case pvBoolean:
           return PVScalarArrayPtr(new DefaultPVBooleanArray(scalarArray));
     case pvByte:
           return PVScalarArrayPtr(new BasePVByteArray(scalarArray));
     case pvShort:
           return PVScalarArrayPtr(new BasePVShortArray(scalarArray));
     case pvInt:
           return PVScalarArrayPtr(new BasePVIntArray(scalarArray));
     case pvLong:
           return PVScalarArrayPtr(new BasePVLongArray(scalarArray));
     case pvUByte:
           return PVScalarArrayPtr(new BasePVUByteArray(scalarArray));
     case pvUShort:
           return PVScalarArrayPtr(new BasePVUShortArray(scalarArray));
     case pvUInt:
           return PVScalarArrayPtr(new BasePVUIntArray(scalarArray));
     case pvULong:
           return PVScalarArrayPtr(new BasePVULongArray(scalarArray));
     case pvFloat:
           return PVScalarArrayPtr(new BasePVFloatArray(scalarArray));
     case pvDouble:
           return PVScalarArrayPtr(new BasePVDoubleArray(scalarArray));
     case pvString:
           return PVScalarArrayPtr(new BasePVStringArray(scalarArray));
     }
     throw std::logic_error("PVDataCreate::createPVScalarArray should never get here");
     
}

PVScalarArrayPtr PVDataCreate::createPVScalarArray(
        ScalarType elementType)
{
     ScalarArrayConstPtr scalarArray = fieldCreate->createScalarArray(elementType);
     return createPVScalarArray(scalarArray);
}

PVScalarArrayPtr PVDataCreate::createPVScalarArray(
        PVScalarArrayPtr const & arrayToClone)
{
     PVScalarArrayPtr pvArray = createPVScalarArray(
          arrayToClone->getScalarArray()->getElementType());
     convert->copyScalarArray(arrayToClone,0, pvArray,0,arrayToClone->getLength());
     PVAuxInfoPtr from = arrayToClone->getPVAuxInfo();
     PVAuxInfoPtr to = pvArray->getPVAuxInfo();
     PVAuxInfo::PVInfoMap & map = from->getInfoMap();
     for(PVAuxInfo::PVInfoIter iter = map.begin(); iter!= map.end(); ++iter) {
         String key = iter->first;
         PVScalarPtr pvFrom = iter->second;
         ScalarConstPtr scalar = pvFrom->getScalar();
         PVScalarPtr pvTo = to->createInfo(key,scalar->getScalarType());
         convert->copyScalar(pvFrom,pvTo);
     }
    return pvArray;
}

PVStructureArrayPtr PVDataCreate::createPVStructureArray(
        StructureArrayConstPtr const & structureArray)
{
     return PVStructureArrayPtr(new PVStructureArray(structureArray));
}

PVStructurePtr PVDataCreate::createPVStructure(
        StructureConstPtr const & structure)
{
     return PVStructurePtr(new PVStructure(structure));
}

PVStructurePtr PVDataCreate::createPVStructure(
        StringArray const & fieldNames,PVFieldPtrArray const & pvFields)
{
     size_t num = fieldNames.size();
     FieldConstPtrArray fields(num);
     for (size_t i=0;i<num;i++) fields[i] = pvFields[i]->getField();
     StructureConstPtr structure = fieldCreate->createStructure(fieldNames,fields);
     PVStructurePtr pvStructure(new PVStructure(structure,pvFields));
     return pvStructure;
}

PVStructurePtr PVDataCreate::createPVStructure(PVStructurePtr const & structToClone)
{
    FieldConstPtrArray field;
    if(structToClone==0) {
        FieldConstPtrArray fields(0);
        StringArray fieldNames(0);
        StructureConstPtr structure = fieldCreate->createStructure(fieldNames,fields);
        return PVStructurePtr(new PVStructure(structure));
    }
    StructureConstPtr structure = structToClone->getStructure();
    PVStructurePtr pvStructure(new PVStructure(structure));
    convert->copyStructure(structToClone,pvStructure);
    return pvStructure;
}

PVDataCreatePtr PVDataCreate::getPVDataCreate()
{
    static PVDataCreatePtr pvDataCreate;
    static Mutex mutex;
    Lock xx(mutex);

    if(pvDataCreate.get()==0) pvDataCreate = PVDataCreatePtr(new PVDataCreate());
    return pvDataCreate;
}

PVDataCreatePtr getPVDataCreate() {
     return PVDataCreate::getPVDataCreate();
}

}}
