/*AbstractPVScalarArray.h*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
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

    ScalarArrayConstPtr PVScalarArray::getScalarArray() 
    {
       return (ScalarArrayConstPtr) PVField::getField();
    }

}}
#endif  /* ABSTRACTPVSCALARARRAY_H */
