/*BasePVStructureArray.h*/
#ifndef BASEPVSTRUCTUREARRAY_H
#define BASEPVSTRUCTUREARRAY_H
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "pvData.h"
#include "factory.h"

namespace epics { namespace pvData {

    class PVStructureArrayPvt {
    public:
        PVStructureArrayPvt(StructureArrayConstPtr structureArray);
        ~PVStructureArrayPvt();

        StructureArrayConstPtr structureArray;
        StructureArrayData *structureArrayData;
        PVStructurePtrArray pvStructureArray;
    };
    PVStructureArrayPvt::PVStructureArrayPvt(
        StructureArrayConstPtr structureArray)
    : structureArray(structureArray),
      structureArrayData(new StructureArrayData()),
      pvStructureArray(new PVStructurePtr[0])
    {}

    PVStructureArrayPvt::~PVStructureArrayPvt()
    {
        delete structureArrayData;
        delete pvStructureArray;
    }
    

    PVStructureArray::PVStructureArray(PVStructure *parent,
        StructureArrayConstPtr structureArray)
    : PVArray(parent,structureArray),
        pImpl(new PVStructureArrayPvt(structureArray))
    {
    }

    PVStructureArray::~PVStructureArray()
    {
        delete pImpl;
    }

    StructureArrayConstPtr PVStructureArray::getStructureArray() 
    {
        return pImpl->structureArray;
    }

    int PVStructureArray::get(
        int offset, int length, StructureArrayData *data) 
    {
        throw std::logic_error(notImplemented);
    }

    int PVStructureArray::put(int offset,int length,
        PVStructurePtrArray  from, int fromOffset)
    {
        throw std::logic_error(notImplemented);
    }

    void PVStructureArray::shareData(
        PVStructurePtrArray value,int capacity,int length)
    {
        throw std::logic_error(notImplemented);
    }

    void PVStructureArray::serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher) 
    {
        throw std::logic_error(notImplemented);
    }

    void PVStructureArray::deserialize(ByteBuffer *pbuffer,
        DeserializableControl *pflusher)
    {
        throw std::logic_error(notImplemented);
    }

    void PVStructureArray::serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher, int offset, int count) 
    {
        throw std::logic_error(notImplemented);
    }

    void PVStructureArray::toString(StringBuilder buf)  {toString(buf,0);}

    void PVStructureArray::toString(StringBuilder buf,int indentLevel) 
    {
        throw std::logic_error(notImplemented);
    }

    epicsBoolean PVStructureArray::equals(PVField  *pv) 
    {
        throw std::logic_error(notImplemented);
    }

    class BasePVStructureArray : public PVStructureArray {
    public:
        BasePVStructureArray(PVStructure *parent,
             StructureArrayConstPtr structureArray)
        : PVStructureArray(parent,structureArray) {}
        ~BasePVStructureArray(){}
    private:
    };

}}
#endif  /* BASEPVSTRUCTUREARRAY_H */
