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

using std::tr1::static_pointer_cast;

namespace epics { namespace pvData {

    PVScalar::~PVScalar() {}

    PVScalar::PVScalar(PVStructure *parent,ScalarConstPtr scalar)
    : PVField(parent,scalar) {}

    ScalarConstPtr PVScalar::getScalar() 
    {
       return static_pointer_cast<const Scalar>(PVField::getField());
    }

}}
