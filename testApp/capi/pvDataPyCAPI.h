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
    void * pvPyGetPVStructurePyPtr(void * pvTopPy)
    {
         epics::pvData::PVTopPyPtr * p
              = static_cast<epics::pvData::PVTopPyPtr *>(pvTopPy);
         epics::pvData::PVTopPy *pp = p->get();
         const void * ppp =  &(pp->getPVStructurePy());
         return const_cast<void *>(ppp);
    }

    void pvPyDumpPVStructurePy(void *pvStructurePy)
    {
         epics::pvData::PVStructurePyPtr *p
              = static_cast<epics::pvData::PVStructurePyPtr *>(pvStructurePy);
         epics::pvData::PVStructurePy *pp = p->get();
         epics::pvData::PVStructurePtr const & pvStructurePtr
              = pp->getPVStructurePtr();
printf("pvPyDumpPVStructurePy pvStructure %p\n",pvStructurePtr.get());
         epics::pvData::String buffer;
         pvStructurePtr->toString(&buffer);
         printf("%s\n",buffer.c_str());
    }

}

    
#endif  /* PVDATAPYCAPI_H */
