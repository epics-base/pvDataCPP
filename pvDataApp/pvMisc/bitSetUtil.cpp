/*bitSetUtil.cpp*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include "noDefaultMethods.h"
#include "pvData.h"
#include "bitSetUtil.h"

namespace epics { namespace pvData {

static bool checkBitSetPVField(
    PVField *pvField,BitSet *bitSet,int initialOffset)
{
    bool atLeastOneBitSet = false;
    bool allBitsSet = true;
    int offset = initialOffset;
    int nbits = pvField->getNumberFields();
    if(nbits==1) return bitSet->get(offset);
    int nextSetBit = bitSet->nextSetBit(offset);
    if(nextSetBit>=(offset+nbits)) return false;
    if(bitSet->get(offset)) {
        if(nbits>1) {
            for(int i=offset+1; i<offset+nbits; i++) bitSet->clear(i);
        }
        return true;
    }
    PVStructure *pvStructure = static_cast<PVStructure *>(pvField);
    while(offset<initialOffset + nbits) {
        PVField *pvSubField = pvStructure->getSubField(offset);
        int nbitsNow = pvSubField->getNumberFields();
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
            for(int i=0; i<num; i++) {
                PVField *pvSubSubField = pvSubStructureFields[i];
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
            for(int i=initialOffset+1; i<initialOffset+nbits; i++){
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
