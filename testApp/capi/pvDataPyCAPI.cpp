/* pvDataPyCAPI.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */

#include <pv/pvDataPyCAPI.h>
#include <pv/convert.h>

using namespace epics::pvData;

static ConvertPtr convert = getConvert();

extern "C"
{
    void * pvPyGetPVStructurePyPtr(void * addrPVTopPyPtr)
    {
         PVTopPyPtr * p = static_cast<PVTopPyPtr *>(addrPVTopPyPtr);
         PVStructurePyPtr * addr = (*p)->addrPVStructurePyPtr();
         return addr;
    }

    void pvPyDumpPVStructurePy(void *addrPVStructurePyPtr)
    {
         PVStructurePyPtr *p = static_cast<PVStructurePyPtr *>(addrPVStructurePyPtr);
         PVStructurePtr pvStructurePtr = (*p)->getPVStructurePtr();
         String buffer;
         pvStructurePtr->toString(&buffer);
         printf("%s\n",buffer.c_str());
    }

    int pvPyGetSubfield(void *addrPVStructurePyPtr, const char *fieldName)
    {
         PVStructurePyPtr *p = static_cast<PVStructurePyPtr *>(addrPVStructurePyPtr);
         PVStructurePtr pvStructurePtr = (*p)->getPVStructurePtr();
         String buffer;
         String name(fieldName);
         return pvStructurePtr->getStructure()->getFieldIndex(name);
    }

    size_t pvPyGetNumberFields(void *addrPVStructurePyPtr)
    {
         PVStructurePyPtr *p = static_cast<PVStructurePyPtr *>(addrPVStructurePyPtr);
         PVStructurePtr pvStructurePtr = (*p)->getPVStructurePtr();
         return pvStructurePtr->getStructure()->getNumberFields();
    }

    int pvPyGetFieldType(void *addrPVStructurePyPtr, size_t index)
    {
         PVStructurePyPtr *p = static_cast<PVStructurePyPtr *>(addrPVStructurePyPtr);
         PVStructurePtr pvStructurePtr = (*p)->getPVStructurePtr();
         size_t offset = pvStructurePtr->getFieldOffset() + index + 1;
         PVFieldPtr pvField = pvStructurePtr->getSubField(offset);
         return pvField->getField()->getType();
    }

    const char *pvPyGetFieldName(void *addrPVStructurePyPtr, size_t index)
    {
         PVStructurePyPtr *p = static_cast<PVStructurePyPtr *>(addrPVStructurePyPtr);
         PVStructurePtr pvStructurePtr = (*p)->getPVStructurePtr();
         size_t offset = pvStructurePtr->getFieldOffset() + index + 1;
         PVFieldPtr pvField = pvStructurePtr->getSubField(offset);
         return pvField->getFieldName().c_str();
    }

    void * pvPyGetPVScalarPy(void *addrPVStructurePyPtr, size_t index)
    {
         PVStructurePyPtr *p = static_cast<PVStructurePyPtr *>(addrPVStructurePyPtr);
         PVFieldPyPtrArrayPtr pvFieldPyPtrArrayPtr = (*p)->getPVFieldPyPtrArrayPtr();
         PVFieldPyPtrArray *xxx = pvFieldPyPtrArrayPtr.get();
         PVFieldPyPtr *pvFieldPyPtr = &(*xxx)[index];
         void * addrPVScalarPyPtr = pvFieldPyPtr;
         return addrPVScalarPyPtr;
    }

    void * pvPyGetPVScalar(void * addrPVScalarPyPtr)
    {
         PVScalarPyPtr *p = static_cast<PVScalarPyPtr *>(addrPVScalarPyPtr);
         return (*p)->addrPVScalarPtr();
    }

    int pvPyScalarGetScalarType(void * addrPVScalarPtr)
    {
         PVScalarPtr *p = static_cast<PVScalarPtr *>(addrPVScalarPtr);
         return(*p)->getScalar()->getScalarType();
    }

    int pvPyScalarGetInt(void * addrPVScalarPtr)
    {
         PVScalarPtr *p = static_cast<PVScalarPtr *>(addrPVScalarPtr);
         return convert->toInt((*p));
    }

    void pvPyScalarPutInt(void * addrPVScalarPtr, int value)
    {
         PVScalarPtr *p = static_cast<PVScalarPtr *>(addrPVScalarPtr);
         convert->fromInt((*p),value);
    }

    double pvPyScalarGetDouble(void * addrPVScalarPtr)
    {
         PVScalarPtr *p = static_cast<PVScalarPtr *>(addrPVScalarPtr);
         return convert->toDouble((*p));
    }

    void pvPyScalarPutDouble(void * addrPVScalarPtr, double value)
    {
         PVScalarPtr *p = static_cast<PVScalarPtr *>(addrPVScalarPtr);
         convert->fromDouble((*p),value);
    }

    void * pvPyGetPVScalarArrayPy(void *addrPVStructurePyPtr, size_t index)
    {
         PVStructurePyPtr *p = static_cast<PVStructurePyPtr *>(addrPVStructurePyPtr);
         PVFieldPyPtrArrayPtr pvFieldPyPtrArrayPtr = (*p)->getPVFieldPyPtrArrayPtr();
         PVFieldPyPtrArray *xxx = pvFieldPyPtrArrayPtr.get();
         PVFieldPyPtr *pvFieldPyPtr = &(*xxx)[index];
         void * addrPVScalarArrayPyPtr = pvFieldPyPtr;
         return addrPVScalarArrayPyPtr;
    } 

    void * pvPyGetPVScalarArray(void * addrPVScalarArrayPyPtr)
    {
         PVScalarArrayPyPtr *p = static_cast<PVScalarArrayPyPtr *>(addrPVScalarArrayPyPtr);
         return (*p)->addrPVScalarArrayPtr();
    } 

    void * pvPyGetPVStructureArrayPy(void *addrPVStructurePyPtr, size_t index)
    {
         PVStructurePyPtr *p = static_cast<PVStructurePyPtr *>(addrPVStructurePyPtr);
         PVFieldPyPtrArrayPtr pvFieldPyPtrArrayPtr = (*p)->getPVFieldPyPtrArrayPtr();
         PVFieldPyPtrArray *xxx = pvFieldPyPtrArrayPtr.get();
         PVFieldPyPtr *pvFieldPyPtr = &(*xxx)[index];
         void * addrPVStructureArrayPyPtr = pvFieldPyPtr;
         return addrPVStructureArrayPyPtr;
    } 

    void * pvPyGetPVStructureArray(void * addrPVStructureArrayPyPtr)
    {
         PVStructureArrayPyPtr *p = static_cast<PVStructureArrayPyPtr *>(addrPVStructureArrayPyPtr);
         return (*p)->addrPVStructureArrayPtr();
    } 

    void * pvPyGetPVStructurePy(void *addrPVStructurePyPtr, size_t index)
    {
         PVStructurePyPtr *p = static_cast<PVStructurePyPtr *>(addrPVStructurePyPtr);
         PVFieldPyPtrArrayPtr pvFieldPyPtrArrayPtr = (*p)->getPVFieldPyPtrArrayPtr();
         PVFieldPyPtrArray *xxx = pvFieldPyPtrArrayPtr.get();
         PVFieldPyPtr *pvFieldPyPtr = &(*xxx)[index];
         void * subaddrPVStructureArrayPyPtr = pvFieldPyPtr;
         return subaddrPVStructureArrayPyPtr;
    } 

    void * pvPyGetPVStructure(void * addrPVStructurePyPtr)
    {
         PVStructurePyPtr *p = static_cast<PVStructurePyPtr *>(addrPVStructurePyPtr);
         return (*p)->addrPVStructurePtr();
    } 

}

