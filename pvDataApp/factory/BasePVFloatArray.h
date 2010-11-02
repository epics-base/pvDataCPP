/*BasePVFloatArray.h*/
#ifndef BASEPVFLOATARRAY_H
#define BASEPVFLOATARRAY_H
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "pvData.h"
#include "factory.h"
#include "AbstractPVScalarArray.h"

namespace epics { namespace pvData {

    PVFloatArray::~PVFloatArray() {}

    PVFloatArray::PVFloatArray(PVStructure *parent,ScalarArrayConstPtr scalar)
    : PVScalarArray(parent,scalar) {}

    class BasePVFloatArray : public PVFloatArray {
    public:
        BasePVFloatArray(PVStructure *parent,ScalarArrayConstPtr scalarArray);
        virtual ~BasePVFloatArray();
        virtual void setCapacity(int capacity);
        virtual int get(int offset, int length, FloatArrayData *data) ;
        virtual int put(int offset,int length,FloatArray from,
           int fromOffset);
        virtual void shareData(float value[],int capacity,int length);
        // from Serializable
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) ;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher);
        virtual void serialize(ByteBuffer *pbuffer,
             SerializableControl *pflusher, int offset, int count) ;
        virtual void toString(StringBuilder buf);
        virtual void toString(StringBuilder buf,int indentLevel);
        virtual bool operator==(PVField  *pv) ;
        virtual bool operator!=(PVField  *pv) ;
    private:
        float *value;
    };

    BasePVFloatArray::BasePVFloatArray(PVStructure *parent,
        ScalarArrayConstPtr scalarArray)
    : PVFloatArray(parent,scalarArray),value(new float[0])
    { } 

    BasePVFloatArray::~BasePVFloatArray()
    {
        delete[] value;
    }

    void BasePVFloatArray::setCapacity(int capacity)
    {
        if(PVArray::getCapacity()==capacity) return;
        if(!PVArray::isCapacityMutable()) {
            std::string message("not capacityMutable");
            PVField::message(message, errorMessage);
            return;
        }
        int length = PVArray::getLength();
        if(length>capacity) length = capacity;
        float *newValue = new float[capacity]; 
        for(int i=0; i<length; i++) newValue[i] = value[i];
        delete[]value;
        value = newValue;
        PVArray::setCapacityLength(capacity,length);
    }

    int BasePVFloatArray::get(int offset, int len, FloatArrayData *data) 
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

    int BasePVFloatArray::put(int offset,int len,
        FloatArray from,int fromOffset)
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

    void BasePVFloatArray::shareData(
        float shareValue[],int capacity,int length)
    {
        delete[] value;
        value = shareValue;
        PVArray::setCapacityLength(capacity,length);
    }

    void BasePVFloatArray::serialize(ByteBuffer *pbuffer,
         SerializableControl *pflusher) 
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVFloatArray::deserialize(ByteBuffer *pbuffer,
         DeserializableControl *pflusher)
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVFloatArray::serialize(ByteBuffer *pbuffer,
         SerializableControl *pflusher, int offset, int count) 
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVFloatArray::toString(StringBuilder buf)
    {
        toString(buf,1);
    }

    void BasePVFloatArray::toString(StringBuilder buf,int indentLevel)
    {
        getConvert()->getString(buf,this,indentLevel);

        PVField::toString(buf,indentLevel);
    }

    bool BasePVFloatArray::operator==(PVField  *pv) 
    {
        return getConvert()->equals(this,pv);
    }

    bool BasePVFloatArray::operator!=(PVField  *pv) 
    {
        return !(getConvert()->equals(this,pv));
    }
}}
#endif  /* BASEPVFLOATARRAY_H */
