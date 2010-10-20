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

namespace epics { namespace pvData {

    PVLong::~PVLong() {}

    class BasePVLong : public PVLong {
    public:
        BasePVLong(PVStructure *parent,ScalarConstPtr scalar);
        virtual ~BasePVLong();
        virtual epicsInt64 get();
        virtual void put(epicsInt64 val);
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher) ;
        virtual void deserialize(ByteBuffer *pbuffer,
            DeserializableControl *pflusher);
        virtual void toString(StringBuilder buf);
        virtual void toString(StringBuilder buf,int indentLevel);
        virtual bool operator==(PVField  *pv) ;
        virtual bool operator!=(PVField  *pv) ;
    private:
        epicsInt64 value;
    };

    BasePVLong::BasePVLong(PVStructure *parent,ScalarConstPtr scalar)
    : PVLong(parent,scalar),value(0.0)
    {}

    BasePVLong::~BasePVLong() {}

    epicsInt64 BasePVLong::get() { return value;}

    void BasePVLong::put(epicsInt64 val){value = val;}

    void BasePVLong::serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher) 
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVLong::deserialize(ByteBuffer *pbuffer,
        DeserializableControl *pflusher)
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVLong::toString(StringBuilder buf) {toString(buf,0);}

    void BasePVLong::toString(StringBuilder buf,int indentLevel) 
    {
        getConvert()->getString(buf,this,indentLevel);
        PVField::toString(buf,indentLevel);
    }

    bool BasePVLong::operator==(PVField  *pvField) 
    {
        return getConvert()->equals(this,pvField);
    }

    bool BasePVLong::operator!=(PVField  *pvField) 
    {
        return !(getConvert()->equals(this,pvField));
    }

}}
#endif  /* BASEPVLONG_H */
