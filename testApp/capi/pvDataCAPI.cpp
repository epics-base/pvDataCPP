/* pvDataCAPI.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#include <pv/pvDataCAPI.h>
#include <pv/convert.h>

using std::tr1::static_pointer_cast;
using std::size_t;

namespace epics { namespace pvData { 

PVFieldCAPI::PVFieldCAPI(Type type)
: type(type){}
 
typedef struct PVScalarCAPIAddr {
    PVScalarCAPIAddr(PVScalarCAPIPtr const &pvScalarCAPIPtr)
    : pvScalarCAPIPtr(pvScalarCAPIPtr) {}
    ~PVScalarCAPIAddr() {}
    PVScalarCAPIPtr pvScalarCAPIPtr;
} PVScalarCAPIAddr;

void *PVScalarCAPI::create(PVScalarPtr const & pvScalarPtr)
{
    PVScalarCAPIPtr xxx(new PVScalarCAPI(pvScalarPtr));
    return new PVScalarCAPIAddr(xxx);
}

PVScalarCAPIPtr PVScalarCAPI::getPVScalarCAPIPtr(void *addrPVScalarCAPI)
{
    PVScalarCAPIAddr *xxx = static_cast<PVScalarCAPIAddr *>(addrPVScalarCAPI);
    return xxx->pvScalarCAPIPtr;
}

void PVScalarCAPI::destroy(void *addrPVScalarCAPI)
{
    PVScalarCAPIAddr *xxx = static_cast<PVScalarCAPIAddr *>(addrPVScalarCAPI);
    delete xxx;
}

PVScalarCAPI::PVScalarCAPI(PVScalarPtr const & pvScalarPtr)
    : PVFieldCAPI(pvScalarPtr->getField()->getType()),
      pvScalarPtr(pvScalarPtr)
      {}
 
typedef struct PVScalarArrayCAPIAddr {
    PVScalarArrayCAPIAddr(PVScalarArrayCAPIPtr const &pvScalarArrayCAPIPtr)
    : pvScalarArrayCAPIPtr(pvScalarArrayCAPIPtr) {}
    ~PVScalarArrayCAPIAddr() {}
    PVScalarArrayCAPIPtr pvScalarArrayCAPIPtr;
} PVScalarArrayCAPIAddr;

void *PVScalarArrayCAPI::create(PVScalarArrayPtr const & pvScalarArrayPtr)
{
    PVScalarArrayCAPIPtr xxx(new PVScalarArrayCAPI(pvScalarArrayPtr));
    return new PVScalarArrayCAPIAddr(xxx);
}

PVScalarArrayCAPIPtr PVScalarArrayCAPI::getPVScalarArrayCAPIPtr(
    void *addrPVScalarArrayCAPI)
{
    PVScalarArrayCAPIAddr *xxx =
          static_cast<PVScalarArrayCAPIAddr *>(addrPVScalarArrayCAPI);
    return xxx->pvScalarArrayCAPIPtr;
}

void PVScalarArrayCAPI::destroy(void *addrPVScalarArrayCAPI)
{
    PVScalarArrayCAPIAddr *xxx =
          static_cast<PVScalarArrayCAPIAddr *>(addrPVScalarArrayCAPI);
    delete xxx;
}

PVScalarArrayCAPI::PVScalarArrayCAPI(PVScalarArrayPtr const & pvScalarArrayPtr)
    : PVFieldCAPI(pvScalarArrayPtr->getField()->getType()),
      pvScalarArrayPtr(pvScalarArrayPtr)
      {}

typedef struct PVStructureArrayCAPIAddr {
    PVStructureArrayCAPIAddr(
        PVStructureArrayCAPIPtr const &pvStructureArrayCAPIPtr)
    : pvStructureArrayCAPIPtr(pvStructureArrayCAPIPtr) {}
    ~PVStructureArrayCAPIAddr() {}
    PVStructureArrayCAPIPtr pvStructureArrayCAPIPtr;
} PVStructureArrayCAPIAddr;

void *PVStructureArrayCAPI::create(
    PVStructureArrayPtr const & pvStructureArrayPtr)
{
   PVStructureArrayCAPIPtr xxx(new PVStructureArrayCAPI(pvStructureArrayPtr));
   return new PVStructureArrayCAPIAddr(xxx);
}

PVStructureArrayCAPIPtr PVStructureArrayCAPI::getPVStructureArrayCAPIPtr(
    void *addrPVStructureArrayCAPI)
{
    PVStructureArrayCAPIAddr *xxx =
        static_cast<PVStructureArrayCAPIAddr *>(addrPVStructureArrayCAPI);
    return xxx->pvStructureArrayCAPIPtr;
}

void PVStructureArrayCAPI::destroy(void *addrPVStructureArrayCAPI)
{
    PVStructureArrayCAPIAddr *xxx =
        static_cast<PVStructureArrayCAPIAddr *>(addrPVStructureArrayCAPI);
    delete xxx;
}

PVStructureArrayCAPI::PVStructureArrayCAPI(
    PVStructureArrayPtr const & pvStructureArrayPtr)
    : PVFieldCAPI(pvStructureArrayPtr->getField()->getType()),
      pvStructureArrayPtr(pvStructureArrayPtr)
      {}

PVStructureCAPI:: PVStructureCAPI(PVStructurePtr const &pvStructurePtr)
       : PVFieldCAPI(structure),
         pvStructurePtr(pvStructurePtr)
         {}
 
typedef struct PVStructureCAPIAddr {
    PVStructureCAPIAddr(PVStructureCAPIPtr const &pvStructureCAPIPtr)
    : pvStructureCAPIPtr(pvStructureCAPIPtr) {}
    ~PVStructureCAPIAddr() {}
    PVStructureCAPIPtr pvStructureCAPIPtr;
} PVStructureCAPIAddr;

void *PVStructureCAPI::create(PVStructurePtr const &pvStructurePtr)
{
    PVStructureCAPIPtr pvStructureCAPIPtr =
        PVStructureCAPI::createPtr(pvStructurePtr);
    return new PVStructureCAPIAddr(pvStructureCAPIPtr);
}

PVStructureCAPIPtr PVStructureCAPI::getPVStructureCAPIPtr(
    void *addrPVStructureCAPI)
{
    PVStructureCAPIAddr *xxx =
        static_cast<PVStructureCAPIAddr *>(addrPVStructureCAPI);
    return xxx->pvStructureCAPIPtr;
}

void PVStructureCAPI::destroy(void *addrPVStructureCAPI)
{
    PVStructureCAPIAddr *xxx =
        static_cast<PVStructureCAPIAddr *>(addrPVStructureCAPI);
    delete xxx;
}

PVStructureCAPIPtr PVStructureCAPI::createPtr(PVStructurePtr const & pvStructurePtr)
{
    PVStructureCAPIPtr pvStructureCAPIPtr =
       PVStructureCAPIPtr(new PVStructureCAPI(pvStructurePtr));
    const PVFieldPtrArray & pvFields = pvStructurePtr->getPVFields();
    size_t n = pvFields.size();
    PVFieldCAPIPtrArrayPtr pvCAPIFields(new PVFieldCAPIPtrArray());
    pvCAPIFields->reserve(n);
    for(size_t i = 0; i<n; i++) {
        PVFieldPtr pvField = pvFields[i];
        Type type = pvField->getField()->getType();
        PVFieldCAPIPtr pvFieldCAPI;
        switch(type) {
        case scalar : 
            {
                PVScalarPtr xxx = static_pointer_cast<PVScalar>(pvField);
                pvFieldCAPI = PVScalarCAPIPtr(new PVScalarCAPI(xxx));
            }
            break;
        case scalarArray :
            {
                PVScalarArrayPtr xxx =
                    static_pointer_cast<PVScalarArray>(pvField);
                pvFieldCAPI = PVScalarArrayCAPIPtr(new PVScalarArrayCAPI(xxx));
            }
            break;
        case structureArray : 
            {
                PVStructureArrayPtr xxx =
                    static_pointer_cast<PVStructureArray>(pvField);
                pvFieldCAPI = PVStructureArrayCAPIPtr(
                    new PVStructureArrayCAPI(xxx));
            }
            break;
        case structure :
            {
                PVStructurePtr xxx = static_pointer_cast<PVStructure>(pvField);
                pvFieldCAPI = PVStructureCAPIPtr(createPtr(xxx));
            }
            break;
        }
        pvCAPIFields->push_back(pvFieldCAPI);
    }
    pvStructureCAPIPtr->pvFieldCAPIPtrArrayPtr = pvCAPIFields;
    return pvStructureCAPIPtr;
}

static ConvertPtr convert = getConvert();

extern "C"
{
    void pvCAPIDestroyPVStructureCAPI(void *addrPVStructureCAPIAddr)
    {
         PVStructureCAPIAddr *p = static_cast<PVStructureCAPIAddr *>(addrPVStructureCAPIAddr);
         delete p;
    }

    void pvCAPIDumpPVStructureCAPI(void *addrPVStructureCAPIAddr)
    {
         PVStructureCAPIAddr *p = static_cast<PVStructureCAPIAddr *>(addrPVStructureCAPIAddr);
         PVStructurePtr pvStructurePtr = p->pvStructureCAPIPtr->getPVStructurePtr();
         String buffer;
         pvStructurePtr->toString(&buffer);
         printf("%s\n",buffer.c_str());
    }

    int pvCAPIGetSubfield(void *addrPVStructureCAPIAddr, const char *fieldName)
    {
         PVStructureCAPIAddr *p = static_cast<PVStructureCAPIAddr *>(addrPVStructureCAPIAddr);
         PVStructurePtr pvStructurePtr = p->pvStructureCAPIPtr->getPVStructurePtr();
         String buffer;
         String name(fieldName);
         return pvStructurePtr->getStructure()->getFieldIndex(name);
    }

    size_t pvCAPIGetNumberFields(void *addrPVStructureCAPIAddr)
    {
         PVStructureCAPIAddr *p = static_cast<PVStructureCAPIAddr *>(addrPVStructureCAPIAddr);
         PVStructurePtr pvStructurePtr = p->pvStructureCAPIPtr->getPVStructurePtr();
         return pvStructurePtr->getStructure()->getNumberFields();
    }

    int pvCAPIGetFieldType(void *addrPVStructureCAPIAddr, size_t index)
    {
         PVStructureCAPIAddr *p = static_cast<PVStructureCAPIAddr *>(addrPVStructureCAPIAddr);
         PVStructurePtr pvStructurePtr = p->pvStructureCAPIPtr->getPVStructurePtr();
         size_t offset = pvStructurePtr->getFieldOffset() + index + 1;
         PVFieldPtr pvField = pvStructurePtr->getSubField(offset);
         return pvField->getField()->getType();
    }

    const char *pvCAPIGetFieldName(void *addrPVStructureCAPIAddr, size_t index)
    {
         PVStructureCAPIAddr *p = static_cast<PVStructureCAPIAddr *>(addrPVStructureCAPIAddr);
         PVStructurePtr pvStructurePtr = p->pvStructureCAPIPtr->getPVStructurePtr();
         size_t offset = pvStructurePtr->getFieldOffset() + index + 1;
         PVFieldPtr pvField = pvStructurePtr->getSubField(offset);
         return pvField->getFieldName().c_str();
    }

    void * pvCAPIGetPVScalarCAPI(void *addrPVStructureCAPIAddr, size_t index)
    {
         PVStructureCAPIAddr *p = static_cast<PVStructureCAPIAddr *>(addrPVStructureCAPIAddr);
         PVStructurePtr pvStructurePtr = p->pvStructureCAPIPtr->getPVStructurePtr();
         size_t offset = pvStructurePtr->getFieldOffset() + index + 1;
         PVFieldPtr pvField = pvStructurePtr->getSubField(offset);
         if(pvField->getField()->getType()!=scalar) return NULL;;
         return PVScalarCAPI::create(static_pointer_cast<PVScalar>(pvField));
    }

    void pvCAPIDestroyPVScalarCAPI(void *addrPVScalarCAPIAddr)
    {
         PVScalarCAPIAddr *p = static_cast<PVScalarCAPIAddr *>(addrPVScalarCAPIAddr);
         delete p;
    }

    int pvCAPIScalarGetScalarType(void * addrPVScalarCAPIAddr)
    {
         PVScalarCAPIAddr *p = static_cast<PVScalarCAPIAddr *>(addrPVScalarCAPIAddr);
         PVScalarCAPIPtr pvScalarCAPIPtr = p->pvScalarCAPIPtr;
         return pvScalarCAPIPtr->getPVScalarPtr()->getScalar()->getScalarType();
    }

    int pvCAPIScalarGetInt(void * addrPVScalarCAPIAddr)
    {
         PVScalarCAPIAddr *p = static_cast<PVScalarCAPIAddr *>(addrPVScalarCAPIAddr);
         PVScalarCAPIPtr pvScalarCAPIPtr = p->pvScalarCAPIPtr;
         PVScalarPtr pvScalarPtr = pvScalarCAPIPtr->getPVScalarPtr();
         return convert->toInt(pvScalarPtr);
    }

    void pvCAPIScalarPutInt(void * addrPVScalarCAPIAddr, int value)
    {
         PVScalarCAPIAddr *p = static_cast<PVScalarCAPIAddr *>(addrPVScalarCAPIAddr);
         PVScalarCAPIPtr pvScalarCAPIPtr = p->pvScalarCAPIPtr;
         PVScalarPtr pvScalarPtr = pvScalarCAPIPtr->getPVScalarPtr();
         convert->fromInt(pvScalarPtr,value);
    }

    double pvCAPIScalarGetDouble(void * addrPVScalarCAPIAddr)
    {
         PVScalarCAPIAddr *p = static_cast<PVScalarCAPIAddr *>(addrPVScalarCAPIAddr);
         PVScalarCAPIPtr pvScalarCAPIPtr = p->pvScalarCAPIPtr;
         PVScalarPtr pvScalarPtr = pvScalarCAPIPtr->getPVScalarPtr();
         return convert->toDouble(pvScalarPtr);
    }

    void pvCAPIScalarPutDouble(void * addrPVScalarCAPIAddr, double value)
    {
         PVScalarCAPIAddr *p = static_cast<PVScalarCAPIAddr *>(addrPVScalarCAPIAddr);
         PVScalarCAPIPtr pvScalarCAPIPtr = p->pvScalarCAPIPtr;
         PVScalarPtr pvScalarPtr = pvScalarCAPIPtr->getPVScalarPtr();
         convert->fromDouble(pvScalarPtr,value);
    }

    void * pvCAPIGetPVScalarArrayCAPI(void *addrPVStructureCAPIAddr, size_t index)
    {
         PVStructureCAPIAddr *p = static_cast<PVStructureCAPIAddr *>(addrPVStructureCAPIAddr);
         PVStructurePtr pvStructurePtr = p->pvStructureCAPIPtr->getPVStructurePtr();
         size_t offset = pvStructurePtr->getFieldOffset() + index + 1;
         PVFieldPtr pvField = pvStructurePtr->getSubField(offset);
         if(pvField->getField()->getType()!=scalarArray) return NULL;;
         return PVScalarArrayCAPI::create(static_pointer_cast<PVScalarArray>(pvField));
    } 

    void pvCAPIDestroyPVScalarArrayCAPI(void *addrPVScalarArrayCAPIAddr)
    {
         PVScalarArrayCAPIAddr *p =
            static_cast<PVScalarArrayCAPIAddr *>(addrPVScalarArrayCAPIAddr);
         delete p;
    }

    void * pvCAPIGetPVStructureArrayCAPI(void *addrPVStructureCAPIAddr, size_t index)
    {
         PVStructureCAPIAddr *p = static_cast<PVStructureCAPIAddr *>(addrPVStructureCAPIAddr);
         PVStructurePtr pvStructurePtr = p->pvStructureCAPIPtr->getPVStructurePtr();
         size_t offset = pvStructurePtr->getFieldOffset() + index + 1;
         PVFieldPtr pvField = pvStructurePtr->getSubField(offset);
         if(pvField->getField()->getType()!=scalarArray) return NULL;;
         return PVStructureArrayCAPI::create(static_pointer_cast<PVStructureArray>(pvField));
    } 

    void pvCAPIDestroyPVStructureArrayCAPI(void *addrPVStructureArrayCAPIAddr)
    {
         PVStructureArrayCAPIAddr *p =
            static_cast<PVStructureArrayCAPIAddr *>(addrPVStructureArrayCAPIAddr);
         delete p;
    }

    void * pvCAPIGetPVStructureCAPI(void *addrPVStructureCAPIAddr, size_t index)
    {
         PVStructureCAPIAddr *p = static_cast<PVStructureCAPIAddr *>(addrPVStructureCAPIAddr);
         PVStructurePtr pvStructurePtr = p->pvStructureCAPIPtr->getPVStructurePtr();
         size_t offset = pvStructurePtr->getFieldOffset() + index + 1;
         PVFieldPtr pvField = pvStructurePtr->getSubField(offset);
         if(pvField->getField()->getType()!=structure) return NULL;;
         return PVStructureCAPI::create(static_pointer_cast<PVStructure>(pvField));
    } 

}

}}
