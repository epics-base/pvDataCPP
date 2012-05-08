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

#include <pv/pvIntrospect.h>
#include <pv/epicsException.h>

#include "dbDefs.h" // for NELEMENTS

namespace epics { namespace pvData {

namespace TypeFunc {
    static const char* names[] = {
        "scalar", "scalarArray", "structure", "structureArray",
    };
    const char* name(Type t) {
        if (t<int(pvBoolean) || t>int(pvString))
            THROW_EXCEPTION2(std::invalid_argument, "logic error unknown Type");
        return names[t];
    }
    void toString(StringBuilder buf,const Type type) {
        *buf += name(type);
    }
} // namespace TypeFunc


namespace ScalarTypeFunc {
    bool isInteger(ScalarType type) {
        if(type>=pvByte && type<=pvULong) return true;
        return false;
    }

    bool isUInteger(ScalarType type) {
        if(type>=pvUByte && type<=pvULong) return true;
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
    
    static const char* names[] = {
        "boolean",
        "byte", "short", "int", "long",
        "ubyte", "ushort", "uint", "ulong",
        "float", "double", "string",
    };
    ScalarType getScalarType(String pvalue) {
        for(size_t i=0; i<NELEMENTS(names); i++)
            if(pvalue==names[i])
                return ScalarType(i);
        THROW_EXCEPTION2(std::invalid_argument, "error unknown ScalarType");
    }

    const char* name(ScalarType t) {
        if (t<pvBoolean || t>pvString)
            THROW_EXCEPTION2(std::invalid_argument, "error unknown ScalarType");
        return names[t];
    }

    void toString(StringBuilder buf,const ScalarType scalarType) {
        *buf += name(scalarType);
    }

} // namespace ScalarTypeFunc

}}
