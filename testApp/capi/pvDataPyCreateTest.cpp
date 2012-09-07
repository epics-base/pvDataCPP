/* pvDataPyCreateTest.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#include <pv/pvDataPyCreateTest.h>

using namespace epics::pvData;

typedef struct PVTopPyAddr {
    PVTopPyAddr(PVTopPyPtr pvTop)
    : pvTop(pvTop)
    {
    }

    ~PVTopPyAddr()
    {
    }
    PVTopPyPtr pvTop;
} PVTopPyAddr;

extern "C"
{
    void * pvPyCreateScalar(int scalarType,const char *properties)
    {
        StandardPVFieldPtr standardPVField = getStandardPVField();
        String xxx(properties);
        ScalarType yyy = static_cast<ScalarType>(scalarType);
        return new PVTopPyAddr(
            PVTopPy::createTop(standardPVField->scalar(yyy,xxx)));
    }

    void * pvPyCreateScalarArray(int elementType,const char *properties)
    {
        StandardPVFieldPtr standardPVField = getStandardPVField();
        String xxx(properties);
        ScalarType yyy = static_cast<ScalarType>(elementType);
        return new PVTopPyAddr(
            PVTopPy::createTop(standardPVField->scalarArray(yyy,xxx)));
    }


    void * pvPyGetTop(void * addrPVTopPyPtr)
    {
         PVTopPyAddr *p = static_cast<PVTopPyAddr *>(addrPVTopPyPtr);
         return &(p->pvTop);
    }

    void pvPyDeleteTop(void *addrPVTopPyPtr)
    {
         PVTopPyAddr *p = static_cast<PVTopPyAddr *>(addrPVTopPyPtr);
         delete p;
    }

}
