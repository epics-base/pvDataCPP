/*BasePVShortArray.h*/
#ifndef BASEPVSHORTARRAY_H
#define BASEPVSHORTARRAY_H
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <epicsTypes.h>
#include "pvData.h"
#include "factory.h"
#include "AbstractPVScalarArray.h"

namespace epics { namespace pvData {

    PVShortArray::~PVShortArray() {}

    PVShortArray::PVShortArray(PVStructure *parent,ScalarArrayConstPtr scalar)
    : PVScalarArray(parent,scalar) {}

    class BasePVShortArray : public PVShortArray {
    public:
        BasePVShortArray(PVStructure *parent,ScalarArrayConstPtr scalarArray);
        virtual ~BasePVShortArray();
        virtual void setCapacity(int capacity);
        virtual int get(int offset, int length, ShortArrayData *data) ;
        virtual int put(int offset,int length,ShortArray from,
           int fromOffset);
        virtual void shareData(epicsInt16 value[],int capacity,int length);
        // from Serializable
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) ;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher);
        virtual void serialize(ByteBuffer *pbuffer,
             SerializableControl *pflusher, int offset, int count) ;
        virtual void toString(StringBuilder buf);
        virtual void toString(StringBuilder buf,int indentLevel);
        virtual bool operator==(PVField  *pv) ;
    private:
        epicsInt16 *value;
    };

    BasePVShortArray::BasePVShortArray(PVStructure *parent,
        ScalarArrayConstPtr scalarArray)
    : PVShortArray(parent,scalarArray),value(new epicsInt16[0])
    { } 

    BasePVShortArray::~BasePVShortArray()
    {
        delete[] value;
    }

    void BasePVShortArray::setCapacity(int capacity)
    {
        if(PVArray::getCapacity()==capacity) return;
        if(!PVArray::isCapacityMutable()) {
            std::string message("not capacityMutable");
            PVField::message(message, errorMessage);
            return;
        }
        int length = PVArray::getLength();
        if(length>capacity) length = capacity;
        epicsInt16 *newValue = new epicsInt16[capacity]; 
        for(int i=0; i<length; i++) newValue[i] = value[i];
        delete[]value;
        value = newValue;
        PVArray::setCapacityLength(capacity,length);
    }

    int BasePVShortArray::get(int offset, int len, ShortArrayData *data) 
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

    int BasePVShortArray::put(int offset,int len,
        ShortArray from,int fromOffset)
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

    void BasePVShortArray::shareData(
        epicsInt16 shareValue[],int capacity,int length)
    {
        delete[] value;
        value = shareValue;
        PVArray::setCapacityLength(capacity,length);
    }

    void BasePVShortArray::serialize(ByteBuffer *pbuffer,
         SerializableControl *pflusher) 
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVShortArray::deserialize(ByteBuffer *pbuffer,
         DeserializableControl *pflusher)
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVShortArray::serialize(ByteBuffer *pbuffer,
         SerializableControl *pflusher, int offset, int count) 
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVShortArray::toString(StringBuilder buf)
    {
        toString(buf,1);
    }

    void BasePVShortArray::toString(StringBuilder buf,int indentLevel)
    {
        getConvert()->getString(buf,this,indentLevel);

        PVField::toString(buf,indentLevel);
    }

    bool BasePVShortArray::operator==(PVField  *pv) 
    {
        return getConvert()->equals(this,pv);
    }
}}
#endif  /* BASEPVSHORTARRAY_H */
