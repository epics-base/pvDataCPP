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

#include <shareLib.h>

namespace epics { namespace pvData {

/**
 * @brief Compress a bitSet.
 *
 */
class epicsShareClass BitSetUtil : private NoDefaultMethods {
public:
    /**
     *  compress the bitSet for a pvStructure.
     *  In all subfields of a structure have been modified then
     *  the bit for the structure is set and all the subfield bits
     *  are cleared.
     *  @param bitSet this must be a valid bitSet for pvStructure.
     *  @param pvStructure the structure.
     */
    static bool compress(BitSetPtr const &bitSet,PVStructurePtr const &pvStructure);
};

}}
#endif /*BITSETUTIL_H */
