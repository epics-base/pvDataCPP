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

namespace epics { namespace pvData {

    PVDoubleArray::~PVDoubleArray() {}

    PVDoubleArray::PVDoubleArray(PVStructure *parent,ScalarArrayConstPtr scalar)
    : PVScalarArray(parent,scalar) {}

    class BasePVDoubleArray : public PVDoubleArray {
    public:
        BasePVDoubleArray(PVStructure *parent,ScalarArrayConstPtr scalarArray);
        virtual ~BasePVDoubleArray();
        virtual void setCapacity(int capacity);
        virtual int get(int offset, int length, DoubleArrayData *data) const;
        virtual int put(int offset,int length,DoubleArrayPtr from,
           int fromOffset);
        virtual void shareData(DoubleArrayPtr from);
       // from Serializable
       virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) const;
       virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher);
       virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher, int offset, int count) const;
       virtual void toString(StringPtr buf)const;
       virtual void toString(StringPtr buf,int indentLevel)const;
    private:
        BasePVDoubleArray(); // not implemented
        DoubleArrayPtr doubleArray;
    };

    BasePVDoubleArray::BasePVDoubleArray(PVStructure *parent,ScalarArrayConstPtr scalarArray)
    : PVDoubleArray(parent,scalarArray),doubleArray(new double[0])
    {
    }

    BasePVDoubleArray::~BasePVDoubleArray()
    {
        delete[] doubleArray;
    }

    void BasePVDoubleArray::setCapacity(int capacity)
    {
    }

    int BasePVDoubleArray::get(int offset, int length,
        DoubleArrayData *data) const
    {
        data->data = doubleArray;
        return getLength();
    }

    int BasePVDoubleArray::put(int offset,int length,
        DoubleArrayPtr from,int fromOffset)
    {
        return getLength();
    }

    void BasePVDoubleArray::shareData(DoubleArrayPtr from)
    {
    }

    void BasePVDoubleArray::serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) const
    {
    }

    void BasePVDoubleArray::deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher)
    {
    }

    void BasePVDoubleArray::serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher, int offset, int count) const
    {
    }

    void BasePVDoubleArray::toString(StringPtr buf)const
    {
    }

    void BasePVDoubleArray::toString(StringPtr buf,int indentLevel)const
    {
    }


}}
#endif  /* BASEPVDOUBLEARRAY_H */
