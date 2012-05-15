/* StandardField.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <string>
#include <cstdio>
#include <stdexcept>
#include <epicsThread.h>
#include <epicsExit.h>
#include <pv/lock.h>
#include <pv/pvIntrospect.h>
#include <pv/standardField.h>

using std::tr1::static_pointer_cast;

namespace epics { namespace pvData { 

static String notImplemented("not implemented");
static FieldCreatePtr fieldCreate;
static String valueFieldName("value");

// following are preallocated structures

static StructureConstPtr alarmField;
static StructureConstPtr timeStampField;
static StructureConstPtr displayField;
static StructureConstPtr controlField;
static StructureConstPtr booleanAlarmField;
static StructureConstPtr byteAlarmField;
static StructureConstPtr shortAlarmField;
static StructureConstPtr intAlarmField;
static StructureConstPtr longAlarmField;
static StructureConstPtr floatAlarmField;
static StructureConstPtr doubleAlarmField;
static StructureConstPtr enumeratedAlarmField;


static void createAlarm() {
    size_t num = 3;
    FieldConstPtrArray fields(num);
    StringArray names(num);
    names[0] = "severity";
    names[1] = "status";
    names[2] = "message";
    fields[0] = fieldCreate->createScalar(pvInt);
    fields[1] = fieldCreate->createScalar(pvInt);
    fields[2] = fieldCreate->createScalar(pvString);
    alarmField = fieldCreate->createStructure(names,fields);
}

static void createTimeStamp() {
    size_t num = 3;
    FieldConstPtrArray fields(num);
    StringArray names(num);
    names[0] = "secondsPastEpoch";
    names[1] = "nanoSeconds";
    names[2] = "userTag";
    fields[0] = fieldCreate->createScalar(pvLong);
    fields[1] = fieldCreate->createScalar(pvInt);
    fields[2] = fieldCreate->createScalar(pvInt);
    timeStampField = fieldCreate->createStructure(names,fields);
}

static void createDisplay() {
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
    displayField = fieldCreate->createStructure(names,fields);
}

static void createControl() {
    size_t num = 3;
    FieldConstPtrArray fields(num);
    StringArray names(num);
    names[0] = "limitLow";
    names[1] = "limitHigh";
    names[2] = "minStep";
    fields[0] = fieldCreate->createScalar(pvDouble);
    fields[1] = fieldCreate->createScalar(pvDouble);
    fields[2] = fieldCreate->createScalar(pvDouble);
    controlField = fieldCreate->createStructure(names,fields);
}

static void createBooleanAlarm() {
    size_t num = 4;
    FieldConstPtrArray fields(num);
    StringArray names(num);
    names[0] = "active";
    names[1] = "falseSeverity";
    names[2] = "trueSeverity";
    names[3] = "changeStateSeverity";
    booleanAlarmField = fieldCreate->createStructure(names,fields);
}

static void createByteAlarm() {
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
    names[9] =  "hystersis";
    fields[0] = fieldCreate->createScalar(pvBoolean);
    for(size_t i=0; i<numFields; i++ ) {
        fields[i] = fieldCreate->createScalar(pvByte);
    }
    byteAlarmField = fieldCreate->createStructure(names,fields);
}

static void createShortAlarm() {
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
    names[9] =  "hystersis";
    fields[0] = fieldCreate->createScalar(pvBoolean);
    for(size_t i=0; i<numFields; i++ ) {
        fields[i] = fieldCreate->createScalar(pvShort);
    }
    shortAlarmField = fieldCreate->createStructure(names,fields);
}

static void createIntAlarm() {
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
    names[9] =  "hystersis";
    fields[0] = fieldCreate->createScalar(pvBoolean);
    for(size_t i=0; i<numFields; i++ ) {
        fields[i] = fieldCreate->createScalar(pvInt);
    }
    intAlarmField = fieldCreate->createStructure(names,fields);
}

static void createLongAlarm() {
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
    names[9] =  "hystersis";
    fields[0] = fieldCreate->createScalar(pvBoolean);
    for(size_t i=0; i<numFields; i++ ) {
        fields[i] = fieldCreate->createScalar(pvLong);
    }
    longAlarmField = fieldCreate->createStructure(names,fields);
}

static void createFloatAlarm() {
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
    names[9] =  "hystersis";
    fields[0] = fieldCreate->createScalar(pvBoolean);
    for(size_t i=0; i<numFields; i++ ) {
        fields[i] = fieldCreate->createScalar(pvFloat);
    }
    floatAlarmField = fieldCreate->createStructure(names,fields);
}

static void createDoubleAlarm() {
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
    names[9] =  "hystersis";
    fields[0] = fieldCreate->createScalar(pvDouble);
    for(size_t i=0; i<numFields; i++ ) {
        fields[i] = fieldCreate->createScalar(pvByte);
    }
    doubleAlarmField = fieldCreate->createStructure(names,fields);
}

static void createEnumeratedAlarm() {
    size_t numFields = 3;
    FieldConstPtrArray fields(numFields);
    StringArray names(numFields);
    names[0] = "active";
    names[1] = "stateSeverity";
    names[2] = "changeStateSeverity";
    fields[0] = fieldCreate->createScalar(pvBoolean);
    fields[1] = fieldCreate->createScalar(pvInt);
    fields[2] = fieldCreate->createScalar(pvInt);
    enumeratedAlarmField = fieldCreate->createStructure(names,fields);
}

static StructureConstPtr createProperties(FieldConstPtr field,String properties)
{
    bool gotAlarm = false;
    bool gotTimeStamp = false;
    bool gotDisplay = false;
    bool gotControl = false;
    bool gotValueAlarm = false;
    int numProp = 0;
    if(properties.find("alarm")!=String::npos) { gotAlarm = true; numProp++; }
    if(properties.find("timeStamp")!=String::npos) { gotTimeStamp = true; numProp++; }
    if(properties.find("display")!=String::npos) { gotDisplay = true; numProp++; }
    if(properties.find("control")!=String::npos) { gotControl = true; numProp++; }
    if(properties.find("valueAlarm")!=String::npos) { gotValueAlarm = true; numProp++; }
    StructureConstPtr valueAlarm;
    Type type= field->getType();
    while(gotValueAlarm) {
        if(type==scalar) {
           ScalarConstPtr scalar = static_pointer_cast<const Scalar>(field);
           ScalarType scalarType = scalar->getScalarType();
           switch(scalarType) {
               case pvBoolean: valueAlarm = booleanAlarmField; break;
               case pvByte: valueAlarm = byteAlarmField; break;
               case pvShort: valueAlarm = shortAlarmField; break;
               case pvInt: valueAlarm = intAlarmField; break;
               case pvLong: valueAlarm = longAlarmField; break;
               case pvFloat: valueAlarm = floatAlarmField; break;
               case pvDouble: valueAlarm = doubleAlarmField; break;
               default:
                throw std::logic_error(String("valueAlarm property for illegal type"));
           }
           break;
        }
        if(type==structure) {
            StructureConstPtr structurePtr = static_pointer_cast<const Structure>(field);
            StringArray names = structurePtr->getFieldNames();
            if(names.size()==2) {
                FieldConstPtrArray fields = structurePtr->getFields();
                FieldConstPtr first = fields[0];
                FieldConstPtr second = fields[1];
                String nameFirst = names[0];
                String nameSecond = names[1];
                int compareFirst = nameFirst.compare("index");
                int compareSecond = nameSecond.compare("choices");
                if(compareFirst==0 && compareSecond==0) {
                    if(first->getType()==scalar
                    && second->getType()==scalarArray) {
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
        throw std::logic_error(String("valueAlarm property for illegal type"));
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
    return fieldCreate->createStructure(names,fields);
}


StructureConstPtr StandardField::scalar(
    ScalarType type,String properties)
{
    ScalarConstPtr field = fieldCreate->createScalar(type);
    return createProperties(field,properties);    
}

StructureConstPtr StandardField::scalarArray(
    ScalarType elementType, String properties)
{
    ScalarArrayConstPtr field = fieldCreate->createScalarArray(elementType);
    return createProperties(field,properties);
}


StructureConstPtr StandardField::structureArray(
    StructureConstPtr &structure,String properties)
{
    StructureArrayConstPtr field = fieldCreate->createStructureArray(
        structure);
    return createProperties(field,properties);
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
    return fieldCreate->createStructure(names,fields);
}

StructureConstPtr StandardField::enumerated(String properties)
{
    StructureConstPtr field = enumerated();
    return createProperties(field,properties);
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

StructureConstPtr StandardField::shortAlarm()
{
    return shortAlarmField;
}

StructureConstPtr StandardField::intAlarm()
{
    return intAlarmField;
}

StructureConstPtr StandardField::longAlarm()
{
    return longAlarmField;
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

StandardFieldPtr StandardField::getStandardField()
{
    static StandardFieldPtr standardFieldCreate;
    static Mutex mutex;
    Lock xx(mutex);

    if(standardFieldCreate.get()==0)
    {
        fieldCreate = getFieldCreate();
        createAlarm();
        createTimeStamp();
        createDisplay();
        createControl();
        createBooleanAlarm();
        createByteAlarm();
        createShortAlarm();
        createIntAlarm();
        createLongAlarm();
        createFloatAlarm();
        createDoubleAlarm();
        createEnumeratedAlarm();
        standardFieldCreate = StandardFieldPtr(new StandardField());
    }
    return standardFieldCreate;
}

StandardField::StandardField(){}
StandardField::~StandardField(){}


StandardFieldPtr getStandardField() {
    return StandardField::getStandardField();
}

}}
