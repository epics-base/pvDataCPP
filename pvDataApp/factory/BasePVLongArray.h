/*BasePVLongArray.h*/
#ifndef BASEPVLONGARRAY_H
#define BASEPVLONGARRAY_H
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <epicsTypes.h>
#include "pvData.h"
#include "factory.h"
#include "AbstractPVScalarArray.h"

namespace epics { namespace pvData {

    PVLongArray::~PVLongArray() {}

    PVLongArray::PVLongArray(PVStructure *parent,ScalarArrayConstPtr scalar)
    : PVScalarArray(parent,scalar) {}

    class BasePVLongArray : public PVLongArray {
    public:
        BasePVLongArray(PVStructure *parent,ScalarArrayConstPtr scalarArray);
        virtual ~BasePVLongArray();
        virtual void setCapacity(int capacity);
        virtual int get(int offset, int length, LongArrayData *data) ;
        virtual int put(int offset,int length,LongArray from,
           int fromOffset);
        virtual void shareData(epicsInt64 value[],int capacity,int length);
        // from Serializable
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) ;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher);
        virtual void serialize(ByteBuffer *pbuffer,
             SerializableControl *pflusher, int offset, int count) ;
        virtual void toString(StringBuilder buf);
        virtual void toString(StringBuilder buf,int indentLevel);
        virtual epicsBoolean equals(PVField  *pv) ;
    private:
        epicsInt64 *value;
    };

    BasePVLongArray::BasePVLongArray(PVStructure *parent,
        ScalarArrayConstPtr scalarArray)
    : PVLongArray(parent,scalarArray),value(new epicsInt64[0])
    { } 

    BasePVLongArray::~BasePVLongArray()
    {
        delete[] value;
    }

    void BasePVLongArray::setCapacity(int capacity)
    {
        if(PVArray::getCapacity()==capacity) return;
        if(!PVArray::isCapacityMutable()) {
            std::string message("not capacityMutable");
            PVField::message(message, errorMessage);
            return;
        }
        int length = PVArray::getLength();
        if(length>capacity) length = capacity;
        epicsInt64 *newValue = new epicsInt64[capacity]; 
        for(int i=0; i<length; i++) newValue[i] = value[i];
        delete[]value;
        value = newValue;
        PVArray::setCapacityLength(capacity,length);
    }

    int BasePVLongArray::get(int offset, int len, LongArrayData *data) 
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

    int BasePVLongArray::put(int offset,int len,
        LongArray from,int fromOffset)
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
        PVField::postPut();
        return len;      
    }

    void BasePVLongArray::shareData(
        epicsInt64 shareValue[],int capacity,int length)
    {
        delete[] value;
        value = shareValue;
        PVArray::setCapacityLength(capacity,length);
    }

    void BasePVLongArray::serialize(ByteBuffer *pbuffer,
         SerializableControl *pflusher) 
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVLongArray::deserialize(ByteBuffer *pbuffer,
         DeserializableControl *pflusher)
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVLongArray::serialize(ByteBuffer *pbuffer,
         SerializableControl *pflusher, int offset, int count) 
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVLongArray::toString(StringBuilder buf)
    {
        toString(buf,1);
    }

    void BasePVLongArray::toString(StringBuilder buf,int indentLevel)
    {
        getConvert()->getString(buf,this,indentLevel);

        PVField::toString(buf,indentLevel);
    }

    epicsBoolean BasePVLongArray::equals(PVField  *pv) 
    {
        return getConvert()->equals(this,pv);
    }
}}
#endif  /* BASEPVLONGARRAY_H */
