/*BasePVInt.h*/
#ifndef BASEPVINT_H
#define BASEPVINT_H
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "pvData.h"
#include "convert.h"
#include "factory.h"
#include "AbstractPVField.h"

namespace epics { namespace pvData {

    PVInt::~PVInt() {}

    class BasePVInt : public PVInt {
    public:
        BasePVInt(PVStructure *parent,ScalarConstPtr scalar);
        virtual ~BasePVInt();
        virtual epicsInt32 get();
        virtual void put(epicsInt32 val);
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher) ;
        virtual void deserialize(ByteBuffer *pbuffer,
            DeserializableControl *pflusher);
        virtual void toString(StringBuilder buf);
        virtual void toString(StringBuilder buf,int indentLevel);
        virtual bool operator==(PVField  *pv) ;
        virtual bool operator!=(PVField  *pv) ;
    private:
        epicsInt32 value;
    };

    BasePVInt::BasePVInt(PVStructure *parent,ScalarConstPtr scalar)
    : PVInt(parent,scalar),value(0.0)
    {}

    BasePVInt::~BasePVInt() {}

    epicsInt32 BasePVInt::get() { return value;}

    void BasePVInt::put(epicsInt32 val){value = val;}

    void BasePVInt::serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher) 
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVInt::deserialize(ByteBuffer *pbuffer,
        DeserializableControl *pflusher)
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVInt::toString(StringBuilder buf) {toString(buf,0);}

    void BasePVInt::toString(StringBuilder buf,int indentLevel) 
    {
        getConvert()->getString(buf,this,indentLevel);
        PVField::toString(buf,indentLevel);
    }

    bool BasePVInt::operator==(PVField  *pvField) 
    {
        return getConvert()->equals(this,pvField);
    }

    bool BasePVInt::operator!=(PVField  *pvField) 
    {
        return !(getConvert()->equals(this,pvField));
    }

}}
#endif  /* BASEPVINT_H */
