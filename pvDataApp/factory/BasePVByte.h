/*BasePVByte.h*/
#ifndef BASEPVBYTE_H
#define BASEPVBYTE_H
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

    PVByte::~PVByte() {}

    class BasePVByte : public PVByte {
    public:
        BasePVByte(PVStructure *parent,ScalarConstPtr scalar);
        virtual ~BasePVByte();
        virtual epicsInt8 get();
        virtual void put(epicsInt8 val);
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher) ;
        virtual void deserialize(ByteBuffer *pbuffer,
            DeserializableControl *pflusher);
        virtual void toString(StringBuilder buf);
        virtual void toString(StringBuilder buf,int indentLevel);
        virtual bool operator==(PVField& pv) ;
        virtual bool operator!=(PVField& pv) ;
    private:
        epicsInt8 value;
    };

    BasePVByte::BasePVByte(PVStructure *parent,ScalarConstPtr scalar)
    : PVByte(parent,scalar),value(0)
    {}

    BasePVByte::~BasePVByte() {}

    epicsInt8 BasePVByte::get() { return value;}

    void BasePVByte::put(epicsInt8 val){value = val;}

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

    void BasePVByte::toString(StringBuilder buf) {toString(buf,0);}

    void BasePVByte::toString(StringBuilder buf,int indentLevel)
    {
        getConvert()->getString(buf,this,indentLevel);
        PVField::toString(buf,indentLevel);
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
#endif  /* BASEPVBYTE_H */
