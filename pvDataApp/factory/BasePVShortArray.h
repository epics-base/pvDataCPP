/*BasePVShortArray.h*/
#ifndef BASEPVSHORTARRAY_H
#define BASEPVSHORTARRAY_H
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <algorithm>
#include <epicsTypes.h>
#include "pvData.h"
#include "factory.h"
#include "AbstractPVScalarArray.h"
#include "serializeHelper.h"

using std::min;

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
        virtual bool operator==(PVField& pv) ;
        virtual bool operator!=(PVField& pv) ;
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
            SerializableControl *pflusher) {
        serialize(pbuffer, pflusher, 0, getLength());
    }

    void BasePVShortArray::deserialize(ByteBuffer *pbuffer,
            DeserializableControl *pcontrol) {
        int size = SerializeHelper::readSize(pbuffer, pcontrol);
        if(size>=0) {
            // prepare array, if necessary
            if(size>getCapacity()) setCapacity(size);
            // retrieve value from the buffer
            int i = 0;
            while(true) {
                int maxIndex = min(size-i, (int)(pbuffer->getRemaining()
                        /sizeof(epicsInt16)))+i;
                for(; i<maxIndex; i++)
                    value[i] = pbuffer->getShort();
                if(i<size)
                    pcontrol->ensureData(sizeof(epicsInt16)); // TODO: is there a better way to ensureData?
                else
                    break;
            }
            // set new length
            setLength(size);
            postPut();
        }
        // TODO null arrays (size == -1) not supported
    }

    void BasePVShortArray::serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher, int offset, int count) {
        // cache
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
        int i = offset;
        while(true) {
            int maxIndex = min(end-i, (int)(pbuffer->getRemaining()
                    /sizeof(epicsInt16)))+i;
            for(; i<maxIndex; i++)
                pbuffer->putShort(value[i]);
            if(i<end)
                pflusher->flushSerializeBuffer();
            else
                break;
        }
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

    bool BasePVShortArray::operator==(PVField& pv)
    {
        return getConvert()->equals(this, &pv);
    }

    bool BasePVShortArray::operator!=(PVField& pv)
    {
        return !(getConvert()->equals(this, &pv));
    }
}}
#endif  /* BASEPVSHORTARRAY_H */
