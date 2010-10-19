/*BasePVIntArray.h*/
#ifndef BASEPVINTARRAY_H
#define BASEPVINTARRAY_H
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <epicsTypes.h>
#include "pvData.h"
#include "factory.h"
#include "AbstractPVScalarArray.h"

namespace epics { namespace pvData {

    PVIntArray::~PVIntArray() {}

    PVIntArray::PVIntArray(PVStructure *parent,ScalarArrayConstPtr scalar)
    : PVScalarArray(parent,scalar) {}

    class BasePVIntArray : public PVIntArray {
    public:
        BasePVIntArray(PVStructure *parent,ScalarArrayConstPtr scalarArray);
        virtual ~BasePVIntArray();
        virtual void setCapacity(int capacity);
        virtual int get(int offset, int length, IntArrayData *data) ;
        virtual int put(int offset,int length,IntArray from,
           int fromOffset);
        virtual void shareData(epicsInt32 value[],int capacity,int length);
        // from Serializable
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) ;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher);
        virtual void serialize(ByteBuffer *pbuffer,
             SerializableControl *pflusher, int offset, int count) ;
        virtual void toString(StringBuilder buf);
        virtual void toString(StringBuilder buf,int indentLevel);
        virtual bool operator==(PVField  *pv) ;
    private:
        epicsInt32 *value;
    };

    BasePVIntArray::BasePVIntArray(PVStructure *parent,
        ScalarArrayConstPtr scalarArray)
    : PVIntArray(parent,scalarArray),value(new epicsInt32[0])
    { } 

    BasePVIntArray::~BasePVIntArray()
    {
        delete[] value;
    }

    void BasePVIntArray::setCapacity(int capacity)
    {
        if(PVArray::getCapacity()==capacity) return;
        if(!PVArray::isCapacityMutable()) {
            std::string message("not capacityMutable");
            PVField::message(message, errorMessage);
            return;
        }
        int length = PVArray::getLength();
        if(length>capacity) length = capacity;
        epicsInt32 *newValue = new epicsInt32[capacity]; 
        for(int i=0; i<length; i++) newValue[i] = value[i];
        delete[]value;
        value = newValue;
        PVArray::setCapacityLength(capacity,length);
    }

    int BasePVIntArray::get(int offset, int len, IntArrayData *data) 
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

    int BasePVIntArray::put(int offset,int len,
        IntArray from,int fromOffset)
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

    void BasePVIntArray::shareData(
        epicsInt32 shareValue[],int capacity,int length)
    {
        delete[] value;
        value = shareValue;
        PVArray::setCapacityLength(capacity,length);
    }

    void BasePVIntArray::serialize(ByteBuffer *pbuffer,
         SerializableControl *pflusher) 
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVIntArray::deserialize(ByteBuffer *pbuffer,
         DeserializableControl *pflusher)
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVIntArray::serialize(ByteBuffer *pbuffer,
         SerializableControl *pflusher, int offset, int count) 
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVIntArray::toString(StringBuilder buf)
    {
        toString(buf,1);
    }

    void BasePVIntArray::toString(StringBuilder buf,int indentLevel)
    {
        getConvert()->getString(buf,this,indentLevel);

        PVField::toString(buf,indentLevel);
    }

    bool BasePVIntArray::operator==(PVField  *pv) 
    {
        return getConvert()->equals(this,pv);
    }
}}
#endif  /* BASEPVINTARRAY_H */
