/*PVArray.cpp*/
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
#include <pv/factory.h>

using std::size_t;

namespace epics { namespace pvData {

    class PVArrayPvt {
    public:
        PVArrayPvt() : length(0),capacity(0),capacityMutable(true)
        {}
        size_t length;
        size_t capacity;
        bool capacityMutable;
    };

    PVArray::PVArray(FieldConstPtr field)
    : PVField(field),pImpl(new PVArrayPvt())
    { }

    PVArray::~PVArray()
    {
        delete pImpl;
    }

     size_t PVArray::getLength() const {return pImpl->length;}

     size_t PVArray::getCapacity() const {return pImpl->capacity;}

     static String fieldImmutable("field is immutable");

     void PVArray::setLength(size_t length) {
        if(length==pImpl->length) return;
        if(PVField::isImmutable()) {
           PVField::message(fieldImmutable,errorMessage);
           return;
        }
        if(length>pImpl->capacity) this->setCapacity(length);
        if(length>pImpl->capacity) length = pImpl->capacity;
        pImpl->length = length;
     }

     void PVArray::setCapacityLength(size_t capacity,size_t length) {
        pImpl->capacity = capacity;
        pImpl->length = length;
     }


     bool PVArray::isCapacityMutable() 
     {
          if(PVField::isImmutable()) {
              return false;
          }
          return pImpl->capacityMutable;
     }

     void PVArray::setCapacityMutable(bool isMutable)
     {
        if(isMutable && PVField::isImmutable()) {
           PVField::message(fieldImmutable,errorMessage);
           return;
        }
        pImpl->capacityMutable = isMutable;
     }

     static String capacityImmutable("capacity is immutable");

     void PVArray::setCapacity(size_t capacity) {
        if(PVField::isImmutable()) {
           PVField::message(fieldImmutable,errorMessage);
           return;
        }
        if(pImpl->capacityMutable==false) {
           PVField::message(capacityImmutable,errorMessage);
           return;
        }
        pImpl->capacity = capacity;
     }

}}
