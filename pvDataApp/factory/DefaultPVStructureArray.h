/*DefaultPVStructureArray.h*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef DEFAULTPVSTRUCTUREARRAY_H
#define DEFAULTPVSTRUCTUREARRAY_H
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "pvData.h"
#include "convert.h"
#include "factory.h"
#include "serializeHelper.h"

namespace epics { namespace pvData {

    class BasePVStructureArray : public PVStructureArray {
    public:
        BasePVStructureArray(PVStructure *parent,
             StructureArrayConstPtr structureArray);
        virtual ~BasePVStructureArray();
        virtual StructureArrayConstPtr getStructureArray();
        virtual int append(int number);
        virtual bool remove(int offset,int number);
        virtual void compress();
        virtual void setCapacity(int capacity);
        virtual int get(int offset, int length,
            StructureArrayData *data);
        virtual int put(int offset,int length,
            PVStructurePtrArray from, int fromOffset);
        virtual bool operator==(PVField &pv);
        virtual bool operator!=(PVField &pv);
        virtual void shareData( PVStructurePtrArray value,int capacity,int length);
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher) const;
        virtual void deserialize(ByteBuffer *buffer,
            DeserializableControl *pflusher);
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher, int offset, int count) const;
    private:
        StructureArrayConstPtr structureArray;
        StructureArrayData *structureArrayData;
        PVStructurePtrArray value;
    };
}}

#endif /*DEFAULTPVSTRUCTUREARRAY_H*/
