/* standardField.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 * Author - Marty Kraimer
 */
/**
 *  @author mrk
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
 * The method with properties creates a structure with fields named fieldName
 *     and each of the property names.
 * Each property field is a structure defining the property.
 * The details about each property is given in the section named "Property".
 * For example the call:
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
        int severity
        int status
        string message
   structure timeStamp
        long secondsPastEpoch
        int  nanoSeconds
        int userTag
 * }
 * In addition there are methods that create each of the property structures,
 * i.e. the methods named: alarm, .... enumeratedAlarm."
 *
 * StandardField is a singleton class. The class is accessed via the statement: {@code
    StandardField *standardField = getStandardField();
 * }
 */

class StandardField;
typedef std::tr1::shared_ptr<StandardField> StandardFieldPtr;

class StandardField {
public:
    static StandardFieldPtr getStandardField();
    ~StandardField();
    StructureConstPtr scalar(ScalarType type,String const & properties);
    StructureConstPtr scalarArray(ScalarType elementType, String const & properties);
    StructureConstPtr structureArray(StructureConstPtr const & structure,String const & properties);
    StructureConstPtr enumerated();
    StructureConstPtr enumerated(String const & properties);
    StructureConstPtr alarm();
    StructureConstPtr timeStamp();
    StructureConstPtr display();
    StructureConstPtr control();
    StructureConstPtr booleanAlarm();
    StructureConstPtr byteAlarm();
    StructureConstPtr ubyteAlarm();
    StructureConstPtr shortAlarm();
    StructureConstPtr ushortAlarm();
    StructureConstPtr intAlarm();
    StructureConstPtr uintAlarm();
    StructureConstPtr longAlarm();
    StructureConstPtr ulongAlarm();
    StructureConstPtr floatAlarm();
    StructureConstPtr doubleAlarm();
    StructureConstPtr enumeratedAlarm();
private:
    StandardField();
};

extern StandardFieldPtr getStandardField();
    
}}
#endif  /* STANDARDFIELD_H */
