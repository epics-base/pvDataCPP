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

namespace epics { namespace pvData {

    PVDouble::~PVDouble() {}

    class BasePVDouble : public PVDouble {
    public:
        BasePVDouble(PVStructure *parent,ScalarConstPtr scalar);
        virtual ~BasePVDouble();
        virtual double get()const;
        virtual void put(double val);
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher) const;
        virtual void deserialize(ByteBuffer *pbuffer,
            DeserializableControl *pflusher);
        virtual void toString(StringPtr buf)const;
        virtual void toString(StringPtr buf,int indentLevel)const;
    private:
        BasePVDouble(); // not implemented
        BasePVDouble(BasePVDouble const & ); // not implemented
        BasePVDouble & operator=(BasePVDouble const &); //not implemented
        double value;
    };


    BasePVDouble::BasePVDouble(PVStructure *parent,ScalarConstPtr scalar)
    : PVDouble(parent,scalar),value(0.0)
    {}

    BasePVDouble::~BasePVDouble() {}

    double BasePVDouble::get()const { return value;}

    void BasePVDouble::put(double val){value = val;}

    void BasePVDouble::serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher) const
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVDouble::deserialize(ByteBuffer *pbuffer,
        DeserializableControl *pflusher)
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVDouble::toString(StringPtr buf)const {toString(buf,0);}

    void BasePVDouble::toString(StringPtr buf,int indentLevel) const
    {
        convert->getString(buf,this,indentLevel);
        PVField::toString(buf,indentLevel);
    }

}}
#endif  /* BASEPVDOUBLE_H */
