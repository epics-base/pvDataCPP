/*AbstractPVScalarArray.h*/
#ifndef ABSTRACTPVSCALARARRAY_H
#define ABSTRACTPVSCALARARRAY_H
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "pvData.h"
#include "factory.h"
#include "AbstractPVArray.h"

namespace epics { namespace pvData {

    PVScalarArray::~PVScalarArray() {}

    PVScalarArray::PVScalarArray(PVStructure *parent,
        ScalarArrayConstPtr scalarArray)
    : PVArray(parent,scalarArray) {}

    ScalarArrayConstPtr PVScalarArray::getScalarArray() const
    {
       return (ScalarArrayConstPtr) PVField::getField();
    }

}}
#endif  /* ABSTRACTPVSCALARARRAY_H */
