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

    PVScalar::PVScalar(PVStructure *parent,ScalarConstPtr scalar)
    : PVField(parent,scalar) {}

    ScalarConstPtr PVScalar::getScalar() const
    {
       return (ScalarConstPtr) PVField::getField();
    }

}}
#endif  /* ABSTRACTPVSCALAR_H */
