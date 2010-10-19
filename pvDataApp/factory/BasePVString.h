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
        virtual void toString(StringBuilder buf);
        virtual void toString(StringBuilder buf,int indentLevel);
        virtual bool operator==(PVField  *pv) ;
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
        SerializableControl *pflusher) 
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVString::deserialize(ByteBuffer *pbuffer,
        DeserializableControl *pflusher)
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVString::toString(StringBuilder buf) {toString(buf,0);}

    void BasePVString::toString(StringBuilder buf,int indentLevel) 
    {
        getConvert()->getString(buf,this,indentLevel);
        PVField::toString(buf,indentLevel);
    }

    bool BasePVString::operator==(PVField  *pvField) 
    {
        return getConvert()->equals(this,pvField);
    }

}}
#endif  /* BASEPVSTRING_H */
