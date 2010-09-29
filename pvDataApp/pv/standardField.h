/* standardField.h */
#include <string>
#include <stdexcept>
#ifndef STANDARDFIELD_H
#define STANDARDFIELD_H
#include "pvIntrospect.h"
#include "pvData.h"

namespace epics { namespace pvData { 

    class StandardField : private NoDefaultMethods {
    public:
        StandardField();
        ~StandardField();
        PVScalar * scalarValue(ScalarType type);
        PVScalarArray * scalarArrayValue(ScalarType elementType);
        PVStructure * scalarValue(ScalarType type,String properties);
        PVStructure * scalarArrayValue(ScalarType elementType,
            String properties);
        PVStructure * enumeratedValue(StringArray choices);
        PVStructure * enumeratedValue(StringArray choices,
            String properties);
        PVStructure * alarm();
        PVStructure * timeStamp();
    };

    extern StandardField * getStandardField();
    
}}
#endif  /* STANDARDFIELD_H */
