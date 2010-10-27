/* StandardPVField.cpp */
#include <string>
#include <stdexcept>
#include <lock.h>
#include "pvIntrospect.h"
#include "pvData.h"
#include "convert.h"
#include "standardField.h"
#include "standardPVField.h"

namespace epics { namespace pvData { 

static String notImplemented("not implemented");
static FieldCreate* fieldCreate = 0;
static PVDataCreate* pvDataCreate = 0;

StandardPVField::StandardPVField(){}

StandardPVField::~StandardPVField(){}


PVScalar * StandardPVField::scalar(String fieldName,ScalarType type)
{
    throw std::logic_error(notImplemented);
}

PVStructure * StandardPVField::scalar(String fieldName,ScalarType type,String properties)
{
    throw std::logic_error(notImplemented);
}

PVScalarArray * StandardPVField::scalarArray(String fieldName,ScalarType elementType)
{
    throw std::logic_error(notImplemented);
}

PVStructure * StandardPVField::scalarArray(String fieldName,ScalarType elementType, String properties)
{
    throw std::logic_error(notImplemented);
}

PVStructureArray * StandardPVField::structureArray(String fieldName,StructureConstPtr structure)
{
    throw std::logic_error(notImplemented);
}

PVStructure * StandardPVField::structureArray(String fieldName,StructureConstPtr structure,String properties)
{
    throw std::logic_error(notImplemented);
}

PVStructure *StandardPVField::structure(String fieldName,PVStructure *pvStructure)
{
    throw std::logic_error(notImplemented);
}

PVStructure *StandardPVField::structure(String fieldName,PVStructure *pvStructure,String properties)
{
    throw std::logic_error(notImplemented);
}

PVStructure * StandardPVField::enumerated(String fieldName,StringArray choices)
{
    throw std::logic_error(notImplemented);
}

PVStructure * StandardPVField::enumerated(String fieldName,StringArray choices, String properties)
{
    throw std::logic_error(notImplemented);
}

PVScalar * StandardPVField::scalarValue(ScalarType scalarType)
{
    ScalarConstPtr scalar = fieldCreate->createScalar(
       String("value"),scalarType);
    return getPVDataCreate()->createPVScalar(0,scalar);
}

PVStructure * StandardPVField::scalarValue(ScalarType type,String properties)
{
    throw std::logic_error(notImplemented);
}

PVScalarArray * StandardPVField::scalarArrayValue(ScalarType elementType)
{
    ScalarArrayConstPtr scalarArray = fieldCreate->createScalarArray(
       String("value"),elementType);
    return pvDataCreate->createPVScalarArray(0,scalarArray);
}

PVStructure * StandardPVField::scalarArrayValue(ScalarType elementType, String properties)
{
    throw std::logic_error(notImplemented);
}

PVStructureArray * StandardPVField::structureArrayValue(StructureConstPtr structure)
{
    throw std::logic_error(notImplemented);
}

PVStructure * StandardPVField::structureArrayValue(StructureConstPtr structure,String properties)
{
    throw std::logic_error(notImplemented);
}

PVStructure *StandardPVField::structureValue(PVStructure *pvStructure)
{
    throw std::logic_error(notImplemented);
}

PVStructure *StandardPVField::structureValue(PVStructure *pvStructure,String properties)
{
    throw std::logic_error(notImplemented);
}

PVStructure * StandardPVField::enumeratedValue(StringArray choices)
{
    throw std::logic_error(notImplemented);
}

PVStructure * StandardPVField::enumeratedValue(StringArray choices, String properties)
{
    throw std::logic_error(notImplemented);
}

PVStructure * StandardPVField::alarm()
{
    throw std::logic_error(notImplemented);
}

PVStructure * StandardPVField::timeStamp()
{
    throw std::logic_error(notImplemented);
}

PVStructure * StandardPVField::display()
{
    throw std::logic_error(notImplemented);
}

PVStructure * StandardPVField::control()
{
    throw std::logic_error(notImplemented);
}

PVStructure * StandardPVField::booleanAlarm()
{
    throw std::logic_error(notImplemented);
}

PVStructure * StandardPVField::byteAlarm()
{
    throw std::logic_error(notImplemented);
}

PVStructure * StandardPVField::shortAlarm()
{
    throw std::logic_error(notImplemented);
}

PVStructure * StandardPVField::intAlarm()
{
    throw std::logic_error(notImplemented);
}

PVStructure * StandardPVField::longAlarm()
{
    throw std::logic_error(notImplemented);
}

PVStructure * StandardPVField::floatAlarm()
{
    throw std::logic_error(notImplemented);
}

PVStructure * StandardPVField::doubleAlarm()
{
    throw std::logic_error(notImplemented);
}

PVStructure * StandardPVField::enumeratedAlarm()
{
    throw std::logic_error(notImplemented);
}

PVStructure * StandardPVField::powerSupply()
{
    throw std::logic_error(notImplemented);
}



static StandardPVField* instance = 0;


class StandardPVFieldExt : public StandardPVField {
public:
    StandardPVFieldExt(): StandardPVField(){};
};

StandardPVField * getStandardPVField() {
    static Mutex mutex = Mutex();
    Lock xx(&mutex);

    if(instance==0) {
        instance = new StandardPVFieldExt();
        fieldCreate = getFieldCreate();
        pvDataCreate = getPVDataCreate();
    }
    return instance;
}

}}
