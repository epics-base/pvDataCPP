/*BasePVBoolean.h*/
#ifndef BASEPVBOOLEAN_H
#define BASEPVBOOLEAN_H
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "pvData.h"
#include "convert.h"
#include "factory.h"
#include "AbstractPVField.h"

namespace epics { namespace pvData {

    PVBoolean::~PVBoolean() {}

    class BasePVBoolean : public PVBoolean {
    public:
        BasePVBoolean(PVStructure *parent,ScalarConstPtr scalar);
        virtual ~BasePVBoolean();
        virtual bool get();
        virtual void put(bool val);
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher) ;
        virtual void deserialize(ByteBuffer *pbuffer,
            DeserializableControl *pflusher);
        virtual void toString(StringBuilder buf);
        virtual void toString(StringBuilder buf,int indentLevel);
        virtual bool operator==(PVField  *pv) ;
    private:
        bool value;
    };

    BasePVBoolean::BasePVBoolean(PVStructure *parent,ScalarConstPtr scalar)
    : PVBoolean(parent,scalar),value(epicsFalse)
    {}

    BasePVBoolean::~BasePVBoolean() {}

    bool BasePVBoolean::get() { return value;}

    void BasePVBoolean::put(bool val){value = val;}

    void BasePVBoolean::serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher) 
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVBoolean::deserialize(ByteBuffer *pbuffer,
        DeserializableControl *pflusher)
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVBoolean::toString(StringBuilder buf) {toString(buf,0);}

    void BasePVBoolean::toString(StringBuilder buf,int indentLevel) 
    {
        getConvert()->getString(buf,this,indentLevel);
        PVField::toString(buf,indentLevel);
    }

    bool BasePVBoolean::operator==(PVField  *pvField) 
    {
        return getConvert()->equals(this,pvField);
    }

}}
#endif  /* BASEPVBOOLEAN_H */
