/*BasePVString.h*/
#ifndef BASEPVSTRING_H
#define BASEPVSTRING_H
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "pvData.h"
#include "convert.h"
#include "factory.h"
#include "AbstractPVField.h"
#include "byteBuffer.h"
#include "serializeHelper.h"

namespace epics { namespace pvData {

    PVString::~PVString() {}

    class BasePVString : public PVString {
    public:
        BasePVString(PVStructure *parent,ScalarConstPtr scalar);
        virtual ~BasePVString();
        virtual String get();
        virtual void put(String val);
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher) ;
        virtual void deserialize(ByteBuffer *pbuffer,
            DeserializableControl *pflusher);
        virtual bool operator==(PVField& pv) ;
        virtual bool operator!=(PVField& pv) ;
    private:
        String value;
    };

    BasePVString::BasePVString(PVStructure *parent,ScalarConstPtr scalar)
    : PVString(parent,scalar),value("")
    {}

    BasePVString::~BasePVString() {}

    String BasePVString::get() { return value;}

    void BasePVString::put(String val){value = val;}

    void BasePVString::serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher) {
        SerializeHelper::serializeString(value, pbuffer, pflusher);
    }

    void BasePVString::deserialize(ByteBuffer *pbuffer,
        DeserializableControl *pflusher) {
        value = SerializeHelper::deserializeString(pbuffer, pflusher);
    }

    bool BasePVString::operator==(PVField& pvField)
    {
        return getConvert()->equals(this, &pvField);
    }

    bool BasePVString::operator!=(PVField& pvField)
    {
        return !(getConvert()->equals(this, &pvField));
    }

}}
#endif  /* BASEPVSTRING_H */
