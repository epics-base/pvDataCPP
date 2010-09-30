/*BasePVDoubleArray.h*/
#ifndef BASEPVDOUBLEARRAY_H
#define BASEPVDOUBLEARRAY_H
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <epicsTypes.h>
#include "pvData.h"
#include "factory.h"
#include "AbstractPVScalarArray.h"

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
        virtual void shareData(DoubleArrayData *from);
        // from Serializable
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) ;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher);
        virtual void serialize(ByteBuffer *pbuffer,
             SerializableControl *pflusher, int offset, int count) ;
        virtual void toString(StringBuilder buf);
        virtual void toString(StringBuilder buf,int indentLevel);
        virtual epicsBoolean equals(PVField  *pv) ;
    private:
        double *doubleArray;
    };

    BasePVDoubleArray::BasePVDoubleArray(PVStructure *parent,
        ScalarArrayConstPtr scalarArray)
    : PVDoubleArray(parent,scalarArray),doubleArray(new double[0])
    { } 

    BasePVDoubleArray::~BasePVDoubleArray()
    {
        delete[] doubleArray;
    }

    void BasePVDoubleArray::setCapacity(int capacity)
    {
        throw std::logic_error(notImplemented);
    }

    int BasePVDoubleArray::get(int offset, int length,
        DoubleArrayData *data) 
    {
        data->data = doubleArray;
        return getLength();
    }

    int BasePVDoubleArray::put(int offset,int length,
        DoubleArray from,int fromOffset)
    {
        return getLength();
    }

    void BasePVDoubleArray::shareData(DoubleArrayData *from)
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVDoubleArray::serialize(ByteBuffer *pbuffer,
         SerializableControl *pflusher) 
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVDoubleArray::deserialize(ByteBuffer *pbuffer,
         DeserializableControl *pflusher)
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVDoubleArray::serialize(ByteBuffer *pbuffer,
         SerializableControl *pflusher, int offset, int count) 
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVDoubleArray::toString(StringBuilder buf)
    {
        toString(buf,1);
    }

    void BasePVDoubleArray::toString(StringBuilder buf,int indentLevel)
    {
        getConvert()->getString(buf,this,indentLevel);
        PVArray::toString(buf,indentLevel);
    }

    epicsBoolean BasePVDoubleArray::equals(PVField  *pv) 
    {
        throw std::logic_error(notImplemented);
    }
}}
#endif  /* BASEPVDOUBLEARRAY_H */
