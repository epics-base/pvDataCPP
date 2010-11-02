/*BasePVFloat.h*/
#ifndef BASEPVFLOAT_H
#define BASEPVFLOAT_H
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

    PVFloat::~PVFloat() {}

    class BasePVFloat : public PVFloat {
    public:
        BasePVFloat(PVStructure *parent,ScalarConstPtr scalar);
        virtual ~BasePVFloat();
        virtual float get();
        virtual void put(float val);
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher) ;
        virtual void deserialize(ByteBuffer *pbuffer,
            DeserializableControl *pflusher);
        virtual void toString(StringBuilder buf);
        virtual void toString(StringBuilder buf,int indentLevel);
        virtual bool operator==(PVField& pv) ;
        virtual bool operator!=(PVField& pv) ;
    private:
        float value;
    };

    BasePVFloat::BasePVFloat(PVStructure *parent,ScalarConstPtr scalar)
    : PVFloat(parent,scalar),value(0.0)
    {}

    BasePVFloat::~BasePVFloat() {}

    float BasePVFloat::get() { return value;}

    void BasePVFloat::put(float val){value = val;}

    void BasePVFloat::serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher) {
        pflusher->ensureBuffer(sizeof(float));
        pbuffer->putFloat(value);
    }

    void BasePVFloat::deserialize(ByteBuffer *pbuffer,
        DeserializableControl *pflusher) {
        pflusher->ensureData(sizeof(float));
        value = pbuffer->getFloat();
    }

    void BasePVFloat::toString(StringBuilder buf) {toString(buf,0);}

    void BasePVFloat::toString(StringBuilder buf,int indentLevel)
    {
        getConvert()->getString(buf,this,indentLevel);
        PVField::toString(buf,indentLevel);
    }

    bool BasePVFloat::operator==(PVField& pvField)
    {
        return getConvert()->equals(this, &pvField);
    }

    bool BasePVFloat::operator!=(PVField& pvField)
    {
        return !(getConvert()->equals(this, &pvField));
    }

}}
#endif  /* BASEPVFLOAT_H */
