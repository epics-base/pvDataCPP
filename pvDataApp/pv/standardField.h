/* standardField.h */
#include <string>
#include <stdexcept>
#ifndef STANDARDFIELD_H
#define STANDARDFIELD_H
#include "pvIntrospect.h"

namespace epics { namespace pvData { 

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
        StructureConstPtr enumerated(String fieldName,
            StringArray choices);
        StructureConstPtr enumerated(String fieldName,
            StringArray choices, String properties);
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
        StructureConstPtr enumeratedValue(StringArray choices);
        StructureConstPtr enumeratedValue(StringArray choices,
             String properties);
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
    };

    extern StandardField * getStandardField();
    
}}
#endif  /* STANDARDFIELD_H */
