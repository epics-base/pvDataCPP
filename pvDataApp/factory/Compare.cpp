/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 * @author mes
 */

#include <pv/convert.h>

#include <algorithm>
#include <iterator>
#include <sstream>

namespace epics { namespace pvData {

// Introspection object comparision

/** Field equality conditions:
 * 1) same instance
 * 2) same type (field and scalar/element), same name, same subfields (if any)
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
    if (a.getID()!=b.getID())
    	return false;
    size_t nflds=a.getNumberFields();
    if (b.getNumberFields()!=nflds)
        return false;

    // std::equals does not work, since FieldConstPtrArray is an array of shared_pointers
    FieldConstPtrArray af = a.getFields();
    FieldConstPtrArray bf = b.getFields();
    for (size_t i = 0; i < nflds; i++)
        if (*(af[i].get()) != *(bf[i].get()))
            return false;

    StringArray an = a.getFieldNames();
    StringArray bn = b.getFieldNames();
    return std::equal( an.begin(), an.end(), bn.begin() );
}

bool operator==(const StructureArray& a, const StructureArray& b)
{
    return *(a.getStructure().get())==*(b.getStructure().get());
}

// PVXXX object comparison

namespace {

// fully typed comparisons

template<typename T>
bool compareScalar(PVScalarValue<T>* left, PVScalarValue<T>* right)
{
    return left->get()==right->get();
}

template<typename T>
bool compareArray(PVValueArray<T>* left, PVValueArray<T>* right)
{
    return std::equal(left->get(), left->get()+left->getLength(), right->get());
}

// partially typed comparisons

bool compareField(PVScalar* left, PVScalar* right)
{
    ScalarType lht = left->getScalar()->getScalarType();
    if(lht != right->getScalar()->getScalarType())
        return false;
    switch(lht) {
#define OP(ENUM, TYPE) case ENUM: return compareScalar(static_cast<PVScalarValue<TYPE>*>(left), static_cast<PVScalarValue<TYPE>*>(right))
    OP(pvBoolean, uint8);
    OP(pvUByte, uint8);
    OP(pvByte, int8);
    OP(pvUShort, uint16);
    OP(pvShort, int16);
    OP(pvUInt, uint32);
    OP(pvInt, int32);
    OP(pvULong, uint64);
    OP(pvLong, int64);
    OP(pvFloat, float);
    OP(pvDouble, double);
#undef OP
    case pvString: {
            PVString *a=static_cast<PVString*>(left), *b=static_cast<PVString*>(right);
            return a->get()==b->get();
        }
    }
    throw std::logic_error("PVScalar with invalid scalar type!");
}

bool compareField(PVScalarArray* left, PVScalarArray* right)
{
    ScalarType lht = left->getScalarArray()->getElementType();
    if(lht != right->getScalarArray()->getElementType())
        return false;

    if(left->getLength()!=right->getLength())
        return false;

    switch(lht) {
#define OP(ENUM, TYPE) case ENUM: return compareArray(static_cast<PVValueArray<TYPE>*>(left), static_cast<PVValueArray<TYPE>*>(right))
    OP(pvBoolean, uint8);
    OP(pvUByte, uint8);
    OP(pvByte, int8);
    OP(pvUShort, uint16);
    OP(pvShort, int16);
    OP(pvUInt, uint32);
    OP(pvInt, int32);
    OP(pvULong, uint64);
    OP(pvLong, int64);
    OP(pvFloat, float);
    OP(pvDouble, double);
    OP(pvString, String);
#undef OP
    }
    throw std::logic_error("PVScalarArray with invalid element type!");
}

bool compareField(PVStructure* left, PVStructure* right)
{
    StructureConstPtr ls = left->getStructure();

    if(*ls!=*right->getStructure())
        return false;

    const PVFieldPtrArray& lf = left->getPVFields();
    const PVFieldPtrArray& rf = right->getPVFields();

    for(size_t i=0, nfld=ls->getNumberFields(); i<nfld; i++) {
        if(*lf[i]!=*rf[i])
            return false;
    }
    return true;
}

bool compareField(PVStructureArray* left, PVStructureArray* right)
{
    if(left->getLength()!=right->getLength())
        return false;

    StructureConstPtr ls = left->getStructureArray()->getStructure();

    if(*ls!=*right->getStructureArray()->getStructure())
        return false;

    const PVStructureArray::pointer ld=left->get(), rd=right->get();

    for(size_t i=0, ilen=left->getLength(); i<ilen; i++)
    {
        const PVFieldPtrArray& lf = ld[i]->getPVFields();
        const PVFieldPtrArray& rf = rd[i]->getPVFields();

        for(size_t k=0, klen=ls->getNumberFields(); k<klen; k++)
        {
            if(*lf[i]!=*rf[i])
                return false;
        }
    }
    return true;
}

} // end namespace

// untyped comparison

bool operator==(PVField& left, PVField& right)
{
    if(&left == &right)
        return true;

    Type lht = left.getField()->getType();
    if(lht != right.getField()->getType())
        return false;

    switch(lht) {
    case scalar: return compareField(static_cast<PVScalar*>(&left), static_cast<PVScalar*>(&right));
    case scalarArray: return compareField(static_cast<PVScalarArray*>(&left), static_cast<PVScalarArray*>(&right));
    case structure: return compareField(static_cast<PVStructure*>(&left), static_cast<PVStructure*>(&right));
    case structureArray: return compareField(static_cast<PVStructureArray*>(&left), static_cast<PVStructureArray*>(&right));
    }
    throw std::logic_error("PVField with invalid type!");
}

}} // namespace epics::pvData
