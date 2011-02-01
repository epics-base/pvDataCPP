/*PVByte.cpp*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "pvData.h"
#include "convert.h"
#include "factory.h"
#include "byteBuffer.h"

namespace epics { namespace pvData {

    class BasePVByte : public PVByte {
    public:
        BasePVByte(PVStructure *parent,ScalarConstPtr scalar);
        virtual ~BasePVByte();
        virtual int8 get();
        virtual void put(int8 val);
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher) ;
        virtual void deserialize(ByteBuffer *pbuffer,
            DeserializableControl *pflusher);
        virtual bool operator==(PVField& pv) ;
        virtual bool operator!=(PVField& pv) ;
    private:
        int8 value;
    };

    BasePVByte::BasePVByte(PVStructure *parent,ScalarConstPtr scalar)
    : PVByte(parent,scalar),value(0)
    {}

    BasePVByte::~BasePVByte() {}

    int8 BasePVByte::get() { return value;}

    void BasePVByte::put(int8 val){value = val;}

    void BasePVByte::serialize(ByteBuffer *pbuffer,
                SerializableControl *pflusher) {
        pflusher->ensureBuffer(1);
        pbuffer->putByte(value);
    }

    void BasePVByte::deserialize(ByteBuffer *pbuffer,
                DeserializableControl *pflusher) {
        pflusher->ensureData(1);
        value = pbuffer->getByte();
    }

    bool BasePVByte::operator==(PVField& pvField)
    {
        return getConvert()->equals(this, &pvField);
    }

    bool BasePVByte::operator!=(PVField& pvField)
    {
        return !(getConvert()->equals(this, &pvField));
    }

}}
