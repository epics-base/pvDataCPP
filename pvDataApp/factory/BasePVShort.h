/*BasePVShort.h*/
#ifndef BASEPVSHORT_H
#define BASEPVSHORT_H
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

    PVShort::~PVShort() {}

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
        virtual void toString(StringBuilder buf);
        virtual void toString(StringBuilder buf,int indentLevel);
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

    void BasePVShort::toString(StringBuilder buf) {toString(buf,0);}

    void BasePVShort::toString(StringBuilder buf,int indentLevel)
    {
        getConvert()->getString(buf,this,indentLevel);
        PVField::toString(buf,indentLevel);
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
#endif  /* BASEPVSHORT_H */
