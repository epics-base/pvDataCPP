/* StandardField.cpp */
#include <string>
#include <stdexcept>
#include <lock.h>
#include "pvIntrospect.h"
#include "pvData.h"
#include "convert.h"
#include "standardField.h"

namespace epics { namespace pvData { 

static String notImplemented("not implemented");

    StandardField::StandardField(){}

    StandardField::~StandardField(){}

    PVScalar * StandardField::scalarValue(ScalarType scalarType)
    {
        ScalarConstPtr scalar = getFieldCreate()->createScalar(
           String("value"),scalarType);
        return getPVDataCreate()->createPVScalar(0,scalar);
    }

    PVScalarArray * StandardField::scalarArrayValue(ScalarType elementType)
    {
        ScalarArrayConstPtr scalarArray = getFieldCreate()->createScalarArray(
           String("value"),elementType);
        return getPVDataCreate()->createPVScalarArray(0,scalarArray);
    }

    PVStructure * StandardField::scalarValue(ScalarType type,String properties)
    {
        throw std::logic_error(notImplemented);
    }

    PVStructure * StandardField::scalarArrayValue(ScalarType elementType,
        String properties)
    {
        throw std::logic_error(notImplemented);
    }

    PVStructure * StandardField::enumeratedValue(StringArray choices)
    {
        throw std::logic_error(notImplemented);
    }

    PVStructure * StandardField::enumeratedValue(StringArray choices,
        String properties)
    {
        throw std::logic_error(notImplemented);
    }

    PVStructure * StandardField::alarm()
    {
        throw std::logic_error(notImplemented);
    }

    PVStructure * StandardField::timeStamp()
    {
        throw std::logic_error(notImplemented);
    }


    static StandardField* instance = 0;


    class StandardFieldExt : public StandardField {
    public:
        StandardFieldExt(): StandardField(){};
    };

    StandardField * getStandardField() {
        static Mutex mutex = Mutex();
        Lock xx(&mutex);

        if(instance==0) instance = new StandardFieldExt();
        return instance;
    }
    
}}
