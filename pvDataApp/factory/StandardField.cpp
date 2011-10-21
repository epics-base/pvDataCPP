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
#include <pv/CDRMonitor.h>

using std::tr1::static_pointer_cast;

namespace epics { namespace pvData { 

static StandardField* standardField = 0;

static String notImplemented("not implemented");
static FieldCreate* fieldCreate = 0;
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
    FieldConstPtrArray fields = new FieldConstPtr[3];
    fields[0] = fieldCreate->createScalar(String("severity"),pvInt);
    fields[1] = fieldCreate->createScalar(String("status"),pvInt);
    fields[2] = fieldCreate->createScalar(String("message"),pvString);
    alarmField = fieldCreate->createStructure(String("alarm"),3,fields);
}

static void createTimeStamp() {
    FieldConstPtrArray fields = new FieldConstPtr[3];
    fields[0] = fieldCreate->createScalar(String("secondsPastEpoch"),pvLong);
    fields[1] = fieldCreate->createScalar(String("nanoSeconds"),pvInt);
    fields[2] = fieldCreate->createScalar(String("userTag"),pvInt);
    timeStampField = fieldCreate->createStructure(String("timeStamp"),3,fields);
}

static void createDisplay() {
    FieldConstPtrArray limitFields = new FieldConstPtr[2];
    limitFields[0] = fieldCreate->createScalar(String("low"),pvDouble);
    limitFields[1] = fieldCreate->createScalar(String("high"),pvDouble);
    FieldConstPtrArray fields = new FieldConstPtr[4];
    fields[0] = fieldCreate->createScalar(String("description"),pvString);
    fields[1] = fieldCreate->createScalar(String("format"),pvString);
    fields[2] = fieldCreate->createScalar(String("units"),pvString);
    fields[3] = fieldCreate->createStructure(String("limit"),2,limitFields);
    displayField = fieldCreate->createStructure(String("display"),4,fields);
}

static void createControl() {
    FieldConstPtrArray limitFields = new FieldConstPtr[2];
    limitFields[0] = fieldCreate->createScalar(String("low"),pvDouble);
    limitFields[1] = fieldCreate->createScalar(String("high"),pvDouble);
    FieldConstPtrArray fields = new FieldConstPtr[2];
    fields[0] = fieldCreate->createStructure(String("limit"),2,limitFields);
    fields[1] = fieldCreate->createScalar(String("minStep"),pvDouble);
    controlField = fieldCreate->createStructure(String("control"),2,fields);
}

static void createBooleanAlarm() {
    FieldConstPtrArray fields = new FieldConstPtr[4];
    fields[0] = fieldCreate->createScalar(String("active"),pvBoolean);
    fields[1] = fieldCreate->createScalar(String("falseSeverity"),pvInt);
    fields[2] = fieldCreate->createScalar(String("trueSeverity"),pvInt);
    fields[3] = fieldCreate->createScalar(String("changeStateSeverity"),pvInt);
    booleanAlarmField = fieldCreate->createStructure(String("valueAlarm"),4,fields);
}

static void createByteAlarm() {
    int numFields = 10;
    FieldConstPtrArray fields = new FieldConstPtr[numFields];
    fields[0] = fieldCreate->createScalar(String("active"),pvBoolean);
    fields[1] = fieldCreate->createScalar(String("lowAlarmLimit"),pvByte);
    fields[2] = fieldCreate->createScalar(String("lowWarningLimit"),pvByte);
    fields[3] = fieldCreate->createScalar(String("highWarningLimit"),pvByte);
    fields[4] = fieldCreate->createScalar(String("highAlarmLimit"),pvByte);
    fields[5] = fieldCreate->createScalar(String("lowAlarmSeverity"),pvInt);
    fields[6] = fieldCreate->createScalar(String("lowWarningSeverity"),pvInt);
    fields[7] = fieldCreate->createScalar(String("highWarningSeverity"),pvInt);
    fields[8] = fieldCreate->createScalar(String("highAlarmSeverity"),pvInt);
    fields[9] = fieldCreate->createScalar(String("hystersis"),pvByte);
    byteAlarmField = fieldCreate->createStructure(String("valueAlarm"),numFields,fields);
}

static void createShortAlarm() {
    int numFields = 10;
    FieldConstPtrArray fields = new FieldConstPtr[numFields];
    fields[0] = fieldCreate->createScalar(String("active"),pvBoolean);
    fields[1] = fieldCreate->createScalar(String("lowAlarmLimit"),pvShort);
    fields[2] = fieldCreate->createScalar(String("lowWarningLimit"),pvShort);
    fields[3] = fieldCreate->createScalar(String("highWarningLimit"),pvShort);
    fields[4] = fieldCreate->createScalar(String("highAlarmLimit"),pvShort);
    fields[5] = fieldCreate->createScalar(String("lowAlarmSeverity"),pvInt);
    fields[6] = fieldCreate->createScalar(String("lowWarningSeverity"),pvInt);
    fields[7] = fieldCreate->createScalar(String("highWarningSeverity"),pvInt);
    fields[8] = fieldCreate->createScalar(String("highAlarmSeverity"),pvInt);
    fields[9] = fieldCreate->createScalar(String("hystersis"),pvShort);
    shortAlarmField = fieldCreate->createStructure(String("valueAlarm"),numFields,fields);
}

static void createIntAlarm() {
    int numFields = 10;
    FieldConstPtrArray fields = new FieldConstPtr[numFields];
    fields[0] = fieldCreate->createScalar(String("active"),pvBoolean);
    fields[1] = fieldCreate->createScalar(String("lowAlarmLimit"),pvInt);
    fields[2] = fieldCreate->createScalar(String("lowWarningLimit"),pvInt);
    fields[3] = fieldCreate->createScalar(String("highWarningLimit"),pvInt);
    fields[4] = fieldCreate->createScalar(String("highAlarmLimit"),pvInt);
    fields[5] = fieldCreate->createScalar(String("lowAlarmSeverity"),pvInt);
    fields[6] = fieldCreate->createScalar(String("lowWarningSeverity"),pvInt);
    fields[7] = fieldCreate->createScalar(String("highWarningSeverity"),pvInt);
    fields[8] = fieldCreate->createScalar(String("highAlarmSeverity"),pvInt);
    fields[9] = fieldCreate->createScalar(String("hystersis"),pvInt);
    intAlarmField = fieldCreate->createStructure(String("valueAlarm"),numFields,fields);
}

static void createLongAlarm() {
    int numFields = 10;
    FieldConstPtrArray fields = new FieldConstPtr[numFields];
    fields[0] = fieldCreate->createScalar(String("active"),pvBoolean);
    fields[1] = fieldCreate->createScalar(String("lowAlarmLimit"),pvLong);
    fields[2] = fieldCreate->createScalar(String("lowWarningLimit"),pvLong);
    fields[3] = fieldCreate->createScalar(String("highWarningLimit"),pvLong);
    fields[4] = fieldCreate->createScalar(String("highAlarmLimit"),pvLong);
    fields[5] = fieldCreate->createScalar(String("lowAlarmSeverity"),pvInt);
    fields[6] = fieldCreate->createScalar(String("lowWarningSeverity"),pvInt);
    fields[7] = fieldCreate->createScalar(String("highWarningSeverity"),pvInt);
    fields[8] = fieldCreate->createScalar(String("highAlarmSeverity"),pvInt);
    fields[9] = fieldCreate->createScalar(String("hystersis"),pvLong);
    longAlarmField = fieldCreate->createStructure(String("valueAlarm"),numFields,fields);
}

static void createFloatAlarm() {
    int numFields = 10;
    FieldConstPtrArray fields = new FieldConstPtr[numFields];
    fields[0] = fieldCreate->createScalar(String("active"),pvBoolean);
    fields[1] = fieldCreate->createScalar(String("lowAlarmLimit"),pvFloat);
    fields[2] = fieldCreate->createScalar(String("lowWarningLimit"),pvFloat);
    fields[3] = fieldCreate->createScalar(String("highWarningLimit"),pvFloat);
    fields[4] = fieldCreate->createScalar(String("highAlarmLimit"),pvFloat);
    fields[5] = fieldCreate->createScalar(String("lowAlarmSeverity"),pvInt);
    fields[6] = fieldCreate->createScalar(String("lowWarningSeverity"),pvInt);
    fields[7] = fieldCreate->createScalar(String("highWarningSeverity"),pvInt);
    fields[8] = fieldCreate->createScalar(String("highAlarmSeverity"),pvInt);
    fields[9] = fieldCreate->createScalar(String("hystersis"),pvFloat);
    floatAlarmField = fieldCreate->createStructure(String("valueAlarm"),numFields,fields);
}

static void createDoubleAlarm() {
    int numFields = 10;
    FieldConstPtrArray fields = new FieldConstPtr[numFields];
    fields[0] = fieldCreate->createScalar(String("active"),pvBoolean);
    fields[1] = fieldCreate->createScalar(String("lowAlarmLimit"),pvDouble);
    fields[2] = fieldCreate->createScalar(String("lowWarningLimit"),pvDouble);
    fields[3] = fieldCreate->createScalar(String("highWarningLimit"),pvDouble);
    fields[4] = fieldCreate->createScalar(String("highAlarmLimit"),pvDouble);
    fields[5] = fieldCreate->createScalar(String("lowAlarmSeverity"),pvInt);
    fields[6] = fieldCreate->createScalar(String("lowWarningSeverity"),pvInt);
    fields[7] = fieldCreate->createScalar(String("highWarningSeverity"),pvInt);
    fields[8] = fieldCreate->createScalar(String("highAlarmSeverity"),pvInt);
    fields[9] = fieldCreate->createScalar(String("hystersis"),pvDouble);
    doubleAlarmField = fieldCreate->createStructure(String("valueAlarm"),numFields,fields);
}

static void createEnumeratedAlarm() {
    int numFields = 3;
    FieldConstPtrArray fields = new FieldConstPtr[numFields];
    fields[0] = fieldCreate->createScalar(String("active"),pvBoolean);
    fields[1] = fieldCreate->createScalar(String("stateSeverity"),pvInt);
    fields[2] = fieldCreate->createScalar(String("changeStateSeverity"),pvInt);
    enumeratedAlarmField = fieldCreate->createStructure(String("valueAlarm"),numFields,fields);
}

static StructureConstPtr createProperties(String fieldName,FieldConstPtr field,String properties) {
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
            if(structurePtr->getNumberFields()==2) {
                FieldConstPtrArray fields = structurePtr->getFields();
                FieldConstPtr first = fields[0];
                FieldConstPtr second = fields[1];
                String nameFirst = first->getFieldName();
                String nameSecond = second->getFieldName();
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
    int numFields = numProp+1;
    FieldConstPtrArray fields = new FieldConstPtr[numFields];
    int next = 0;
    fields[next++] = field;
    if(gotAlarm) {fields[next++] = alarmField;}
    if(gotTimeStamp) {fields[next++] = timeStampField;}
    if(gotDisplay) {fields[next++] = displayField;}
    if(gotControl) {fields[next++] = controlField;}
    if(gotValueAlarm) {fields[next++] = valueAlarm;}
    return fieldCreate->createStructure(fieldName,numFields,fields);
}


ScalarConstPtr StandardField::scalar(String fieldName,ScalarType type)
{
    return fieldCreate->createScalar(fieldName,type);
}

StructureConstPtr StandardField::scalar(
    String fieldName,ScalarType type,String properties)
{
    ScalarConstPtr field = fieldCreate->createScalar(valueFieldName,type);
    return createProperties(fieldName,field,properties);    
}

ScalarArrayConstPtr StandardField::scalarArray(
    String fieldName,ScalarType elementType)
{
    return fieldCreate->createScalarArray(fieldName,elementType);
}

StructureConstPtr StandardField::scalarArray(
    String fieldName,ScalarType elementType, String properties)
{
    ScalarArrayConstPtr field = fieldCreate->createScalarArray(
        valueFieldName,elementType);
    return createProperties(fieldName,field,properties);
}

StructureArrayConstPtr StandardField::structureArray(
    String fieldName,StructureConstPtr structure)
{
    return fieldCreate->createStructureArray(fieldName,structure);
}

StructureConstPtr StandardField::structureArray(
    String fieldName,StructureConstPtr structure,String properties)
{
    StructureArrayConstPtr field = fieldCreate->createStructureArray(
        valueFieldName,structure);
    return createProperties(fieldName,field,properties);
}

StructureConstPtr StandardField::structure(
    String fieldName,int numFields,FieldConstPtrArray fields)
{
    return fieldCreate->createStructure(fieldName,numFields,fields);
}

StructureConstPtr StandardField::enumerated(String fieldName)
{
    FieldConstPtrArray fields = new FieldConstPtr[2];
    fields[0] = fieldCreate->createScalar(String("index"),pvInt);
    fields[1] = fieldCreate->createScalarArray(String("choices"),pvString);
    return fieldCreate->createStructure(fieldName,2,fields);
}

StructureConstPtr StandardField::enumerated(
    String fieldName,String properties)
{
    StructureConstPtr field = standardField->enumerated(valueFieldName);
    return createProperties(fieldName,field,properties);
}

ScalarConstPtr StandardField::scalarValue(ScalarType type)
{
    return fieldCreate->createScalar(valueFieldName,type);
}

StructureConstPtr StandardField::scalarValue(ScalarType type,String properties)
{
    ScalarConstPtr field =  fieldCreate->createScalar(valueFieldName,type);
    return createProperties(valueFieldName,field,properties);
}

ScalarArrayConstPtr StandardField::scalarArrayValue(ScalarType elementType)
{
    return fieldCreate->createScalarArray(valueFieldName,elementType);
}

StructureConstPtr StandardField::scalarArrayValue(
    ScalarType elementType, String properties)
{
    ScalarArrayConstPtr field = fieldCreate->createScalarArray(
         valueFieldName,elementType);
    return createProperties(valueFieldName,field,properties);

}

StructureArrayConstPtr StandardField::structureArrayValue(
    StructureConstPtr structure)
{
    return fieldCreate->createStructureArray(valueFieldName,structure);
}

StructureConstPtr StandardField::structureArrayValue(
    StructureConstPtr structure,String properties)
{
    StructureArrayConstPtr field = fieldCreate->createStructureArray(
        valueFieldName,structure);
    return createProperties(valueFieldName,field,properties);

}

StructureConstPtr StandardField::structureValue(
    int numFields,FieldConstPtrArray fields)
{
    return fieldCreate->createStructure(valueFieldName,numFields,fields);
}

StructureConstPtr StandardField::enumeratedValue()
{
    FieldConstPtrArray fields = new FieldConstPtr[2];
    fields[0] = fieldCreate->createScalar(String("index"),pvInt);
    fields[1] = fieldCreate->createScalarArray(String("choices"),pvString);
    return fieldCreate->createStructure(valueFieldName,2,fields);
}

StructureConstPtr StandardField::enumeratedValue( String properties)
{
    StructureConstPtr field = standardField->enumerated(valueFieldName);
    return createProperties(valueFieldName,field,properties);
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
    createFloatAlarm();
    createDoubleAlarm();
    createEnumeratedAlarm();
}


StandardField::StandardField(){init();}

StandardField::~StandardField(){
}

static void myDeleteStatic(void*)
{
    alarmField.reset();
    timeStampField.reset();
    displayField.reset();
    controlField.reset();
    booleanAlarmField.reset();
    byteAlarmField.reset();
    shortAlarmField.reset();
    intAlarmField.reset();
    longAlarmField.reset();
    floatAlarmField.reset();
    doubleAlarmField.reset();
    enumeratedAlarmField.reset();

}

static void myInitStatic(void*)
{
    standardField = new StandardField();
    fieldCreate = getFieldCreate();
    epicsAtExit(&myDeleteStatic,0);
}

static
epicsThreadOnceId myInitOnce = EPICS_THREAD_ONCE_INIT;

StandardField * getStandardField() {
    epicsThreadOnce(&myInitOnce,&myInitStatic,0);
    return standardField;
}

}}
