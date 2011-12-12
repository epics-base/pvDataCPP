/* standardPVField.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 * Author - Marty Kraimer
 */
#ifndef STANDARDPVFIELD_H
#define STANDARDPVFIELD_H
#include <string>
#include <stdexcept>
#include <pv/pvIntrospect.h>
#include <pv/pvData.h>

namespace epics { namespace pvData { 
/**
 * StandardPVField is a class or creating standard data fields.
 * Like class StandardField it has two forms of the methods which create a fields:
 *      one without properties and one with properties.
 * The properties are some combination of alarm, timeStamp, control, display, and valueAlarm.
 * Just like StandardField there are methods to create the standard properties.
 *
 * StandardPVField is a singleton class. The class is accessed via the statement: {@code
    StandardPVField *standardPVField = getStandardPVField();
 * }
 */

class StandardPVField : private NoDefaultMethods {
public:
    StandardPVField();
    ~StandardPVField();
    PVScalar * scalar(PVStructure *parent,String fieldName,ScalarType type);
    PVStructure * scalar(PVStructure *parent,
        String fieldName,ScalarType type,String properties);
    PVScalarArray * scalarArray(PVStructure *parent,
        String fieldName,ScalarType elementType);
    PVStructure * scalarArray(PVStructure *parent,
        String fieldName,ScalarType elementType, String properties);
    PVStructureArray * structureArray(PVStructure *parent,
        String fieldName,StructureConstPtr structure);
    PVStructure* structureArray(PVStructure *parent,
        String fieldName,StructureConstPtr structure,String properties);
    PVStructure * enumerated(PVStructure *parent,
        String fieldName,StringArray choices, int number);
    PVStructure * enumerated(PVStructure *parent,
        String fieldName,StringArray choices, int number, String properties);
    PVScalar * scalarValue(PVStructure *parent,ScalarType type);
    PVStructure * scalarValue(PVStructure *parent,
        ScalarType type,String properties);
    PVScalarArray * scalarArrayValue(
        PVStructure *parent,ScalarType elementType);
    PVStructure * scalarArrayValue(PVStructure *parent,
        ScalarType elementType, String properties);
    PVStructureArray * structureArrayValue(PVStructure *parent,
        StructureConstPtr structure);
    PVStructure * structureArrayValue(PVStructure *parent,
        StructureConstPtr structure,String properties);
    PVStructure * enumeratedValue(
        PVStructure *parent,StringArray choices,int number);
    PVStructure * enumeratedValue(PVStructure *parent,
        StringArray choices,int number, String properties);
    PVStructure * alarm(PVStructure *parent);
    PVStructure * timeStamp(PVStructure *parent);
    PVStructure * display(PVStructure *parent);
    PVStructure * control(PVStructure *parent);
    PVStructure * booleanAlarm(PVStructure *parent);
    PVStructure * byteAlarm(PVStructure *parent);
    PVStructure * shortAlarm(PVStructure *parent);
    PVStructure * intAlarm(PVStructure *parent);
    PVStructure * longAlarm(PVStructure *parent);
    PVStructure * floatAlarm(PVStructure *parent);
    PVStructure * doubleAlarm(PVStructure *parent);
    PVStructure * enumeratedAlarm(PVStructure *parent);
    PVStructure * powerSupply(PVStructure *parent);
};

extern StandardPVField * getStandardPVField();
    
}}
#endif  /* STANDARDPVFIELD_H */
