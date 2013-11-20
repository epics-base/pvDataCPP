/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 * @author Marty Kraimer
 * @date 2013.07
 */
#include <string>
#include <stdexcept>
#include <memory>

#define epicsExportSharedSymbols
#include <pv/pvSubArrayCopy.h>

namespace epics { namespace pvData { 

template<typename T>
void copy(
    PVValueArray<T> & pvFrom,
    size_t fromOffset,
    PVValueArray<T> & pvTo,
    size_t toOffset,
    size_t len)
{
    if(pvTo.isImmutable()) {
        throw std::logic_error("pvSubArrayCopy  to is immutable");
    }
    size_t fromLength = pvFrom.getLength();
    if(fromOffset+len>fromLength) {
        throw std::length_error("pvSubArrayCopy from length error");
    }
    size_t capacity = pvTo.getCapacity();
    if(toOffset+len>capacity) capacity = toOffset + len;
    shared_vector<T> temp(capacity);
    typename PVValueArray<T>::const_svector vecFrom = pvFrom.view();
    typename PVValueArray<T>::const_svector vecTo = pvTo.view();
    for(size_t i=0; i<toOffset; ++i) temp[i] = vecTo[i];
    for(size_t i=0; i<len; ++i) temp[i + toOffset] = vecFrom[i + fromOffset];
    for(size_t i=len + toOffset; i<capacity; ++i) temp[i] = vecTo[i];
    shared_vector<const T> temp2(freeze(temp));
    pvTo.replace(temp2);
}

void copy(
    PVScalarArray & from,
    size_t fromOffset,
    PVScalarArray & to,
    size_t toOffset,
    size_t len)
{
    ScalarType scalarType = from.getScalarArray()->getElementType();
    ScalarType otherType = to.getScalarArray()->getElementType();
    if(scalarType!=otherType) {
        throw std::invalid_argument("pvSubArrayCopy element types do not match");
    }
    switch(scalarType)
    {
    case pvBoolean:
       {
           copy(dynamic_cast<PVValueArray<boolean> &>(from),fromOffset,
           dynamic_cast<PVValueArray<boolean>& >(to),
           toOffset,len);
       }
       break;
    case pvByte:
       {
           copy(dynamic_cast<PVValueArray<int8> &>(from),fromOffset,
           dynamic_cast<PVValueArray<int8>& >(to),
           toOffset,len);
       }
       break;
    case pvShort:
       {
           copy(dynamic_cast<PVValueArray<int16> &>(from),fromOffset,
           dynamic_cast<PVValueArray<int16>& >(to),
           toOffset,len);
       }
       break;
    case pvInt:
       {
           copy(dynamic_cast<PVValueArray<int32> &>(from),fromOffset,
           dynamic_cast<PVValueArray<int32>& >(to),
           toOffset,len);
       }
       break;
    case pvLong:
       {
           copy(dynamic_cast<PVValueArray<int64> &>(from),fromOffset,
           dynamic_cast<PVValueArray<int64>& >(to),
           toOffset,len);
       }
       break;
    case pvUByte:
       {
           copy(dynamic_cast<PVValueArray<uint8> &>(from),fromOffset,
           dynamic_cast<PVValueArray<uint8>& >(to),
           toOffset,len);
       }
       break;
    case pvUShort:
       {
           copy(dynamic_cast<PVValueArray<uint16> &>(from),fromOffset,
           dynamic_cast<PVValueArray<uint16>& >(to),
           toOffset,len);
       }
       break;
    case pvUInt:
       {
           copy(dynamic_cast<PVValueArray<uint32> &>(from),fromOffset,
           dynamic_cast<PVValueArray<uint32>& >(to),
           toOffset,len);
       }
       break;
    case pvULong:
       {
           copy(dynamic_cast<PVValueArray<uint64> &>(from),fromOffset,
           dynamic_cast<PVValueArray<uint64>& >(to),
           toOffset,len);
       }
       break;
    case pvFloat:
       {
           copy(dynamic_cast<PVValueArray<float> &>(from),fromOffset,
           dynamic_cast<PVValueArray<float>& >(to),
           toOffset,len);
       }
       break;
    case pvDouble:
       {
           copy(dynamic_cast<PVValueArray<double> &>(from),fromOffset,
           dynamic_cast<PVValueArray<double>& >(to),
           toOffset,len);
       }
       break;
    case pvString:
       {
           copy(dynamic_cast<PVValueArray<String> &>(from),fromOffset,
           dynamic_cast<PVValueArray<String>& >(to),
           toOffset,len);
       }
       break;
    }
}

void copy(
    PVStructureArray & from,
    size_t fromOffset,
    PVStructureArray & to,
    size_t toOffset,
    size_t len)
{
    if(to.isImmutable()) {
        throw std::logic_error("pvSubArrayCopy  to is immutable");
    }
    StructureArrayConstPtr fromStructure = from.getStructureArray();
    StructureArrayConstPtr toStructure = to.getStructureArray();
    if(fromStructure!=toStructure) {
        throw std::invalid_argument(
            "pvSubArrayCopy structureArray to and from have different structures");
    }
    size_t fromLength = from.getLength();
    if(fromOffset+len>fromLength) {
        throw std::length_error("pvSubArrayCopy from length error");
    }
    size_t capacity = to.getCapacity();
    if(toOffset+len>capacity) capacity = toOffset+len;
    shared_vector<PVStructurePtr> temp(capacity);
    PVValueArray<PVStructurePtr>::const_svector vecFrom = from.view();
    PVValueArray<PVStructurePtr>::const_svector vecTo = to.view();
    for(size_t i=0; i<toOffset; ++i) temp[i] = vecTo[i];
    for(size_t i=0; i<len; ++i) temp[i + toOffset] = vecFrom[i + fromOffset];
    for(size_t i=len + toOffset; i<capacity; ++i) temp[i] = vecTo[i];
    shared_vector<const PVStructurePtr> temp2(freeze(temp));
    to.replace(temp2);
}

void copy(
    PVArray & from,
    size_t fromOffset,
    PVArray & to,
    size_t toOffset,
    size_t len)
{
    Type type = from.getField()->getType();
    Type otherType = to.getField()->getType();
    if(type!=otherType) {
        throw std::invalid_argument("pvSubArrayCopy types do not match");
    }
    if(type==scalarArray) {
           copy(dynamic_cast<PVScalarArray &>(from) ,fromOffset,
           dynamic_cast<PVScalarArray&>(to),
           toOffset,len);
    }
    if(type==structureArray) {
           copy(dynamic_cast<PVStructureArray &>(from) ,fromOffset,
           dynamic_cast<PVStructureArray&>(to),
           toOffset,len);
    }
}

}}

