/*PVScalar.cpp*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>

#define epicsExportSharedSymbols
#include <pv/pvData.h>
#include <pv/factory.h>

using std::tr1::static_pointer_cast;

namespace epics { namespace pvData {

    PVScalar::~PVScalar() {}

    PVScalar::PVScalar(ScalarConstPtr const & scalar)
    : PVField(scalar) {}

    const ScalarConstPtr PVScalar::getScalar() const
    {
       return static_pointer_cast<const Scalar>(PVField::getField());
    }

    template<>
    std::ostream& PVScalarValue<int8>::dumpValue(std::ostream& o) const
    {
        return o << static_cast<int>(get());
    }

    template<>
    std::ostream& PVScalarValue<uint8>::dumpValue(std::ostream& o) const
    {
    	return o << static_cast<unsigned int>(get());
    }

    template<>
    std::ostream& PVScalarValue<boolean>::dumpValue(std::ostream& o) const
    {
        return o << std::boolalpha << static_cast<bool>(get());
    }
}}
