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
        epicsBoolean capacityMutable;
    };

    PVArray::PVArray(PVStructure *parent,FieldConstPtr field)
    : PVField(parent,field),pImpl(new PVArrayPvt())
    { }

    PVArray::~PVArray()
    {
        delete pImpl;
    }

     int PVArray::getLength() const {return pImpl->length;}

     static std::string fieldImmutable("field is immutable");

     void PVArray::setLength(int length) {
        if(PVField::isImmutable()) {
           PVField::message(&fieldImmutable,errorMessage);
           return;
        }
        if(length>pImpl->capacity) this->setCapacity(length);
        if(length>pImpl->capacity) length = pImpl->capacity;
        pImpl->length = length;
     }


     epicsBoolean PVArray::isCapacityImmutable() const
     {
          if(PVField::isImmutable()) {
              return epicsFalse;
          }
          return pImpl->capacityMutable;
     }

     void PVArray::setCapacityImmutable(epicsBoolean isMutable)
     {
        if(isMutable && PVField::isImmutable()) {
           PVField::message(&fieldImmutable,errorMessage);
           return;
        }
        pImpl->capacityMutable = isMutable;
     }

     static std::string capacityImmutable("capacity is immutable");

     void PVArray::setCapacity(int capacity) {
        if(PVField::isImmutable()) {
           PVField::message(&fieldImmutable,errorMessage);
           return;
        }
        if(pImpl->capacityMutable==epicsFalse) {
           PVField::message(&capacityImmutable,errorMessage);
           return;
        }
        pImpl->capacity = capacity;
     }

     void PVArray::toString(StringPtr buf) const {toString(buf,0);}

}}
#endif  /* ABSTRACTPVARRAY_H */
