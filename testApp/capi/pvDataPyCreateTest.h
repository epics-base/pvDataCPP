/* pvDataPyCreateTest.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#ifndef PVDATAPYCREATETEST_H
#define PVDATAPYCREATETEST_H
#include <pv/pvDataPyCAPI.h>
#include <pv/standardPVField.h>

extern "C"
{
    /**
     * Create top level structure with a scalar value field.
     * @param scalarType The scalarType for the value field.
     * &param properties Some combination of "alarm,timeStamp,display,control"
     * @return void *addrPVTopPyPtr.
     * PVTopPyPtr has the address of the top level structure
     */
    extern void * pvPyCreateScalar(int scalarType,const char *properties);
    /**
     * Create top level structure with a scalar array value field.
     * @param elementType The scalarType for the value field elements.
     * &param properties Some combination of "alarm,timeStamp,display,control"
     * @return void *addrPVTopPyPtr.
     * PVTopPyPtr has the address of the top level structure
     */
    extern void * pvPyCreateScalarArray(int elementType,const char *properties);
    /**
     * Get the top level structure.
     * @param addrPVTopPyPtr PVTopPyPtr has the address of the top level structure
     * @return void *addrPVStructurePyPtr
     */
    extern void * pvPyGetTop(void * addrPVTopPyPtr);
    /**
     * Delete PVTopPyPtr.
     * Also deletes the top level structure.
     * @param addrPVTopPyPtr PVTopPyPtr has the address of the top level structure
     */
    extern void pvPyDeleteTop(void *addrPVTopPyPtr);
}

#endif  /* PVDATAPYCREATETEST_H */
