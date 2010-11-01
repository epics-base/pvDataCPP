/*BasePVDouble.h*/
#ifndef BASEPVDOUBLE_H
#define BASEPVDOUBLE_H
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

    PVDouble::~PVDouble() {}

    class BasePVDouble : public PVDouble {
    public:
        BasePVDouble(PVStructure *parent,ScalarConstPtr scalar);
        virtual ~BasePVDouble();
        virtual double get();
        virtual void put(double val);
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher) ;
        virtual void deserialize(ByteBuffer *pbuffer,
            DeserializableControl *pflusher);
        virtual void toString(StringBuilder buf);
        virtual void toString(StringBuilder buf,int indentLevel);
        virtual bool operator==(PVField& pv) ;
        virtual bool operator!=(PVField& pv) ;
    private:
        double value;
    };

    BasePVDouble::BasePVDouble(PVStructure *parent,ScalarConstPtr scalar)
    : PVDouble(parent,scalar),value(0.0)
    {}

    BasePVDouble::~BasePVDouble() {}

    double BasePVDouble::get() { return value;}

    void BasePVDouble::put(double val){value = val;}

    void BasePVDouble::serialize(ByteBuffer *pbuffer,
                SerializableControl *pflusher) {
        pflusher->ensureBuffer(sizeof(double));
        pbuffer->putDouble(value);
    }

    void BasePVDouble::deserialize(ByteBuffer *pbuffer,
                DeserializableControl *pflusher) {
        pflusher->ensureData(sizeof(double));
        value = pbuffer->getDouble();
    }

    void BasePVDouble::toString(StringBuilder buf) {toString(buf,0);}

    void BasePVDouble::toString(StringBuilder buf,int indentLevel)
    {
        getConvert()->getString(buf,this,indentLevel);
        PVField::toString(buf,indentLevel);
    }

    bool BasePVDouble::operator==(PVField& pvField)
    {
        return getConvert()->equals(this, &pvField);
    }

    bool BasePVDouble::operator!=(PVField& pvField)
    {
        return !(getConvert()->equals(this, &pvField));
    }

}}
#endif  /* BASEPVDOUBLE_H */
