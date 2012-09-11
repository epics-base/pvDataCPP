/* pvDataCAPICreateTest.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#include <pv/pvDataCAPICreateTest.h>
#include <pv/pvDataCAPI.h>
#include <pv/standardPVField.h>

using namespace epics::pvData;

extern "C"
{
    void * pvCAPICreateScalar(int scalarType,const char *properties)
    {
        StandardPVFieldPtr standardPVField = getStandardPVField();
        String xxx(properties);
        ScalarType yyy = static_cast<ScalarType>(scalarType);
        return PVStructureCAPI::create(standardPVField->scalar(yyy,xxx));
    }

    void * pvCAPICreateScalarArray(int elementType,const char *properties)
    {
        StandardPVFieldPtr standardPVField = getStandardPVField();
        String xxx(properties);
        ScalarType yyy = static_cast<ScalarType>(elementType);
        return PVStructureCAPI::create(standardPVField->scalarArray(yyy,xxx));
    }

}

