/*BasePVDouble.h*/
#ifndef BASEPVDOUBLE_H
#define BASEPVDOUBLE_H
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "pvData.h"
#include "factory.h"
#include "AbstractPVField.h"

namespace epics { namespace pvData {

    PVDouble::~PVDouble() {}

    class BasePVDouble : public PVDouble {
    public:
        BasePVDouble(PVStructure *parent,ScalarConstPtr scalar)
        : PVDouble(parent,scalar),value(0.0) {}
        virtual ~BasePVDouble() {}
        virtual double get()const { return value;}
        virtual void put(double val){value = val;}
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) const{}
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher){}
        virtual void toString(StringPtr buf)const {}
        virtual void toString(StringPtr buf,int indentLevel)const {}
    private:
        BasePVDouble(); // not implemented
        double value;
    };


}}
#endif  /* BASEPVDOUBLE_H */
