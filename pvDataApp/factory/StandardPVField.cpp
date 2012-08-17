/* StandardPVField.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <string>
#include <stdexcept>
#include <epicsThread.h>
#include <epicsExit.h>
#include <pv/lock.h>
#include <pv/pvIntrospect.h>
#include <pv/pvData.h>
#include <pv/convert.h>
#include <pv/standardField.h>
#include <pv/standardPVField.h>

namespace epics { namespace pvData { 

static StandardFieldPtr standardField;
static FieldCreatePtr fieldCreate;
static PVDataCreatePtr pvDataCreate;


static String notImplemented("not implemented");

StandardPVField::StandardPVField(){}

StandardPVField::~StandardPVField(){}

PVStructurePtr StandardPVField::scalar(ScalarType type,String  properties)
{
    StructureConstPtr field = standardField->scalar(type,properties);
    PVStructurePtr pvStructure = pvDataCreate->createPVStructure(field);
    return pvStructure;
}

PVStructurePtr StandardPVField::scalarArray(ScalarType elementType, String  properties)
{
    StructureConstPtr field = standardField->scalarArray(elementType,properties);
    PVStructurePtr pvStructure = pvDataCreate->createPVStructure(field);
    return pvStructure;
}

PVStructurePtr StandardPVField::structureArray(StructureConstPtr const & structure,String  properties)
{
    StructureConstPtr field = standardField->structureArray(structure,properties);
    PVStructurePtr pvStructure = pvDataCreate->createPVStructure(field);
    return pvStructure;
}

PVStructurePtr StandardPVField::enumerated(StringArray const &choices)
{
    StructureConstPtr field = standardField->enumerated();
    PVStructurePtr pvStructure = pvDataCreate->createPVStructure(field);
    PVScalarArrayPtr pvScalarArray = pvStructure->getScalarArrayField(
        "choices",pvString);
    if(pvScalarArray.get()==NULL) {
        throw std::logic_error(String("StandardPVField::enumerated"));
    }
    PVStringArray * pvChoices = static_cast<PVStringArray *>(pvScalarArray.get());
    pvChoices->put(0,choices.size(),get(choices),0);
    return pvStructure;
}

PVStructurePtr StandardPVField::enumerated(StringArray const &choices,String  properties)
{
    StructureConstPtr field = standardField->enumerated(properties);
    PVStructurePtr pvStructure =  pvDataCreate->createPVStructure(field);
    PVScalarArrayPtr pvScalarArray = pvStructure->getScalarArrayField("value.choices",pvString);
    if(pvScalarArray.get()==NULL) {
        throw std::logic_error(String("StandardPVField::enumerated"));
    }
    PVStringArray * pvChoices = static_cast<PVStringArray *>(pvScalarArray.get());
    pvChoices->put(0,choices.size(),get(choices),0);
    return pvStructure;
}

StandardPVFieldPtr StandardPVField::getStandardPVField()
{
    static StandardPVFieldPtr standardPVField;
    static Mutex mutex;
    Lock xx(mutex);

    if(standardPVField.get()==NULL) {
        standardField = getStandardField();
        fieldCreate = getFieldCreate();
        pvDataCreate = getPVDataCreate();
        standardPVField= StandardPVFieldPtr(new StandardPVField());
    }
    return standardPVField;
}

StandardPVFieldPtr getStandardPVField() {
    return StandardPVField::getStandardPVField();
}

}}
