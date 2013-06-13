/*bitSetUtil.h*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mes
 */
#ifndef BITSETUTIL_H
#define BITSETUTIL_H
#include <pv/noDefaultMethods.h>
#include <pv/pvData.h>
#include <pv/bitSet.h>

namespace epics { namespace pvData {

class BitSetUtil : private NoDefaultMethods {
public:
    static bool compress(BitSetPtr const &bitSet,PVStructurePtr const &pvStructure);
};

}}
#endif /*BITSETUTIL_H */
