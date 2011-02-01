/*PVScalar.cpp*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "pvData.h"
#include "factory.h"

namespace epics { namespace pvData {

    PVScalar::~PVScalar() {}

    PVScalar::PVScalar(PVStructure *parent,ScalarConstPtr scalar)
    : PVField(parent,scalar) {}

    ScalarConstPtr PVScalar::getScalar() 
    {
       return (ScalarConstPtr) PVField::getField();
    }

}}
