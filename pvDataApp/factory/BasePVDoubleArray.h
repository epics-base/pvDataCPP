/*BasePVDoubleArray.h*/
#ifndef BASEPVDOUBLEARRAY_H
#define BASEPVDOUBLEARRAY_H
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "pvData.h"
#include "factory.h"
#include "AbstractPVScalarArray.h"
#include "serializeHelper.h"

using std::min;

namespace epics { namespace pvData {

    PVDoubleArray::~PVDoubleArray() {}

    PVDoubleArray::PVDoubleArray(PVStructure *parent,ScalarArrayConstPtr scalar)
    : PVScalarArray(parent,scalar) {}

    class BasePVDoubleArray : public PVDoubleArray {
    public:
        BasePVDoubleArray(PVStructure *parent,ScalarArrayConstPtr scalarArray);
        virtual ~BasePVDoubleArray();
        virtual void setCapacity(int capacity);
        virtual int get(int offset, int length, DoubleArrayData *data) ;
        virtual int put(int offset,int length,DoubleArray from,
           int fromOffset);
        virtual void shareData(double value[],int capacity,int length);
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
        double *value;
    };

    BasePVDoubleArray::BasePVDoubleArray(PVStructure *parent,
        ScalarArrayConstPtr scalarArray)
    : PVDoubleArray(parent,scalarArray),value(new double[0])
    { }

    BasePVDoubleArray::~BasePVDoubleArray()
    {
        delete[] value;
    }

    void BasePVDoubleArray::setCapacity(int capacity)
    {
        if(getCapacity()==capacity) return;
        if(!isCapacityMutable()) {
            std::string message("not capacityMutable");
            PVField::message(message, errorMessage);
            return;
        }
        int length = PVArray::getLength();
        if(length>capacity) length = capacity;
        double *newValue = new double[capacity];
        for(int i=0; i<length; i++) newValue[i] = value[i];
        delete[]value;
        value = newValue;
        setCapacityLength(capacity,length);
    }

    int BasePVDoubleArray::get(int offset, int len, DoubleArrayData *data)
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

    int BasePVDoubleArray::put(int offset,int len,
        DoubleArray from,int fromOffset)
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

    void BasePVDoubleArray::shareData(
        double shareValue[],int capacity,int length)
    {
        delete[] value;
        value = shareValue;
        PVArray::setCapacityLength(capacity,length);
    }

    void BasePVDoubleArray::serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher) {
        serialize(pbuffer, pflusher, 0, getLength());
    }

    void BasePVDoubleArray::deserialize(ByteBuffer *pbuffer,
            DeserializableControl *pcontrol) {
        int size = SerializeHelper::readSize(pbuffer, pcontrol);
        if(size>=0) {
            // prepare array, if necessary
            if(size>getCapacity()) setCapacity(size);
            // retrieve value from the buffer
            int i = 0;
            while(true) {
                int maxIndex = min(size-i, (int)(pbuffer->getRemaining()
                        /sizeof(double)))+i;
                for(; i<maxIndex; i++)
                    value[i] = pbuffer->getDouble();
                if(i<size)
                    pcontrol->ensureData(sizeof(double));
                else
                    break;
            }
            // set new length
            setLength(size);
            postPut();
        }
        // TODO null arrays (size == -1) not supported
    }

    void BasePVDoubleArray::serialize(ByteBuffer *pbuffer,
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
                    /sizeof(double)))+i;
            for(; i<maxIndex; i++)
                pbuffer->putDouble(value[i]);
            if(i<end)
                pflusher->flushSerializeBuffer();
            else
                break;
        }
    }

    void BasePVDoubleArray::toString(StringBuilder buf)
    {
        toString(buf,1);
    }

    void BasePVDoubleArray::toString(StringBuilder buf,int indentLevel)
    {
        getConvert()->getString(buf,this,indentLevel);

        PVField::toString(buf,indentLevel);
    }

    bool BasePVDoubleArray::operator==(PVField& pv)
    {
        return getConvert()->equals(this, &pv);
    }

    bool BasePVDoubleArray::operator!=(PVField& pv)
    {
        return !(getConvert()->equals(this, &pv));
    }
}}
#endif  /* BASEPVDOUBLEARRAY_H */
