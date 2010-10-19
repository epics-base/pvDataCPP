/*AbstractPVArray.h*/
#ifndef ABSTRACTPVARRAY_H
#define ABSTRACTPVARRAY_H
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "pvData.h"
#include "factory.h"

namespace epics { namespace pvData {

    class PVArrayPvt {
    public:
        PVArrayPvt() : length(0),capacity(0),capacityMutable(epicsTrue)
        {}
        int length;
        int capacity;
        bool capacityMutable;
    };

    PVArray::PVArray(PVStructure *parent,FieldConstPtr field)
    : PVField(parent,field),pImpl(new PVArrayPvt())
    { }

    PVArray::~PVArray()
    {
        delete pImpl;
    }

     int PVArray::getLength()  {return pImpl->length;}

     int PVArray::getCapacity()  {return pImpl->capacity;}

     static String fieldImmutable("field is immutable");

     void PVArray::setLength(int length) {
        if(length==pImpl->length) return;
        if(PVField::isImmutable()) {
           PVField::message(fieldImmutable,errorMessage);
           return;
        }
        if(length>pImpl->capacity) this->setCapacity(length);
        if(length>pImpl->capacity) length = pImpl->capacity;
        pImpl->length = length;
     }

     void PVArray::setCapacityLength(int capacity,int length) {
        pImpl->capacity = capacity;
        pImpl->length = length;
     }


     bool PVArray::isCapacityMutable() 
     {
          if(PVField::isImmutable()) {
              return epicsFalse;
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

     void PVArray::setCapacity(int capacity) {
        if(PVField::isImmutable()) {
           PVField::message(fieldImmutable,errorMessage);
           return;
        }
        if(pImpl->capacityMutable==epicsFalse) {
           PVField::message(capacityImmutable,errorMessage);
           return;
        }
        pImpl->capacity = capacity;
     }

}}
#endif  /* ABSTRACTPVARRAY_H */
