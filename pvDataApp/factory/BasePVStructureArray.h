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
        PVStructureArrayPtr pvStructureArray;
    };
    PVStructureArrayPvt::PVStructureArrayPvt(StructureArrayConstPtr structureArray)
    : structureArray(structureArray),
      structureArrayData(new StructureArrayData()),
      pvStructureArray(new PVStructurePtr[0])
    {}

    PVStructureArrayPvt::~PVStructureArrayPvt()
    {
        delete structureArrayData;
        delete pvStructureArray;
    }
    

    PVStructureArray::PVStructureArray(PVStructure *parent,StructureArrayConstPtr structureArray)
    : PVArray(parent,structureArray),pImpl(new PVStructureArrayPvt(structureArray))
    {
    }

    PVStructureArray::~PVStructureArray()
    {
        delete pImpl;
    }

    StructureArrayConstPtr PVStructureArray::getStructureArray() const
    {
        return pImpl->structureArray;
    }

    int PVStructureArray::get(int offset, int length, StructureArrayData *data) const
    {
        return 0;
    }

    int PVStructureArray::put(int offset,int length, PVStructureArrayPtr  from, int fromOffset)
    {
        return 0;
    }

    void PVStructureArray::shareData(PVStructureArrayPtr from)
    {
    }

    void PVStructureArray::serialize(ByteBuffer *pbuffer, SerializableControl *pflusher) const
    {
    }

    void PVStructureArray::deserialize(ByteBuffer *pbuffer, DeserializableControl *pflusher)
    {
    }

    void PVStructureArray::serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher, int offset, int count) const
    {
    }

    void PVStructureArray::toString(StringPtr buf) const {toString(buf,0);}

    void PVStructureArray::toString(StringPtr buf,int indentLevel) const
    {
    }

    class BasePVStructureArray : public PVStructureArray {
    public:
        BasePVStructureArray(PVStructure *parent,StructureArrayConstPtr structureArray)
        : PVStructureArray(parent,structureArray) {}
        ~BasePVStructureArray(){}
    private:
        BasePVStructureArray(); // not implemented
        BasePVStructureArray(BasePVStructureArray const & ); // not implemented
        BasePVStructureArray & operator=(BasePVStructureArray const &); //not implemented
    };

}}
#endif  /* BASEPVSTRUCTUREARRAY_H */
