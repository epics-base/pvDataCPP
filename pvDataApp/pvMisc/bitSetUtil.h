/*bitSetUtil.h*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef BITSETUTIL_H
#define BITSETUTIL_H
#include "noDefaultMethods.h"
#include "pvData.h"
#include "bitSet.h"

namespace epics { namespace pvData {

class BitSetUtil : private NoDefaultMethods {
public:
    static bool compress(BitSet *bitSet,PVStructure *pvStructure);
};

}}
#endif /*BITSETUTIL_H */
