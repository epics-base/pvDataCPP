
#include <epicsTypes.h>
#include <epicsVersion.h>
#include <epicsConvert.h>
#include <epicsAssert.h>
#include <epicsMutex.h>
#include <epicsGuard.h>

#define epicsExportSharedSymbols
#include <pv/createRequest.h>
#include <pv/bitSet.h>

namespace epics{namespace pvData {

static
void setStruct(BitSet& ret, const PVStructure& S)
{
    for(size_t i=S.getFieldOffset(), L=S.getNextFieldOffset(); i<L; i++) {
        ret.set(i);
    }
}

static
void _buildMask(BitSet& ret, const PVStructure* type, const Structure* pvRequestMask, bool expand, unsigned depth)
{
    const StringArray& reqNames = pvRequestMask->getFieldNames();

    if(reqNames.empty()) {
        // empty sub-structure selects all members
        ret.set(type->getFieldOffset());
        if(expand) setStruct(ret, *type);

    } else {
        // iterate through request fields
        for(size_t i=0, N=reqNames.size(); i<N; i++)
        {
            // does the requested field actually exist in the target Structure?
            PVField::const_shared_pointer subtype(type->getSubField(reqNames[i]));
            const FieldConstPtr& subReq = pvRequestMask->getFields()[i];

            if(!subtype || subReq->getType()!=structure)
                continue; // TODO: warn/error on invalid selection?

            if(subtype->getField()->getType()==structure && depth<5) {
                // requested field is a Structure, recurse if below arbitrary limit
                _buildMask(ret,
                           static_cast<const PVStructure*>(subtype.get()),
                           static_cast<const Structure*>(subReq.get()),
                           expand,
                           depth+1);

            } else {
                // requested field so not a structure, or at recursion limit, so just select it and move on
                ret.set(subtype->getFieldOffset());
                if(expand && subtype->getField()->getType()==structure) setStruct(ret, static_cast<const PVStructure&>(*subtype));
                // TODO: error if subReq has sub-structure?
            }

        }
    }
}

BitSet extractRequestMask(const PVStructure::const_shared_pointer& type,
                          const PVStructure::const_shared_pointer& pvRequestMask,
                          bool expand)
{
    BitSet ret;

    if(!type)
        throw std::invalid_argument("NULL type not allowed");

    if(!pvRequestMask) {
        // we treat no sub-struct as wildcard.  (totally empty selection is useless)
        ret.set(0);
        if(expand) setStruct(ret, *type);

    } else {
        _buildMask(ret, type.get(), pvRequestMask->getStructure().get(), expand, 0);
    }

    return ret;
}


}} //namespace epics::pvData
