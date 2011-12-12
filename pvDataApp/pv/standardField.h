/* standardField.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 * Author - Marty Kraimer
 */
#ifndef STANDARDFIELD_H
#define STANDARDFIELD_H
#include <string>
#include <stdexcept>
#include <pv/pvIntrospect.h>

namespace epics { namespace pvData { 

/**
 * Standard Fields is a class or creating or sharing Field objects for standard fields.
 * For each type of standard object two methods are defined:s
 *      one with no properties and with properties
 * The property field is a comma separated string of property names of the following:
 *    alarm, timeStamp, display, control, and valueAlarm.
 * An example is "alarm,timeStamp,valueAlarm".
 * The method with properties creates a structure with fields named fieldName and each of the property names.s
 * Each property field is a structure defining the property.
 * The details about each property is given in the section named "Property". For example the call:
 * {@code
   StructureConstPtr example = standardField->scalar(
        String("value"),
        pvDouble,
        String("value,alarm,timeStamp")); 
 * }
 * Will result in a Field definition that has the form: {@code
  structure example
    double value
    structure alarm
        structure severity
            int index
            string[] choices
       structure timeStamp
            long secondsPastEpoch
            int  nanoSeconds
 * }
 * In addition there are methods that create each of the property structures,
 * i.e. the methods named: alarm, .... enumeratedAlarm."
 *
 * StandardField is a singleton class. The class is accessed via the statement: {@code
    StandardField *standardField = getStandardField();
 * }
 */

class StandardField : private NoDefaultMethods {
public:
    StandardField();
    ~StandardField();
    ScalarConstPtr scalar(String fieldName,ScalarType type);
    StructureConstPtr scalar(String fieldName,
        ScalarType type,String properties);
    ScalarArrayConstPtr scalarArray(String fieldName,
        ScalarType elementType);
    StructureConstPtr scalarArray(String fieldName,
        ScalarType elementType, String properties);
    StructureArrayConstPtr structureArray(String fieldName,
        StructureConstPtr structure);
    StructureConstPtr structureArray(String fieldName,
        StructureConstPtr structure,String properties);
    StructureConstPtr structure(String fieldName,
        int numFields,FieldConstPtrArray fields);
    StructureConstPtr enumerated(String fieldName);
    StructureConstPtr enumerated(String fieldName, String properties);
    ScalarConstPtr scalarValue(ScalarType type);
    StructureConstPtr scalarValue(ScalarType type,String properties);
    ScalarArrayConstPtr scalarArrayValue(ScalarType elementType);
    StructureConstPtr scalarArrayValue(ScalarType elementType,
        String properties);
    StructureArrayConstPtr structureArrayValue(StructureConstPtr structure);
    StructureConstPtr structureArrayValue(StructureConstPtr structure,
        String properties);
    StructureConstPtr structureValue(
        int numFields,FieldConstPtrArray fields);
    StructureConstPtr enumeratedValue();
    StructureConstPtr enumeratedValue(String properties);
    StructureConstPtr alarm();
    StructureConstPtr timeStamp();
    StructureConstPtr display();
    StructureConstPtr control();
    StructureConstPtr booleanAlarm();
    StructureConstPtr byteAlarm();
    StructureConstPtr shortAlarm();
    StructureConstPtr intAlarm();
    StructureConstPtr longAlarm();
    StructureConstPtr floatAlarm();
    StructureConstPtr doubleAlarm();
    StructureConstPtr enumeratedAlarm();
private:
    static void init();
};

extern StandardField * getStandardField();
    
}}
#endif  /* STANDARDFIELD_H */
