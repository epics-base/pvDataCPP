/*BasePVStringArray.h*/
#ifndef BASEPVSTRINGARRAY_H
#define BASEPVSTRINGARRAY_H
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "pvData.h"
#include "factory.h"
#include "AbstractPVScalarArray.h"

namespace epics { namespace pvData {

    PVStringArray::~PVStringArray() {}

    PVStringArray::PVStringArray(PVStructure *parent,ScalarArrayConstPtr scalar)
    : PVScalarArray(parent,scalar) {}

    class BasePVStringArray : public PVStringArray {
    public:
        BasePVStringArray(PVStructure *parent,ScalarArrayConstPtr scalarArray);
        virtual ~BasePVStringArray();
        virtual void setCapacity(int capacity);
        virtual int get(int offset, int length, StringArrayData *data) ;
        virtual int put(int offset,int length,StringArray from,
           int fromOffset);
        virtual void shareData(String value[],int capacity,int length);
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
        String *value;
    };

    BasePVStringArray::BasePVStringArray(PVStructure *parent,
        ScalarArrayConstPtr scalarArray)
    : PVStringArray(parent,scalarArray),value(new String[0])
    { } 

    BasePVStringArray::~BasePVStringArray()
    {
        delete[] value;
    }

    void BasePVStringArray::setCapacity(int capacity)
    {
        if(PVArray::getCapacity()==capacity) return;
        if(!PVArray::isCapacityMutable()) {
            std::string message("not capacityMutable");
            PVField::message(message, errorMessage);
            return;
        }
        int length = PVArray::getLength();
        if(length>capacity) length = capacity;
        String *newValue = new String[capacity]; 
        for(int i=0; i<length; i++) newValue[i] = value[i];
        delete[]value;
        value = newValue;
        PVArray::setCapacityLength(capacity,length);
    }

    int BasePVStringArray::get(int offset, int len, StringArrayData *data) 
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

    int BasePVStringArray::put(int offset,int len,
        StringArray from,int fromOffset)
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

    void BasePVStringArray::shareData(
        String shareValue[],int capacity,int length)
    {
        delete[] value;
        value = shareValue;
        PVArray::setCapacityLength(capacity,length);
    }

    void BasePVStringArray::serialize(ByteBuffer *pbuffer,
         SerializableControl *pflusher) 
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVStringArray::deserialize(ByteBuffer *pbuffer,
         DeserializableControl *pflusher)
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVStringArray::serialize(ByteBuffer *pbuffer,
         SerializableControl *pflusher, int offset, int count) 
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVStringArray::toString(StringBuilder buf)
    {
        toString(buf,1);
    }

    void BasePVStringArray::toString(StringBuilder buf,int indentLevel)
    {
        getConvert()->getString(buf,this,indentLevel);

        PVField::toString(buf,indentLevel);
    }

    bool BasePVStringArray::operator==(PVField  *pv) 
    {
        return getConvert()->equals(this,pv);
    }

    bool BasePVStringArray::operator!=(PVField  *pv) 
    {
        return !(getConvert()->equals(this,pv));
    }
}}
#endif  /* BASEPVSTRINGARRAY_H */
