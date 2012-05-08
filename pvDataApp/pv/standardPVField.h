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

class StandardPVField;
typedef std::tr1::shared_ptr<StandardPVField> StandardPVFieldPtr;

class StandardPVField : private NoDefaultMethods {
public:
    static StandardPVFieldPtr getStandardPVField();
    ~StandardPVField();
    PVStructurePtr scalar(ScalarType type,String properties);
    PVStructurePtr scalarArray(ScalarType elementType, String properties);
    PVStructurePtr structureArray(StructureConstPtr structure,String properties);
    PVStructurePtr enumerated(StringArray choices);
    PVStructurePtr enumerated(StringArray choices, String properties);
private:
    StandardPVField();
};

extern StandardPVFieldPtr getStandardPVField();
    
}}
#endif  /* STANDARDPVFIELD_H */
