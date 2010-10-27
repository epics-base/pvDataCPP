/* standardPVField.h */
#include <string>
#include <stdexcept>
#ifndef STANDARDPVFIELD_H
#define STANDARDPVFIELD_H
#include "pvIntrospect.h"
#include "pvData.h"

namespace epics { namespace pvData { 

    class StandardPVField : private NoDefaultMethods {
    public:
        StandardPVField();
        ~StandardPVField();
        PVScalar * scalar(String fieldName,ScalarType type);
        PVStructure * scalar(String fieldName,ScalarType type,String properties);
        PVScalarArray * scalarArray(String fieldName,ScalarType elementType);
        PVStructure * scalarArray(String fieldName,ScalarType elementType, String properties);
        PVStructureArray * structureArray(String fieldName,StructureConstPtr structure);
        PVStructure * structureArray(String fieldName,StructureConstPtr structure,String properties);
        PVStructure *structure(String fieldName,PVStructure *pvStructure);
        PVStructure *structure(String fieldName,PVStructure *pvStructure,String properties);
        PVStructure * enumerated(String fieldName,StringArray choices);
        PVStructure * enumerated(String fieldName,StringArray choices, String properties);
        PVScalar * scalarValue(ScalarType type);
        PVStructure * scalarValue(ScalarType type,String properties);
        PVScalarArray * scalarArrayValue(ScalarType elementType);
        PVStructure * scalarArrayValue(ScalarType elementType, String properties);
        PVStructureArray * structureArrayValue(StructureConstPtr structure);
        PVStructure * structureArrayValue(StructureConstPtr structure,String properties);
        PVStructure *structureValue(PVStructure *pvStructure);
        PVStructure *structureValue(PVStructure *pvStructure,String properties);
        PVStructure * enumeratedValue(StringArray choices);
        PVStructure * enumeratedValue(StringArray choices, String properties);
        PVStructure * alarm();
        PVStructure * timeStamp();
        PVStructure * display();
        PVStructure * control();
        PVStructure * booleanAlarm();
        PVStructure * byteAlarm();
        PVStructure * shortAlarm();
        PVStructure * intAlarm();
        PVStructure * longAlarm();
        PVStructure * floatAlarm();
        PVStructure * doubleAlarm();
        PVStructure * enumeratedAlarm();
        PVStructure * powerSupply();
    };

    extern StandardPVField * getStandardPVField();
    
}}
#endif  /* STANDARDPVFIELD_H */
