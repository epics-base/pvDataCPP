/*BasePVLong.h*/
#ifndef BASEPVLONG_H
#define BASEPVLONG_H
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "pvData.h"
#include "convert.h"
#include "factory.h"
#include "AbstractPVField.h"
#include "byteBuffer.h"

namespace epics { namespace pvData {

    PVLong::~PVLong() {}

    class BasePVLong : public PVLong {
    public:
        BasePVLong(PVStructure *parent,ScalarConstPtr scalar);
        virtual ~BasePVLong();
        virtual int64 get();
        virtual void put(int64 val);
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher) ;
        virtual void deserialize(ByteBuffer *pbuffer,
            DeserializableControl *pflusher);
        virtual bool operator==(PVField& pv) ;
        virtual bool operator!=(PVField& pv) ;
    private:
        int64 value;
    };

    BasePVLong::BasePVLong(PVStructure *parent,ScalarConstPtr scalar)
    : PVLong(parent,scalar),value(0)
    {}

    BasePVLong::~BasePVLong() {}

    int64 BasePVLong::get() { return value;}

    void BasePVLong::put(int64 val){value = val;}

    void BasePVLong::serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher) {
        pflusher->ensureBuffer(sizeof(int64));
        pbuffer->putLong(value);
    }

    void BasePVLong::deserialize(ByteBuffer *pbuffer,
        DeserializableControl *pflusher) {
        pflusher->ensureData(sizeof(int64));
        value = pbuffer->getLong();
    }

    bool BasePVLong::operator==(PVField& pvField)
    {
        return getConvert()->equals(this, &pvField);
    }

    bool BasePVLong::operator!=(PVField& pvField)
    {
        return !(getConvert()->equals(this, &pvField));
    }

}}
#endif  /* BASEPVLONG_H */
