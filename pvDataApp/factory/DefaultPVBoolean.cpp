/*PVBoolean.cpp*/
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

    class BasePVBoolean : public PVBoolean {
    public:
        BasePVBoolean(PVStructure *parent,ScalarConstPtr scalar);
        virtual ~BasePVBoolean();
        virtual bool get();
        virtual void put(bool val);
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher);
        virtual void deserialize(ByteBuffer *pbuffer,
            DeserializableControl *pflusher);
        virtual bool operator==(PVField& pv) ;
        virtual bool operator!=(PVField& pv) ;
    private:
        bool value;
    };

    BasePVBoolean::BasePVBoolean(PVStructure *parent,ScalarConstPtr scalar)
    : PVBoolean(parent,scalar),value(false)
    {}

    BasePVBoolean::~BasePVBoolean() {}

    bool BasePVBoolean::get() { return value;}

    void BasePVBoolean::put(bool val){value = val;}

    void BasePVBoolean::serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher) {
        pflusher->ensureBuffer(1);
        pbuffer->putBoolean(value);
    }

    void BasePVBoolean::deserialize(ByteBuffer *pbuffer,
        DeserializableControl *pflusher)
    {
        pflusher->ensureData(1);
        value = pbuffer->getBoolean();
    }

    bool BasePVBoolean::operator==(PVField& pvField)
    {
        return getConvert()->equals(this, &pvField);
    }

    bool BasePVBoolean::operator!=(PVField& pvField)
    {
        return !(getConvert()->equals(this, &pvField));
    }

}}
