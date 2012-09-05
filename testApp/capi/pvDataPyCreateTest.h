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

typedef struct PVTopPyAddr {
    epics::pvData::PVTopPyPtr pvTop;
} PVTopPyAddr;

extern "C"
{
    void * pvPyCreateScalar(int scalarType,const char *properties)
    {
        PVTopPyAddr *pPVTopPyAddr = new PVTopPyAddr();
        epics::pvData::StandardPVFieldPtr standardPVField
            = epics::pvData::getStandardPVField();
        epics::pvData::String xxx(properties);
        epics::pvData::ScalarType yyy
            = static_cast<epics::pvData::ScalarType>(scalarType);
        epics::pvData::PVStructurePtr pv = standardPVField->scalar(yyy,xxx);
printf("pvPyCreateScalar pvStructure %p\n",pv.get());
epics::pvData::String buffer;
pv->toString(&buffer);
printf("%s\n",buffer.c_str());
        pPVTopPyAddr->pvTop = epics::pvData::PVTopPy::createTop(pv);
printf("pvPyCreateScalar pvTop %p\n",&pPVTopPyAddr->pvTop);
        return static_cast<void *>(pPVTopPyAddr);
    }


    void * pvPyGetTop(void * pvTopPyAddr)
    {
         PVTopPyAddr *p = static_cast<PVTopPyAddr *>(pvTopPyAddr);
         void *ppp = static_cast<void *>(&p->pvTop);
printf("pvPyGetTop pvTop %p\n",&p->pvTop);
         return ppp;
    }

    void pvPyDeleteTop(void *pvTopPyAddr)
    {
         PVTopPyAddr *p = static_cast<PVTopPyAddr *>(pvTopPyAddr);
         delete p;
    }

}

#endif  /* PVDATAPYCREATETEST_H */
