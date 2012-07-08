/*PVStructureArray.cpp*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <pv/pvData.h>
#include <pv/convert.h>
#include <pv/factory.h>
#include <pv/serializeHelper.h>

using std::tr1::static_pointer_cast;
using std::size_t;

namespace epics { namespace pvData {

PVStructureArray::PVStructureArray(StructureArrayConstPtr const & structureArray)
: PVArray(structureArray),
  structureArray(structureArray),
  value(std::tr1::shared_ptr<std::vector<PVStructurePtr> >
      (new std::vector<PVStructurePtr>()))
{
}

size_t PVStructureArray::append(size_t number)
{
    size_t currentLength = getCapacity();
    size_t newLength = currentLength + number;
    setCapacity(newLength);
    StructureConstPtr structure = structureArray->getStructure();
    PVStructurePtrArray *to = value.get();
    for(size_t i=currentLength; i<newLength; i++) {
        PVStructurePtr pvStructure(getPVDataCreate()->createPVStructure(structure));
        (*to)[i].swap(pvStructure);
    }
    return newLength;
}

bool PVStructureArray::remove(size_t offset,size_t number)
{
    size_t length = getLength();
    if(offset+number>length) return false;
    PVStructurePtrArray vec = *value.get();
    for(size_t i = offset; i+number < length; i++) {
         vec[i] =  vec[i + number];
    }
    size_t newLength = length - number;
    setCapacityLength(newLength,newLength);
    return true;
}

void PVStructureArray::compress() {
    size_t length = getCapacity();
    size_t newLength = 0;
    PVStructurePtrArray vec = *value.get();
    for(size_t i=0; i<length; i++) {
        if(vec[i].get()!=NULL) {
            newLength++;
            continue;
        }
        // find first non 0
        size_t notNull = 0;
        for(size_t j=i+1;j<length;j++) {
            if(vec[j].get()!=NULL) {
                notNull = j;
                break;
            }
        }
        if(notNull!=0) {
            vec[i] = vec[notNull];
            vec[notNull].reset();
            newLength++;
            continue;
         }
         break;
    }
    setCapacityLength(newLength,newLength);
}

void PVStructureArray::setCapacity(size_t capacity) {
    if(getCapacity()==capacity) return;
    if(!isCapacityMutable()) {
        std::string message("not capacityMutable");
        PVField::message(message, errorMessage);
        return;
    }
    size_t length = getCapacity();
    PVStructurePtrArray array(capacity);
    size_t num = PVArray::getLength();
    if(num>capacity) num = capacity;
    PVStructurePtr * from = get();
    for (size_t i=0; i<num; i++) array[i] = from[i];
    value->swap(array);
    setCapacityLength(capacity,length);
}


StructureArrayConstPtr PVStructureArray::getStructureArray() const
{
    return structureArray;
}

size_t PVStructureArray::get(
    size_t offset, size_t len, StructureArrayData &data)
{
    size_t n = len;
    size_t length = getLength();
    if(offset+len > length) {
        n = length - offset;
        if(n<0) n = 0;
    }
    data.data = *value.get();
    data.offset = offset;
    return n;
}

size_t PVStructureArray::put(size_t offset,size_t len,
    const_vector const & from, size_t fromOffset)
{
    if(isImmutable()) {
        message(String("field is immutable"), errorMessage);
        return 0;
    }
    if(&from==value.get()) return 0;
    if(len<1) return 0;
    size_t length = getLength();
    size_t capacity = getCapacity();
    if(offset+len > length) {
        size_t newlength = offset + len;
        if(newlength>capacity) {
            setCapacity(newlength);
            capacity = getCapacity();
            newlength = capacity;
            len = newlength - offset;
            if(len<=0) return 0;
        }
        length = newlength;
        setLength(length);
    }
    PVStructurePtrArray *to = value.get();
    StructureConstPtr structure = structureArray->getStructure();
    for(size_t i=0; i<len; i++) {
    	PVStructurePtr frompv = from[i+fromOffset];
    	if(frompv.get()!=NULL) {
    	    if(frompv->getStructure()!=structure) {
                 throw std::invalid_argument(String(
                   "Element is not a compatible structure"));
    	    }
    	}
    	(*to)[i+offset] = frompv;
    }
    postPut();
    setLength(length);
    return len;
}

void PVStructureArray::shareData(
     std::tr1::shared_ptr<std::vector<PVStructurePtr> > const & sharedValue,
     std::size_t capacity,
     std::size_t length)
{
    value = sharedValue;
    setCapacityLength(capacity,length);
}

void PVStructureArray::serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher) const {
    serialize(pbuffer, pflusher, 0, getLength());
}

void PVStructureArray::deserialize(ByteBuffer *pbuffer,
        DeserializableControl *pcontrol) {
    size_t size = SerializeHelper::readSize(pbuffer, pcontrol);
    if(size>=0) {
        // prepare array, if necessary
        if(size>getCapacity()) setCapacity(size);
        PVStructurePtrArray pvArray = *value.get();
        for(size_t i = 0; i<size; i++) {
            pcontrol->ensureData(1);
            size_t temp = pbuffer->getByte();
            if(temp==0) {
                pvArray[i].reset();
            }
            else {
                if(pvArray[i].get()==NULL) {
                    StructureConstPtr structure = structureArray->getStructure();
                    pvArray[i] = getPVDataCreate()->createPVStructure(structure);
                }
                pvArray[i]->deserialize(pbuffer, pcontrol);
            }
        }
        setLength(size);
        postPut();
    }
}

void PVStructureArray::serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher, size_t offset, size_t count) const {
    // cache
    size_t length = getLength();

    // check bounds
    if(offset<0)
        offset = 0;
    else if(offset>length) offset = length;
    if(count<0) count = length;

    size_t maxCount = length-offset;
    if(count>maxCount) count = maxCount;

    PVStructurePtrArray pvArray = *value.get();
    // write
    SerializeHelper::writeSize(count, pbuffer, pflusher);
    for(size_t i = 0; i<count; i++) {
        if(pbuffer->getRemaining()<1) pflusher->flushSerializeBuffer();
        PVStructurePtr pvStructure = pvArray[i+offset];
        if(pvStructure.get()==NULL) {
            pbuffer->putByte(0);
        }
        else {
            pbuffer->putByte(1);
            pvStructure->serialize(pbuffer, pflusher);
        }
    }
}

}}
