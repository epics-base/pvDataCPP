/*bitSetUtil.cpp*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#include <pv/noDefaultMethods.h>
#include <pv/pvData.h>
#include <pv/bitSetUtil.h>

namespace epics { namespace pvData {

static bool checkBitSetPVField(
    PVField *pvField,BitSet *bitSet,int32 initialOffset)
{
    bool atLeastOneBitSet = false;
    bool allBitsSet = true;
    int32 offset = initialOffset;
    int32 nbits = pvField->getNumberFields();
    if(nbits==1) return bitSet->get(offset);
    int32 nextSetBit = bitSet->nextSetBit(offset);
    if(nextSetBit>=(offset+nbits)) return false;
    if(bitSet->get(offset)) {
        if(nbits>1) {
            for(int32 i=offset+1; i<offset+nbits; i++) bitSet->clear(i);
        }
        return true;
    }
    PVStructure *pvStructure = static_cast<PVStructure *>(pvField);
    while(offset<initialOffset + nbits) {
        PVField *pvSubField = pvStructure->getSubField(offset).get();
        int32 nbitsNow = pvSubField->getNumberFields();
        if(nbitsNow==1) {
            if(bitSet->get(offset)) {
                atLeastOneBitSet = true;
            } else {
                allBitsSet = false;
            }
            offset++;
        } else {
            offset++;
            PVStructure *pvSubStructure = static_cast<PVStructure*>(pvField);
            PVFieldPtrArray pvSubStructureFields =
                pvSubStructure->getPVFields();
            int num = pvSubStructure->getStructure()->getNumberFields();
            for(int32 i=0; i<num; i++) {
                PVField *pvSubSubField = pvSubStructureFields[i].get();
                bool result = checkBitSetPVField(pvSubSubField,bitSet,offset);
                if(result) {
                    atLeastOneBitSet = true;
                    if(!bitSet->get(offset)) {
                        allBitsSet = false;
                    }
                } else {
                    allBitsSet = false;
                }
                offset += pvSubSubField->getNumberFields();
            }
        }
    }
    if(allBitsSet) {
        if(nbits>1) {
            for(int32 i=initialOffset+1; i<initialOffset+nbits; i++){
                bitSet->clear(i);
            }
        }
        bitSet->set(initialOffset);
    }
    return atLeastOneBitSet;
}

bool BitSetUtil::compress(BitSet *bitSet,PVStructure *pvStructure)
{
    return checkBitSetPVField(pvStructure,bitSet,0);   
}

}}
