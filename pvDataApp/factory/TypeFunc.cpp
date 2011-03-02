/*TypeFunc.cpp*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>

#include "pvIntrospect.h"
#include "epicsException.h"

namespace epics { namespace pvData {

namespace TypeFunc {
    void toString(StringBuilder buf,const Type type) {
        static const String unknownString("logic error unknown Type");
        switch(type) {
        case scalar : *buf += "scalar"; break;
        case scalarArray : *buf += "scalarArray"; break;
        case structure : *buf += "structure"; break;
        case structureArray : *buf += "structureArray"; break;
        default:
            THROW_EXCEPTION2(std::invalid_argument, unknownString);
        }
    }
} // namespace TypeFunc


namespace ScalarTypeFunc {
    bool isInteger(ScalarType type) {
        if(type>=pvByte && type<=pvLong) return true;
        return false;
    }

    bool isNumeric(ScalarType type) {
        if(type>=pvByte && type<=pvDouble) return true;
        return false;
    }

    bool isPrimitive(ScalarType type) {
        if(type>=pvBoolean && type<=pvDouble) return true;
        return false;
    }

    ScalarType getScalarType(String pvalue) {
        static const String unknownString("error unknown ScalarType");
        if(pvalue == "boolean") return pvBoolean;
        if(pvalue == "byte") return pvByte;
        if(pvalue == "short") return pvShort;
        if(pvalue == "int") return pvInt;
        if(pvalue == "long") return pvLong;
        if(pvalue == "float") return pvFloat;
        if(pvalue == "double") return pvDouble;
        if(pvalue == "string") return pvString;
        THROW_EXCEPTION2(std::invalid_argument, unknownString);
    }
    void toString(StringBuilder buf,const ScalarType scalarType) {
        static const String unknownString("logic error unknown ScalarType");
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
        THROW_EXCEPTION2(std::invalid_argument, unknownString);
    }

} // namespace ScalarTypeFunc

}}
