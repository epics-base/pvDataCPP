
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <epicsTypes.h>

#include "pvIntrospect.h"

namespace epics { namespace pvData {

    void TypeFunc::toString(StringBuilder buf,const Type type) {
        static String unknownString = "logic error unknown Type";
        switch(type) {
        case scalar : *buf += "scalar"; break;
        case scalarArray : *buf += "scalarArray"; break;
        case structure : *buf += "structure"; break;
        case structureArray : *buf += "structureArray"; break;
        default:
        throw std::invalid_argument(unknownString);
        }
    }


    bool ScalarTypeFunc::isInteger(ScalarType type) {
        if(type>=pvByte && type<=pvLong) return true;
        return false;
    }

    bool ScalarTypeFunc::isNumeric(ScalarType type) {
        if(type>=pvByte && type<=pvDouble) return true;
        return false;
    }

    bool ScalarTypeFunc::isPrimitive(ScalarType type) {
        if(type>=pvBoolean && type<=pvDouble) return true;
        return false;
    }

    ScalarType ScalarTypeFunc::getScalarType(String pvalue) {
        static String unknownString = "error unknown ScalarType";
        if(pvalue.compare("boolean")==0) return pvBoolean;
        if(pvalue.compare("byte")==0) return pvByte;
        if(pvalue.compare("short")==0) return pvShort;
        if(pvalue.compare("int")==0) return pvInt;
        if(pvalue.compare("long")==0) return pvLong;
        if(pvalue.compare("float")==0) return pvFloat;
        if(pvalue.compare("double")==0) return pvDouble;
        if(pvalue.compare("string")==0) return pvString;
        throw std::invalid_argument(unknownString);
    }
    void ScalarTypeFunc::toString(StringBuilder buf,const ScalarType scalarType) {
        static String unknownString = "logic error unknown ScalarType";
        switch(scalarType) {
        case pvBoolean : *buf += "boolean"; return;
        case pvByte : *buf += "byte"; return;;
        case pvShort : *buf += "short"; return;
        case pvInt : *buf += "int"; return;
        case pvLong : *buf += "long"; return;
        case pvFloat : *buf += "float"; return;
        case pvDouble : *buf += "double"; return;
        case pvString : *buf += "string"; return;
        }
        throw std::invalid_argument(unknownString);
    }


}}
