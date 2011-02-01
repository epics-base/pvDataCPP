/*PVInt.cpp*/
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

    class BasePVInt : public PVInt {
    public:
        BasePVInt(PVStructure *parent,ScalarConstPtr scalar);
        virtual ~BasePVInt();
        virtual int32 get();
        virtual void put(int32 val);
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher) ;
        virtual void deserialize(ByteBuffer *pbuffer,
            DeserializableControl *pflusher);
        virtual bool operator==(PVField& pv) ;
        virtual bool operator!=(PVField& pv) ;
    private:
        int32 value;
    };

    BasePVInt::BasePVInt(PVStructure *parent,ScalarConstPtr scalar)
    : PVInt(parent,scalar),value(0)
    {}

    BasePVInt::~BasePVInt() {}

    int32 BasePVInt::get() { return value;}

    void BasePVInt::put(int32 val){value = val;}

    void BasePVInt::serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher) {
        pflusher->ensureBuffer(sizeof(int32));
        pbuffer->putInt(value);
    }

    void BasePVInt::deserialize(ByteBuffer *pbuffer,
        DeserializableControl *pflusher) {
        pflusher->ensureData(sizeof(int32));
        value = pbuffer->getInt();
    }

    bool BasePVInt::operator==(PVField& pvField)
    {
        return getConvert()->equals(this, &pvField);
    }

    bool BasePVInt::operator!=(PVField& pvField)
    {
        return !(getConvert()->equals(this, &pvField));
    }

}}
