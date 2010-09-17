/*AbstractPVScalar.h*/
#ifndef ABSTRACTPVSCALAR_H
#define ABSTRACTPVSCALAR_H
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "pvData.h"
#include "factory.h"
#include "AbstractPVField.h"

namespace epics { namespace pvData {

    PVScalar::~PVScalar() {}

    class AbstractPVScalar : public AbstractPVField, public  PVScalar {
    public:
        AbstractPVScalar(PVStructure *parent,ScalarConstPtr scalar)
        : AbstractPVField(parent,scalar) {}
        virtual ~AbstractPVScalar() {}
        virtual Scalar *getScalar()const {
            return (Scalar*)AbstractPVField::getField();
        }
    };


}}
#endif  /* ABSTRACTPVSCALAR_H */
