/*PVShort.cpp*/
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

    class BasePVShort : public PVShort {
    public:
        BasePVShort(PVStructure *parent,ScalarConstPtr scalar);
        virtual ~BasePVShort();
        virtual int16 get();
        virtual void put(int16 val);
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher) ;
        virtual void deserialize(ByteBuffer *pbuffer,
            DeserializableControl *pflusher);
        virtual bool operator==(PVField& pv) ;
        virtual bool operator!=(PVField& pv) ;
    private:
        int16 value;
    };

    BasePVShort::BasePVShort(PVStructure *parent,ScalarConstPtr scalar)
    : PVShort(parent,scalar),value(0)
    {}

    BasePVShort::~BasePVShort() {}

    int16 BasePVShort::get() { return value;}

    void BasePVShort::put(int16 val){value = val;}

    void BasePVShort::serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher) {
        pflusher->ensureBuffer(sizeof(int16));
        pbuffer->putShort(value);
    }

    void BasePVShort::deserialize(ByteBuffer *pbuffer,
        DeserializableControl *pflusher) {
        pflusher->ensureData(sizeof(int16));
        value = pbuffer->getShort();
    }

    bool BasePVShort::operator==(PVField& pvField)
    {
        return getConvert()->equals(this, &pvField);
    }

    bool BasePVShort::operator!=(PVField& pvField)
    {
        return !(getConvert()->equals(this, &pvField));
    }

}}
