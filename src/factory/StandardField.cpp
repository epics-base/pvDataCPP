/* StandardField.cpp */
/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */
/**
 *  @author mrk
 */
#include <string>
#include <cstdio>
#include <stdexcept>

#include <epicsMutex.h>

#define epicsExportSharedSymbols
#include <pv/lock.h>
#include <pv/pvIntrospect.h>
#include <pv/standardField.h>

using std::tr1::static_pointer_cast;
using std::string;

namespace epics { namespace pvData { 


StandardField::StandardField()
:  fieldCreate(getFieldCreate()),
   notImplemented("not implemented"),
   valueFieldName("value")
{}

void StandardField::init()
{
    createAlarm();
    createTimeStamp();
    createDisplay();
    createControl();
    createBooleanAlarm();
    createByteAlarm();
    createShortAlarm();
    createIntAlarm();
    createLongAlarm();
    createUByteAlarm();
    createUShortAlarm();
    createUIntAlarm();
    createULongAlarm();
    createFloatAlarm();
    createDoubleAlarm();
    createEnumeratedAlarm();
}

StandardField::~StandardField(){}

StructureConstPtr StandardField::createProperties(string id,FieldConstPtr field,string properties)
{
    bool gotAlarm = false;
    bool gotTimeStamp = false;
    bool gotDisplay = false;
    bool gotControl = false;
    bool gotValueAlarm = false;
    int numProp = 0;
    if(properties.find("alarm")!=string::npos) { gotAlarm = true; numProp++; }
    if(properties.find("timeStamp")!=string::npos) { gotTimeStamp = true; numProp++; }
    if(properties.find("display")!=string::npos) { gotDisplay = true; numProp++; }
    if(properties.find("control")!=string::npos) { gotControl = true; numProp++; }
    if(properties.find("valueAlarm")!=string::npos) { gotValueAlarm = true; numProp++; }
    StructureConstPtr valueAlarm;
    Type type= field->getType();
    while(gotValueAlarm) {
        if(type==epics::pvData::scalar || type==epics::pvData::scalarArray) {
           ScalarType scalarType = (type==epics::pvData::scalar) ?
		static_pointer_cast<const Scalar>(field)->getScalarType() :
		static_pointer_cast<const ScalarArray>(field)->getElementType();
           switch(scalarType) {
               case pvBoolean: valueAlarm = booleanAlarmField; break;
               case pvByte: valueAlarm = byteAlarmField; break;
               case pvShort: valueAlarm = shortAlarmField; break;
               case pvInt: valueAlarm = intAlarmField; break;
               case pvLong: valueAlarm = longAlarmField; break;
               case pvUByte: valueAlarm = ubyteAlarmField; break;
               case pvUShort: valueAlarm = ushortAlarmField; break;
               case pvUInt: valueAlarm = uintAlarmField; break;
               case pvULong: valueAlarm = ulongAlarmField; break;
               case pvFloat: valueAlarm = floatAlarmField; break;
               case pvDouble: valueAlarm = doubleAlarmField; break;
               case pvString:
                throw std::logic_error(string("valueAlarm property not supported for pvString"));
           }
           break;
        }
        if(type==structure) {
            StructureConstPtr structurePtr = static_pointer_cast<const Structure>(field);
            StringArray const & names = structurePtr->getFieldNames();
            if(names.size()==2) {
                FieldConstPtrArray const & fields = structurePtr->getFields();
                FieldConstPtr first = fields[0];
                FieldConstPtr second = fields[1];
                string nameFirst = names[0];
                string nameSecond = names[1];
                int compareFirst = nameFirst.compare("index");
                int compareSecond = nameSecond.compare("choices");
                if(compareFirst==0 && compareSecond==0) {
                    if(first->getType()==epics::pvData::scalar
                    && second->getType()==epics::pvData::scalarArray) {
                        ScalarConstPtr scalarFirst = static_pointer_cast<const Scalar>(first);
                        ScalarArrayConstPtr scalarArraySecond = 
                            static_pointer_cast<const ScalarArray>(second);
                        if(scalarFirst->getScalarType()==pvInt
                        && scalarArraySecond->getElementType()==pvString) {
                            valueAlarm = enumeratedAlarmField;
                            break;
                        }
                    }
                }
            }
        }
        throw std::logic_error(string("valueAlarm property for illegal type"));
    }
    size_t numFields = numProp+1;
    FieldConstPtrArray fields(numFields);
    StringArray names(numFields);
    int next = 0;
    names[0] = "value";
    fields[next++] = field;
    if(gotAlarm) {
        names[next] = "alarm";
        fields[next++] = alarmField;
    }
    if(gotTimeStamp) {
        names[next] = "timeStamp";
        fields[next++] = timeStampField;
    }
    if(gotDisplay) {
        names[next] = "display";
        fields[next++] = displayField;
    }
    if(gotControl) {
        names[next] = "control";
        fields[next++] = controlField;
    }
    if(gotValueAlarm) {
        names[next] = "valueAlarm";
        fields[next++] = valueAlarm;
    }
    return fieldCreate->createStructure(id,names,fields);
}

void StandardField::createAlarm() {
    size_t num = 3;
    FieldConstPtrArray fields(num);
    StringArray names(num);
    names[0] = "severity";
    names[1] = "status";
    names[2] = "message";
    fields[0] = fieldCreate->createScalar(pvInt);
    fields[1] = fieldCreate->createScalar(pvInt);
    fields[2] = fieldCreate->createScalar(pvString);
    alarmField = fieldCreate->createStructure("alarm_t",names,fields);
}

void StandardField::createTimeStamp() {
    size_t num = 3;
    FieldConstPtrArray fields(num);
    StringArray names(num);
    names[0] = "secondsPastEpoch";
    names[1] = "nanoseconds";
    names[2] = "userTag";
    fields[0] = fieldCreate->createScalar(pvLong);
    fields[1] = fieldCreate->createScalar(pvInt);
    fields[2] = fieldCreate->createScalar(pvInt);
    timeStampField = fieldCreate->createStructure("time_t",names,fields);
}

void StandardField::createDisplay() {
    size_t num = 5;
    FieldConstPtrArray fields(num);
    StringArray names(num);
    names[0] = "limitLow";
    names[1] = "limitHigh";
    names[2] = "description";
    names[3] = "format";
    names[4] = "units";
    fields[0] = fieldCreate->createScalar(pvDouble);
    fields[1] = fieldCreate->createScalar(pvDouble);
    fields[2] = fieldCreate->createScalar(pvString);
    fields[3] = fieldCreate->createScalar(pvString);
    fields[4] = fieldCreate->createScalar(pvString);
    displayField = fieldCreate->createStructure("display_t",names,fields);
}

void StandardField::createControl() {
    size_t num = 3;
    FieldConstPtrArray fields(num);
    StringArray names(num);
    names[0] = "limitLow";
    names[1] = "limitHigh";
    names[2] = "minStep";
    fields[0] = fieldCreate->createScalar(pvDouble);
    fields[1] = fieldCreate->createScalar(pvDouble);
    fields[2] = fieldCreate->createScalar(pvDouble);
    controlField = fieldCreate->createStructure("control_t",names,fields);
}

void StandardField::createBooleanAlarm() {
    size_t numFields = 4;
    FieldConstPtrArray fields(numFields);
    StringArray names(numFields);
    names[0] = "active";
    names[1] = "falseSeverity";
    names[2] = "trueSeverity";
    names[3] = "changeStateSeverity";
    fields[0] = fieldCreate->createScalar(pvBoolean);
    fields[1] = fieldCreate->createScalar(pvInt);
    fields[2] = fieldCreate->createScalar(pvInt);
    fields[3] = fieldCreate->createScalar(pvInt);
    booleanAlarmField = fieldCreate->createStructure("valueAlarm_t",names,fields);
}

void StandardField::createByteAlarm() {
    size_t numFields = 10;
    FieldConstPtrArray fields(numFields);
    StringArray names(numFields);
    names[0] = "active";
    names[1] = "lowAlarmLimit";
    names[2] = "lowWarningLimit";
    names[3] = "highWarningLimit";
    names[4] = "highAlarmLimit";
    names[5] = "lowAlarmSeverity";
    names[6] = "lowWarningSeverity";
    names[7] = "highWarningSeverity";
    names[8] = "highAlarmSeverity";
    names[9] =  "hysteresis";
    fields[0] = fieldCreate->createScalar(pvBoolean);
    fields[1] = fieldCreate->createScalar(pvByte);
    fields[2] = fieldCreate->createScalar(pvByte);
    fields[3] = fieldCreate->createScalar(pvByte);
    fields[4] = fieldCreate->createScalar(pvByte);
    fields[5] = fieldCreate->createScalar(pvInt);
    fields[6] = fieldCreate->createScalar(pvInt);
    fields[7] = fieldCreate->createScalar(pvInt);
    fields[8] = fieldCreate->createScalar(pvInt);
    fields[9] = fieldCreate->createScalar(pvByte);
    byteAlarmField = fieldCreate->createStructure("valueAlarm_t",names,fields);
}

void StandardField::createShortAlarm() {
    size_t numFields = 10;
    FieldConstPtrArray fields(numFields);
    StringArray names(numFields);
    names[0] = "active";
    names[1] = "lowAlarmLimit";
    names[2] = "lowWarningLimit";
    names[3] = "highWarningLimit";
    names[4] = "highAlarmLimit";
    names[5] = "lowAlarmSeverity";
    names[6] = "lowWarningSeverity";
    names[7] = "highWarningSeverity";
    names[8] = "highAlarmSeverity";
    names[9] =  "hysteresis";
    fields[0] = fieldCreate->createScalar(pvBoolean);
    fields[1] = fieldCreate->createScalar(pvShort);
    fields[2] = fieldCreate->createScalar(pvShort);
    fields[3] = fieldCreate->createScalar(pvShort);
    fields[4] = fieldCreate->createScalar(pvShort);
    fields[5] = fieldCreate->createScalar(pvInt);
    fields[6] = fieldCreate->createScalar(pvInt);
    fields[7] = fieldCreate->createScalar(pvInt);
    fields[8] = fieldCreate->createScalar(pvInt);
    fields[9] = fieldCreate->createScalar(pvShort);
    shortAlarmField = fieldCreate->createStructure("valueAlarm_t",names,fields);
}

void StandardField::createIntAlarm() {
    size_t numFields = 10;
    FieldConstPtrArray fields(numFields);
    StringArray names(numFields);
    names[0] = "active";
    names[1] = "lowAlarmLimit";
    names[2] = "lowWarningLimit";
    names[3] = "highWarningLimit";
    names[4] = "highAlarmLimit";
    names[5] = "lowAlarmSeverity";
    names[6] = "lowWarningSeverity";
    names[7] = "highWarningSeverity";
    names[8] = "highAlarmSeverity";
    names[9] =  "hysteresis";
    fields[0] = fieldCreate->createScalar(pvBoolean);
    fields[1] = fieldCreate->createScalar(pvInt);
    fields[2] = fieldCreate->createScalar(pvInt);
    fields[3] = fieldCreate->createScalar(pvInt);
    fields[4] = fieldCreate->createScalar(pvInt);
    fields[5] = fieldCreate->createScalar(pvInt);
    fields[6] = fieldCreate->createScalar(pvInt);
    fields[7] = fieldCreate->createScalar(pvInt);
    fields[8] = fieldCreate->createScalar(pvInt);
    fields[9] = fieldCreate->createScalar(pvInt);
    intAlarmField = fieldCreate->createStructure("valueAlarm_t",names,fields);
}

void StandardField::createLongAlarm() {
    size_t numFields = 10;
    FieldConstPtrArray fields(numFields);
    StringArray names(numFields);
    names[0] = "active";
    names[1] = "lowAlarmLimit";
    names[2] = "lowWarningLimit";
    names[3] = "highWarningLimit";
    names[4] = "highAlarmLimit";
    names[5] = "lowAlarmSeverity";
    names[6] = "lowWarningSeverity";
    names[7] = "highWarningSeverity";
    names[8] = "highAlarmSeverity";
    names[9] =  "hysteresis";
    fields[0] = fieldCreate->createScalar(pvBoolean);
    fields[1] = fieldCreate->createScalar(pvLong);
    fields[2] = fieldCreate->createScalar(pvLong);
    fields[3] = fieldCreate->createScalar(pvLong);
    fields[4] = fieldCreate->createScalar(pvLong);
    fields[5] = fieldCreate->createScalar(pvInt);
    fields[6] = fieldCreate->createScalar(pvInt);
    fields[7] = fieldCreate->createScalar(pvInt);
    fields[8] = fieldCreate->createScalar(pvInt);
    fields[9] = fieldCreate->createScalar(pvLong);
    longAlarmField = fieldCreate->createStructure("valueAlarm_t",names,fields);
}

void StandardField::createUByteAlarm() {
    size_t numFields = 10;
    FieldConstPtrArray fields(numFields);
    StringArray names(numFields);
    names[0] = "active";
    names[1] = "lowAlarmLimit";
    names[2] = "lowWarningLimit";
    names[3] = "highWarningLimit";
    names[4] = "highAlarmLimit";
    names[5] = "lowAlarmSeverity";
    names[6] = "lowWarningSeverity";
    names[7] = "highWarningSeverity";
    names[8] = "highAlarmSeverity";
    names[9] =  "hysteresis";
    fields[0] = fieldCreate->createScalar(pvBoolean);
    fields[1] = fieldCreate->createScalar(pvUByte);
    fields[2] = fieldCreate->createScalar(pvUByte);
    fields[3] = fieldCreate->createScalar(pvUByte);
    fields[4] = fieldCreate->createScalar(pvUByte);
    fields[5] = fieldCreate->createScalar(pvInt);
    fields[6] = fieldCreate->createScalar(pvInt);
    fields[7] = fieldCreate->createScalar(pvInt);
    fields[8] = fieldCreate->createScalar(pvInt);
    fields[9] = fieldCreate->createScalar(pvUByte);
    ubyteAlarmField = fieldCreate->createStructure("valueAlarm_t",names,fields);
}

void StandardField::createUShortAlarm() {
    size_t numFields = 10;
    FieldConstPtrArray fields(numFields);
    StringArray names(numFields);
    names[0] = "active";
    names[1] = "lowAlarmLimit";
    names[2] = "lowWarningLimit";
    names[3] = "highWarningLimit";
    names[4] = "highAlarmLimit";
    names[5] = "lowAlarmSeverity";
    names[6] = "lowWarningSeverity";
    names[7] = "highWarningSeverity";
    names[8] = "highAlarmSeverity";
    names[9] =  "hysteresis";
    fields[0] = fieldCreate->createScalar(pvBoolean);
    fields[1] = fieldCreate->createScalar(pvUShort);
    fields[2] = fieldCreate->createScalar(pvUShort);
    fields[3] = fieldCreate->createScalar(pvUShort);
    fields[4] = fieldCreate->createScalar(pvUShort);
    fields[5] = fieldCreate->createScalar(pvInt);
    fields[6] = fieldCreate->createScalar(pvInt);
    fields[7] = fieldCreate->createScalar(pvInt);
    fields[8] = fieldCreate->createScalar(pvInt);
    fields[9] = fieldCreate->createScalar(pvUShort);
    ushortAlarmField = fieldCreate->createStructure("valueAlarm_t",names,fields);
}

void StandardField::createUIntAlarm() {
    size_t numFields = 10;
    FieldConstPtrArray fields(numFields);
    StringArray names(numFields);
    names[0] = "active";
    names[1] = "lowAlarmLimit";
    names[2] = "lowWarningLimit";
    names[3] = "highWarningLimit";
    names[4] = "highAlarmLimit";
    names[5] = "lowAlarmSeverity";
    names[6] = "lowWarningSeverity";
    names[7] = "highWarningSeverity";
    names[8] = "highAlarmSeverity";
    names[9] =  "hysteresis";
    fields[0] = fieldCreate->createScalar(pvBoolean);
    fields[1] = fieldCreate->createScalar(pvUInt);
    fields[2] = fieldCreate->createScalar(pvUInt);
    fields[3] = fieldCreate->createScalar(pvUInt);
    fields[4] = fieldCreate->createScalar(pvUInt);
    fields[5] = fieldCreate->createScalar(pvInt);
    fields[6] = fieldCreate->createScalar(pvInt);
    fields[7] = fieldCreate->createScalar(pvInt);
    fields[8] = fieldCreate->createScalar(pvInt);
    fields[9] = fieldCreate->createScalar(pvUInt);
    uintAlarmField = fieldCreate->createStructure("valueAlarm_t",names,fields);
}

void StandardField::createULongAlarm() {
    size_t numFields = 10;
    FieldConstPtrArray fields(numFields);
    StringArray names(numFields);
    names[0] = "active";
    names[1] = "lowAlarmLimit";
    names[2] = "lowWarningLimit";
    names[3] = "highWarningLimit";
    names[4] = "highAlarmLimit";
    names[5] = "lowAlarmSeverity";
    names[6] = "lowWarningSeverity";
    names[7] = "highWarningSeverity";
    names[8] = "highAlarmSeverity";
    names[9] =  "hysteresis";
    fields[0] = fieldCreate->createScalar(pvBoolean);
    fields[1] = fieldCreate->createScalar(pvULong);
    fields[2] = fieldCreate->createScalar(pvULong);
    fields[3] = fieldCreate->createScalar(pvULong);
    fields[4] = fieldCreate->createScalar(pvULong);
    fields[5] = fieldCreate->createScalar(pvInt);
    fields[6] = fieldCreate->createScalar(pvInt);
    fields[7] = fieldCreate->createScalar(pvInt);
    fields[8] = fieldCreate->createScalar(pvInt);
    fields[9] = fieldCreate->createScalar(pvULong);
    ulongAlarmField = fieldCreate->createStructure("valueAlarm_t",names,fields);
}

void StandardField::createFloatAlarm() {
    size_t numFields = 10;
    FieldConstPtrArray fields(numFields);
    StringArray names(numFields);
    names[0] = "active";
    names[1] = "lowAlarmLimit";
    names[2] = "lowWarningLimit";
    names[3] = "highWarningLimit";
    names[4] = "highAlarmLimit";
    names[5] = "lowAlarmSeverity";
    names[6] = "lowWarningSeverity";
    names[7] = "highWarningSeverity";
    names[8] = "highAlarmSeverity";
    names[9] =  "hysteresis";
    fields[0] = fieldCreate->createScalar(pvBoolean);
    fields[1] = fieldCreate->createScalar(pvFloat);
    fields[2] = fieldCreate->createScalar(pvFloat);
    fields[3] = fieldCreate->createScalar(pvFloat);
    fields[4] = fieldCreate->createScalar(pvFloat);
    fields[5] = fieldCreate->createScalar(pvInt);
    fields[6] = fieldCreate->createScalar(pvInt);
    fields[7] = fieldCreate->createScalar(pvInt);
    fields[8] = fieldCreate->createScalar(pvInt);
    fields[9] = fieldCreate->createScalar(pvFloat);
    floatAlarmField = fieldCreate->createStructure("valueAlarm_t",names,fields);
}

void StandardField::createDoubleAlarm() {
    size_t numFields = 10;
    FieldConstPtrArray fields(numFields);
    StringArray names(numFields);
    names[0] = "active";
    names[1] = "lowAlarmLimit";
    names[2] = "lowWarningLimit";
    names[3] = "highWarningLimit";
    names[4] = "highAlarmLimit";
    names[5] = "lowAlarmSeverity";
    names[6] = "lowWarningSeverity";
    names[7] = "highWarningSeverity";
    names[8] = "highAlarmSeverity";
    names[9] =  "hysteresis";
    fields[0] = fieldCreate->createScalar(pvBoolean);
    fields[1] = fieldCreate->createScalar(pvDouble);
    fields[2] = fieldCreate->createScalar(pvDouble);
    fields[3] = fieldCreate->createScalar(pvDouble);
    fields[4] = fieldCreate->createScalar(pvDouble);
    fields[5] = fieldCreate->createScalar(pvInt);
    fields[6] = fieldCreate->createScalar(pvInt);
    fields[7] = fieldCreate->createScalar(pvInt);
    fields[8] = fieldCreate->createScalar(pvInt);
    fields[9] = fieldCreate->createScalar(pvDouble);
    doubleAlarmField = fieldCreate->createStructure("valueAlarm_t",names,fields);
}

void StandardField::createEnumeratedAlarm() {
    size_t numFields = 3;
    FieldConstPtrArray fields(numFields);
    StringArray names(numFields);
    names[0] = "active";
    names[1] = "stateSeverity";
    names[2] = "changeStateSeverity";
    fields[0] = fieldCreate->createScalar(pvBoolean);
    fields[1] = fieldCreate->createScalarArray(pvInt);
    fields[2] = fieldCreate->createScalar(pvInt);
    enumeratedAlarmField = fieldCreate->createStructure("valueAlarm_t",names,fields);
}


StructureConstPtr StandardField::scalar(
    ScalarType type,string  const &properties)
{
    ScalarConstPtr field = fieldCreate->createScalar(type); // scalar_t
    return createProperties("epics:nt/NTScalar:1.0",field,properties);
}

StructureConstPtr StandardField::regUnion(
    UnionConstPtr const &field,
        string const & properties)
{
   return createProperties("epics:nt/NTUnion:1.0",field,properties);
}

StructureConstPtr StandardField::variantUnion(
    string const & properties)
{
    UnionConstPtr field =  fieldCreate->createVariantUnion();
    return createProperties("epics:nt/NTUnion:1.0",field,properties);
}

StructureConstPtr StandardField::scalarArray(
    ScalarType elementType, string  const &properties)
{
    ScalarArrayConstPtr field = fieldCreate->createScalarArray(elementType); // scalar_t[]
    return createProperties("epics:nt/NTScalarArray:1.0",field,properties);
}


StructureConstPtr StandardField::structureArray(
    StructureConstPtr const & structure,string  const &properties)
{
    StructureArrayConstPtr field = fieldCreate->createStructureArray(
        structure);
    return createProperties("epics:nt/NTStructureArray:1.0",field,properties);
}

StructureConstPtr StandardField::unionArray(
    UnionConstPtr const & punion,string  const &properties)
{
    UnionArrayConstPtr field = fieldCreate->createUnionArray(
        punion);
    return createProperties("epics:nt/NTUnionArray:1.0",field,properties);
}

StructureConstPtr StandardField::enumerated()
{
    size_t num = 2;
    FieldConstPtrArray fields(num);
    StringArray names(num);
    names[0] = "index";
    names[1] = "choices";
    fields[0] = fieldCreate->createScalar(pvInt);
    fields[1] = fieldCreate->createScalarArray(pvString);
    return fieldCreate->createStructure("enum_t",names,fields);
    // NOTE: if this method is used to get NTEnum without properties the ID will be wrong!
}

StructureConstPtr StandardField::enumerated(string  const &properties)
{
    StructureConstPtr field = enumerated(); // enum_t
    return createProperties("epics:nt/NTEnum:1.0",field,properties);
}

StructureConstPtr StandardField::alarm()
{
    return alarmField;
}

StructureConstPtr StandardField::timeStamp()
{
    return timeStampField;
}

StructureConstPtr StandardField::display()
{
    return displayField;
}

StructureConstPtr StandardField::control()
{
    return controlField;
}

StructureConstPtr StandardField::booleanAlarm()
{
    return booleanAlarmField;
}

StructureConstPtr StandardField::byteAlarm()
{
    return byteAlarmField;
}

StructureConstPtr StandardField::ubyteAlarm()
{
    return ubyteAlarmField;
}

StructureConstPtr StandardField::shortAlarm()
{
    return shortAlarmField;
}

StructureConstPtr StandardField::ushortAlarm()
{
    return ushortAlarmField;
}

StructureConstPtr StandardField::intAlarm()
{
    return intAlarmField;
}

StructureConstPtr StandardField::uintAlarm()
{
    return uintAlarmField;
}

StructureConstPtr StandardField::longAlarm()
{
    return longAlarmField;
}

StructureConstPtr StandardField::ulongAlarm()
{
    return ulongAlarmField;
}

StructureConstPtr StandardField::floatAlarm()
{
    return floatAlarmField;
}

StructureConstPtr StandardField::doubleAlarm()
{
    return doubleAlarmField;
}

StructureConstPtr StandardField::enumeratedAlarm()
{
    return enumeratedAlarmField;
}

const StandardFieldPtr &StandardField::getStandardField()
{
    static StandardFieldPtr standardFieldCreate;
    static Mutex mutex;
    Lock xx(mutex);

    if(standardFieldCreate.get()==0)
    {
        standardFieldCreate = StandardFieldPtr(new StandardField());
        standardFieldCreate->init();
    }
    return standardFieldCreate;
}

}}
