/* convert.h */
#include <string>
#include <stdexcept>
#include <map>
#ifndef CONVERT_H
#define CONVERT_H
#include "pvIntrospect.h"
#include "pvData.h"

namespace epics { namespace pvData { 


    class Convert {
    public:
        Convert();
        ~Convert();
        void getFullName(std::string *buf,PVField *pvField);
        void getString(std::string *buf,PVfield *pvField,int indentLevel);
        void getString(std::string *buf,PVfield *pvField);
        void fromString(PVScalar *pv, StringConstPtr from);
        int fromString(PVScalarArray *pv, StringConstPtr from);
    private:
        Convert(); // not implemented
        Convert(Convert const & ); // not implemented
        Convert & operator=(Convert const &); //not implemented
        class Convert *pImpl;
    };

    extern Convert * getConvert();
    
}}
#endif  /* CONVERT_H */
