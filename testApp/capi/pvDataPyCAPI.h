/* pvDataPy.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#ifndef PVDATAPYCAPI_H
#define PVDATAPYCAPI_H
#include <pv/pvDataPy.h>

extern "C"
{
    /**
     * Get the addrPVStructurePyPtr 
     * @param addrPVTopPyPtr PVTopPyPtr has the address of the top level structure.
     * @return void *addrPVStructurePyPtr
     */
    extern void * pvPyGetPVStructurePyPtr(void * addrPVTopPyPtr);
    /**
     * Dump the structure
     * @param addrPVStructurePyPtr PVStructurePyPtr holds the PVStructurePy.
     */
    extern void pvPyDumpPVStructurePy(void *addrPVStructurePyPtr);
    /**
     * Get the index of the subfield with the specified fieldName
     * @param addrPVStructurePyPtr PVStructurePyPtr holds the PVStructurePy.
     * @param fieldName The fieldname
     * @return The index or -1 if the field is not found.
     */
    extern int pvPyGetSubfield(void *addrPVStructurePyPtr, const char *fieldName);
    /**
     * Get the number of subfields
     * @param addrPVStructurePyPtr PVStructurePyPtr holds the PVStructurePy.
     * @return The number of subfields.
     */
    extern size_t pvPyGetNumberFields(void *addrPVStructurePyPtr);
    /**
     * Get the field type
     * @param addrPVStructurePyPtr PVStructurePyPtr holds the PVStructurePy.
     * @param index The subfield.
     * @return The field type.
     */
    extern int pvPyGetFieldType(void *addrPVStructurePyPtr, size_t index);
    /**
     * Get the field name
     * @param addrPVStructurePyPtr PVStructurePyPtr holds the PVStructurePy.
     * @param index The subfield.
     * @return The field name.
     */
    extern const char *pvPyGetFieldName(void *addrPVStructurePyPtr, size_t index);
    /**
     * Get a scalar subfield.
     * @param addrPVStructurePyPtr PVStructurePyPtr holds the PVStructurePy.
     * @param index The subfield.
     * @return void * addrPVScalarPyPtr.
     */
    extern void * pvPyGetPVScalarPy(void *addrPVStructurePyPtr, size_t index);
    /**
     * Get a scalar subfield.
     * @param addrPVScalarPyPtr PVScalarPyPtr holds the PVScalarPy
     * @return void * addrPVScalarPtr.
     */
    extern void * pvPyGetPVScalar(void * addrPVScalarPyPtr);
    /**
     * Given a PVScalarPy get the scalarType
     * @param addrPVScalarPtr PVScalaryPtr holds the PVScalar
     * @return The type.
     */
    extern int pvPyScalarGetScalarType(void * addrPVScalarPtr);
    /**
     * Given a PVScalar get the value converted to integer.
     * @param addrPVScalar The PVScalar
     * @return The value converted to int.
     */
    extern int pvPyScalarGetInt(void * addrPVScalarPtr);
    /**
     * Given a PVScalar put the value
     * @param addrPVScalar The PVScalar
     * @param value The value.
     */
    extern void pvPyScalarPutInt(void * addrPVScalarPtr, int value);
    /**
     * Given a PVScalar get the value converted to double.
     * @param addrPVScalar The PVScalar
     * @return The value converted to double.
     */
    extern double pvPyScalarGetDouble(void * addrPVScalarPtr);
    /**
     * Given a PVScalar put the value
     * @param addrPVScalar The PVScalar
     * @param value The value.
     */
    extern void pvPyScalarPutDouble(void * addrPVScalarPtr, double value);
    /**
     * Get a scalar array subfield.
     * @param addrPVStructurePyPtr PVStructurePyPtr holds the PVStructurePy.
     * @param index The subfield.
     * @return void * addrPVScalarArrayPyPtr.
     */
    extern void * pvPyGetPVScalarArrayPy(void *addrPVStructurePyPtr, size_t index);
    /**
     * Get a scalar array subfield.
     * @param addrPVScalarArrayPyPtr PVScalarPyPtr holds the PVScalarArrayPy
     * @return void * addrPVScalarArrayPtr.
     */
    extern void * pvPyGetPVScalarArray(void * addrPVScalarArrayPyPtr);
    /**
     * Get a structure array subfield.
     * @param addrPVStructurePyPtr PVStructurePyPtr holds the PVStructurePy.
     * @param index The subfield.
     * @return void * addrPVStructureArrayPyPtr.
     */
    extern void * pvPyGetPVStructureArrayPy(void *addrPVStructurePyPtr, size_t index);
    /**
     * Get a structure array subfield.
     * @param addrPVStructureArrayPyPtr PVStructurePyPtr holds the PVStructureArrayPy
     * @return void * addrPVStructureArrayPtr.
     */
    extern void * pvPyGetPVStructureArray(void * addrPVStructureArrayPyPtr);
    /**
     * Get a structure subfield.
     * @param addrPVStructurePyPtr PVStructurePyPtr holds the PVStructurePy.
     * @param index The subfield.
     * @return void * addrPVStructurePyPtr.
     */
    extern void * pvPyGetPVStructurePy(void *addrPVStructurePyPtr, size_t index);
    /**
     * Get a structure subfield.
     * @param addrPVStructurePyPtr PVStructurePyPtr holds the PVStructurePy
     * @return void * addrPVStructurePtr.
     */
    extern void * pvPyGetPVStructure(void * addrPVStructurePyPtr);
  
}

    
#endif  /* PVDATAPYCAPI_H */
