/* pvDataCAPICreateTest.h */
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
#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Create top level structure with a scalar value field.
     * @param scalarType The scalarType for the value field.
     * &param properties Some combination of "alarm,timeStamp,display,control"
     * @return void *addrPVStructureCAPIAddr.
     */
    extern void * pvCAPICreateScalar(int scalarType,const char *properties);
    /**
     * Create top level structure with a scalar array value field.
     * @param elementType The scalarType for the value field elements.
     * &param properties Some combination of "alarm,timeStamp,display,control"
     * @return void *addrPVStructureCAPIAddr.
     */
    extern void * pvCAPICreateScalarArray(int elementType,const char *properties);

#ifdef __cplusplus
}
#endif

#endif  /* PVDATAPYCREATETEST_H */
