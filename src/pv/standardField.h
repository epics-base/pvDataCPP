/* standardField.h */
/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */
/**
 *  @author mrk
 */
#ifndef STANDARDFIELD_H
#define STANDARDFIELD_H

#include <string>
#include <stdexcept>

#include <pv/pvIntrospect.h>

#include <shareLib.h>

namespace epics { namespace pvData { 


class StandardField;
typedef std::tr1::shared_ptr<StandardField> StandardFieldPtr;

/**
 * @brief Standard Fields is a class or creating or sharing Field objects for standard fields.
 *
 * For each type of standard object two methods are defined:s
 *      one with no properties and with properties
 * The property field is a comma separated string of property names of the following:
 *    alarm, timeStamp, display, control, and valueAlarm.
 * An example is "alarm,timeStamp,valueAlarm".
 * The method with properties creates a structure with fields named fieldName
 *     and each of the property names.
 * Each property field is a structure defining the property.
 * The details about each property is given in the section named "Property".
 * For example the call:
 * {@code
   StructureConstPtr example = standardField->scalar(
        std::string("value"),
        pvDouble,
        std::string("value,alarm,timeStamp")); 
 * }
 * Will result in a Field definition that has the form: {@code
  structure example
    double value
    structure alarm
        int severity
        int status
        string message
   structure timeStamp
        long secondsPastEpoch
        int  nanoseconds
        int userTag
 * }
 * In addition there are methods that create each of the property structures,
 * i.e. the methods named: alarm, .... enumeratedAlarm."
 *
 * StandardField is a singleton class. The class is accessed via the statement: {@code
    StandardField *standardField = getStandardField();
 * }
 */
class epicsShareClass StandardField {
public:
    /** 
     * getStandardField returns the singleton.
     * @return Shared pointer to StandardField.
     */
    static const StandardFieldPtr& getStandardField();
    ~StandardField();
    /** Create a structure that has a scalar value field.
     * @param type The type.
     * @param properties A comma separated list of properties.
     * This is some combination of "alarm,timeStamp,display,control,valueAlarm".
     * @return The const shared pointer to the structure.
     */
    StructureConstPtr scalar(ScalarType type,std::string const & properties);
    /** Create a structure that has a union value field.
     * @param punion The interface for value field.
     * @param properties A comma separated list of properties.
     * This is some combination of "alarm,timeStamp,display,control,valueAlarm".
     * @return The const shared pointer to the structure.
     */
    StructureConstPtr regUnion(
        UnionConstPtr const & punion,
        std::string const & properties);
    /** Create a structure that has a variant union value field.
     * @param properties A comma separated list of properties.
     * This is some combination of "alarm,timeStamp,display,control,valueAlarm".
     * @return The const shared pointer to the structure.
     */
    StructureConstPtr variantUnion(std::string const & properties);
    /** Create a structure that has a scalarArray value field.
     * @param elementType The element type.
     * @param properties A comma separated list of properties.
     * This is some combination of "alarm,timeStamp,display,control,valueAlarm".
     * @return The const shared pointer to the structure.
     */
    StructureConstPtr scalarArray(ScalarType elementType, std::string const & properties);
    /** Create a structure that has a structureArray value field.
     * @param structure The Structure introspection object for elements of the value field.
     * @param properties A comma separated list of properties.
     * This is some combination of "alarm,timeStamp,display,control,valueAlarm".
     * @return The const shared pointer to the structure.
     */
    StructureConstPtr structureArray(
        StructureConstPtr const & structure,
        std::string const & properties);
    /** Create a structure that has a unionArray value field.
     * @param punion The Union introspection object for elements of the value field.
     * @param properties A comma separated list of properties.
     * This is some combination of "alarm,timeStamp,display,control".
     * @return The const shared pointer to the structure.
     */
    StructureConstPtr unionArray(
        UnionConstPtr const & punion,
        std::string const & properties);
    /** Create a structure that has an enumerated structure value field.
     *  The id for the structure is "enum_t".
     * @return The const shared pointer to the structure.
     */
    StructureConstPtr enumerated();
    /** Create a structure that has an enumerated structure value field
     * The id for the structure is "epics:nt/NTEnum:1.0".
     * @param properties A comma separated list of properties.
     * This is some combination of "alarm,timeStamp,display,control,valueAlarm".
     * @return The const shared pointer to the structure.
     */
    StructureConstPtr enumerated(std::string const & properties);
    /**
     * create an alarm structure
     * @return The const shared pointer to the structure.
     */
    StructureConstPtr alarm();
    /**
     * create a timeStamp structure
     * @return The const shared pointer to the structure.
     */
    StructureConstPtr timeStamp();
    /**
     * create a display structure
     * @return The const shared pointer to the structure.
     */
    StructureConstPtr display();
    /**
     * create a control structure
     * @return The const shared pointer to the structure.
     */
    StructureConstPtr control();
    /**
     * create a boolean alarm structure
     * @return The const shared pointer to the structure.
     */
    StructureConstPtr booleanAlarm();
    /**
     * create a byte alarm structure
     * @return The const shared pointer to the structure.
     */
    StructureConstPtr byteAlarm();
    /**
     * create a unsigned byte alarm structure
     * @return The const shared pointer to the structure.
     */
    StructureConstPtr ubyteAlarm();
    /**
     * create a short alarm structure
     * @return The const shared pointer to the structure.
     */
    StructureConstPtr shortAlarm();
    /**
     * create a unsigned short alarm structure
     * @return The const shared pointer to the structure.
     */
    StructureConstPtr ushortAlarm();
    /**
     * create an int alarm structure
     * @return The const shared pointer to the structure.
     */
    StructureConstPtr intAlarm();
    /**
     * create a unsigned int alarm structure
     * @return The const shared pointer to the structure.
     */
    StructureConstPtr uintAlarm();
    /**
     * create a long alarm structure
     * @return The const shared pointer to the structure.
     */
    StructureConstPtr longAlarm();
    /**
     * create a unsigned long alarm structure
     * @return The const shared pointer to the structure.
     */
    StructureConstPtr ulongAlarm();
    /**
     * create a float alarm structure
     * @return The const shared pointer to the structure.
     */
    StructureConstPtr floatAlarm();
    /**
     * create a double alarm structure
     * @return The const shared pointer to the structure.
     */
    StructureConstPtr doubleAlarm();
    /**
     * create an enumerated alarm structure
     * @return The const shared pointer to the structure.
     */
    StructureConstPtr enumeratedAlarm();
private:
    StandardField();
    void init();
    StructureConstPtr createProperties(
        std::string id,FieldConstPtr field,std::string properties);
    FieldCreatePtr fieldCreate;
    std::string notImplemented;
    std::string valueFieldName;
    StructureConstPtr alarmField;
    StructureConstPtr timeStampField;
    StructureConstPtr displayField;
    StructureConstPtr controlField;
    StructureConstPtr booleanAlarmField;
    StructureConstPtr byteAlarmField;
    StructureConstPtr shortAlarmField;
    StructureConstPtr intAlarmField;
    StructureConstPtr longAlarmField;
    StructureConstPtr ubyteAlarmField;
    StructureConstPtr ushortAlarmField;
    StructureConstPtr uintAlarmField;
    StructureConstPtr ulongAlarmField;
    StructureConstPtr floatAlarmField;
    StructureConstPtr doubleAlarmField;
    StructureConstPtr enumeratedAlarmField;
    void createAlarm();
    void createTimeStamp();
    void createDisplay();
    void createControl();
    void createBooleanAlarm();
    void createByteAlarm();
    void createShortAlarm();
    void createIntAlarm();
    void createLongAlarm();
    void createUByteAlarm();
    void createUShortAlarm();
    void createUIntAlarm();
    void createULongAlarm();
    void createFloatAlarm();
    void createDoubleAlarm();
    void createEnumeratedAlarm();
    //friend StandardFieldPtr getStandardField();
};

FORCE_INLINE const StandardFieldPtr& getStandardField() {
    return StandardField::getStandardField();
}
    
}}
#endif  /* STANDARDFIELD_H */
