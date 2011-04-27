
#include "convert.h"

#include <algorithm>
#include <iterator>
#include <sstream>

namespace epics { namespace pvData {

// PVXXX object comparison

bool operator==(PVField& left, PVField& right)
{
 return getConvert()->equals(left,right);
}

// Introspection object comparision

/** Field equality conditions:
 * 1) same instance
 * 2) same type (field and scalar/element)
 * Note: not the same name
 */
bool operator==(const Field& a, const Field& b)
{
    if(&a==&b)
        return true;
    if(a.getType()!=b.getType())
        return false;
    switch(a.getType()) {
    case scalar: {
        const Scalar &A=static_cast<const Scalar&>(a);
        const Scalar &B=static_cast<const Scalar&>(b);
        return A==B;
    }
    case scalarArray: {
            const ScalarArray &A=static_cast<const ScalarArray&>(a);
            const ScalarArray &B=static_cast<const ScalarArray&>(b);
            return A==B;
        }
    case structure: {
            const Structure &A=static_cast<const Structure&>(a);
            const Structure &B=static_cast<const Structure&>(b);
            return A==B;
        }
    case structureArray: {
            const StructureArray &A=static_cast<const StructureArray&>(a);
            const StructureArray &B=static_cast<const StructureArray&>(b);
            return A==B;
        }
    default:
        throw std::logic_error("Invalid Field type in comparision");
    }
}

bool operator==(const Scalar& a, const Scalar& b)
{
    if(&a==&b)
        return true;
    return a.getScalarType()==b.getScalarType();
}

bool operator==(const ScalarArray& a, const ScalarArray& b)
{
    if(&a==&b)
        return true;
    return a.getElementType()==b.getElementType();
}

bool operator==(const Structure& a, const Structure& b)
{
    if(&a==&b)
        return true;
    int nflds=a.getNumberFields();
    if (b.getNumberFields()!=nflds)
        return false;
    return std::equal(a.getFields(), a.getFields()+nflds,
                      b.getFields());
}

bool operator==(const StructureArray& a, const StructureArray& b)
{
    return a.structure() == b.structure();
}

namespace nconvert {

} // namespace nconvert

}} // namespace epics::pvData
