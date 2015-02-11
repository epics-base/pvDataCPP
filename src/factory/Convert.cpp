/* Convert.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#include <cstdio>
#include <string>
#include <vector>
#include <stdexcept>
#include <typeinfo>

#define epicsExportSharedSymbols
#include <pv/lock.h>
#include <pv/pvIntrospect.h>
#include <pv/pvData.h>
#include <pv/convert.h>

using std::tr1::static_pointer_cast;
using std::size_t;
using std::string;

namespace epics { namespace pvData { 


static std::vector<string> split(string commaSeparatedList) {
    string::size_type numValues = 1;
    string::size_type index=0;
    while(true) {
        string::size_type pos = commaSeparatedList.find(',',index);
        if(pos==string::npos) break;
        numValues++;
        index = pos +1;
    }
    std::vector<string> valueList(numValues,"");
    index=0;
    for(size_t i=0; i<numValues; i++) {
        size_t pos = commaSeparatedList.find(',',index);
        string value = commaSeparatedList.substr(index,pos);
        valueList[i] = value;
        index = pos +1;
    }
    return valueList;
}

void Convert::getString(string *buf,PVField const *pvField,int /*indentLevel*/)
{
    // TODO indextLevel ignored
    std::ostringstream strm;
    pvField->dumpValue(strm);
    strm << std::endl;
//    PrinterPlain p;
//    p.setStream(strm);
//    p.print(*pvField);
    strm.str().swap(*buf);
}


size_t Convert::fromString(PVStructurePtr const &pvStructure, StringArray const & from, size_t fromStartIndex)
{
    size_t processed = 0;
    
    PVFieldPtrArray const & fieldsData = pvStructure->getPVFields();
    if (fieldsData.size() != 0) {
        size_t length = pvStructure->getStructure()->getNumberFields();
        for(size_t i=0; i<length; i++) {
            PVFieldPtr fieldField = fieldsData[i];

            Type type = fieldField->getField()->getType();
            if(type==structure) {
                PVStructurePtr pv = static_pointer_cast<PVStructure>(fieldField);
                size_t count = fromString(pv, from, fromStartIndex);
                processed += count;
                fromStartIndex += count;
            }
            else if(type==scalarArray) {
                PVScalarArrayPtr pv = static_pointer_cast<PVScalarArray>(fieldField);
                size_t count = fromString(pv, from[fromStartIndex]);
                processed += count;
                fromStartIndex += count;
            }
            else if(type==scalar) {
                PVScalarPtr pv = static_pointer_cast<PVScalar>(fieldField);
                fromString(pv, from[fromStartIndex++]);
                processed++;
            }
            else {
                // union, structureArray, unionArray not supported
                std::ostringstream oss;
                oss << "Convert::fromString unsupported fieldType " << type;
                throw std::logic_error(oss.str());
            }
        }
    }
    
    return processed;
}

size_t Convert::fromString(PVScalarArrayPtr const &pv, string from)
{
   if(from[0]=='[' && from[from.length()]==']') {
        size_t offset = from.rfind(']');
        from = from.substr(1, offset);
    }
    std::vector<string> valueList(split(from));
    size_t length = valueList.size();
    size_t num = fromStringArray(pv,0,length,valueList,0);
    if(num<length) length = num;
    pv->setLength(length);
    return length;
}

size_t Convert::fromStringArray(PVScalarArrayPtr const &pv,
                                size_t offset, size_t length,
                                StringArray const & from,
                                size_t fromOffset)
{
    size_t alen = pv->getLength();

    if(offset==0 && length>=alen) {
        // replace all existing elements
        assert(from.size()>=fromOffset+length);

        PVStringArray::svector data(length);
        std::copy(from.begin()+fromOffset,
                  from.begin()+fromOffset+length,
                  data.begin());

        PVStringArray::const_svector temp(freeze(data));
        pv->putFrom<string>(temp);
        return length;

    } else {
        // partial update.
        throw std::runtime_error("fromStringArray: partial update not implemented");
    }
}

size_t Convert::toStringArray(PVScalarArrayPtr const & pv,
                              size_t offset, size_t length,
                              StringArray  &to, size_t toOffset)
{
    PVStringArray::const_svector data;
    pv->getAs<string>(data);
    data.slice(offset, length);
    if(toOffset+data.size() > to.size())
        to.resize(toOffset+data.size());
    std::copy(data.begin()+toOffset, data.end(), to.begin());
    return data.size();
}

bool Convert::isCopyCompatible(FieldConstPtr const &from, FieldConstPtr const &to)
{
    if(from->getType()!=to->getType()) return false;
    switch(from->getType()) {
    case scalar:
        {
             ScalarConstPtr xxx = static_pointer_cast<const Scalar>(from);
             ScalarConstPtr yyy = static_pointer_cast<const Scalar>(to);
             return isCopyScalarCompatible(xxx,yyy);
        }
    case scalarArray:
        {
             ScalarArrayConstPtr xxx = static_pointer_cast<const ScalarArray>(from);
             ScalarArrayConstPtr yyy = static_pointer_cast<const ScalarArray>(to);
             return isCopyScalarArrayCompatible(xxx,yyy);
        }
    case structure:
        {
             StructureConstPtr xxx = static_pointer_cast<const Structure>(from);
             StructureConstPtr yyy = static_pointer_cast<const Structure>(to);
             return isCopyStructureCompatible(xxx,yyy);
        }
    case structureArray:
        {
             StructureArrayConstPtr xxx = static_pointer_cast<const StructureArray>(from);
             StructureArrayConstPtr yyy = static_pointer_cast<const StructureArray>(to);
             return isCopyStructureArrayCompatible(xxx,yyy);
        }
    case union_:
        {
             UnionConstPtr xxx = static_pointer_cast<const Union>(from);
             UnionConstPtr yyy = static_pointer_cast<const Union>(to);
             return isCopyUnionCompatible(xxx,yyy);
        }
    case unionArray:
        {
             UnionArrayConstPtr xxx = static_pointer_cast<const UnionArray>(from);
             UnionArrayConstPtr yyy = static_pointer_cast<const UnionArray>(to);
             return isCopyUnionArrayCompatible(xxx,yyy);
        }
    }
    string message("Convert::isCopyCompatible should never get here");
    throw std::logic_error(message);
}

void Convert::copy(PVFieldPtr const & from, PVFieldPtr const & to)
{
    switch(from->getField()->getType()) {
    case scalar: 
        {
             PVScalarPtr xxx = static_pointer_cast<PVScalar>(from);
             PVScalarPtr yyy = static_pointer_cast<PVScalar>(to);
             copyScalar(xxx,yyy);
             return;
        }
    case scalarArray:
        {
             PVScalarArrayPtr fromArray = static_pointer_cast<PVScalarArray>(from);
             PVScalarArrayPtr toArray = static_pointer_cast<PVScalarArray>(to);
             toArray->assign(*fromArray.get());
             return;
        }
    case structure:
        {
             PVStructurePtr xxx = static_pointer_cast<PVStructure>(from);
             PVStructurePtr yyy = static_pointer_cast<PVStructure>(to);
             copyStructure(xxx,yyy);
             return;
        }
    case structureArray: {
            PVStructureArrayPtr fromArray = static_pointer_cast<PVStructureArray>(from);
            PVStructureArrayPtr toArray = static_pointer_cast<PVStructureArray>(to);
            copyStructureArray(fromArray,toArray);
            return;
        }
    case union_:
        {
             PVUnionPtr xxx = static_pointer_cast<PVUnion>(from);
             PVUnionPtr yyy = static_pointer_cast<PVUnion>(to);
             copyUnion(xxx,yyy);
             return;
        }
    case unionArray: {
            PVUnionArrayPtr fromArray = static_pointer_cast<PVUnionArray>(from);
            PVUnionArrayPtr toArray = static_pointer_cast<PVUnionArray>(to);
            copyUnionArray(fromArray,toArray);
            return;
        }
    }
}

bool Convert::isCopyScalarCompatible(
     ScalarConstPtr const & fromField, ScalarConstPtr const & toField)
{
    ScalarType fromScalarType = fromField->getScalarType();
    ScalarType toScalarType = toField->getScalarType();
    if(fromScalarType==toScalarType) return true;
    if(ScalarTypeFunc::isNumeric(fromScalarType)
    && ScalarTypeFunc::isNumeric(toScalarType)) return true;
    if(fromScalarType==pvString) return true;
    if(toScalarType==pvString) return true;
    return false;
}

void Convert::copyScalar(PVScalarPtr const & from, PVScalarPtr const & to)
{
    if(to->isImmutable()) {
        if(from==to) return;
        string message("Convert.copyScalar destination is immutable");
        throw std::invalid_argument(message);
    }
    to->assign(*from.get());
}

bool Convert::isCopyScalarArrayCompatible(ScalarArrayConstPtr const &fromArray,
    ScalarArrayConstPtr const &toArray)
{
    ScalarType fromType = fromArray->getElementType();
    ScalarType toType = toArray->getElementType();
    if(fromType==toType) return true;
    if(ScalarTypeFunc::isNumeric(fromType)
    && ScalarTypeFunc::isNumeric(toType)) return true;
    if(toType==pvString) return true;
    if(fromType==pvString) return true;
    return false;
}

bool Convert::isCopyStructureCompatible(
    StructureConstPtr const &fromStruct, StructureConstPtr const &toStruct)
{
    FieldConstPtrArray const & fromFields = fromStruct->getFields();
    FieldConstPtrArray const & toFields = toStruct->getFields();
    size_t length = fromStruct->getNumberFields();
    if(length!=toStruct->getNumberFields()) return false;
    for(size_t i=0; i<length; i++) {
        FieldConstPtr from = fromFields[i];
        FieldConstPtr to = toFields[i];
        Type fromType = from->getType();
        Type toType = to->getType();
        if(fromType!=toType) return false;
        switch(fromType) {
        case scalar:
            {
		ScalarConstPtr xxx = static_pointer_cast<const Scalar>(from);
                ScalarConstPtr yyy = static_pointer_cast<const Scalar>(to);
                if(!isCopyScalarCompatible(xxx,yyy)) return false;
            }
            break;
        case scalarArray:
            {
		ScalarArrayConstPtr xxx = static_pointer_cast<const ScalarArray>(from);
                ScalarArrayConstPtr yyy = static_pointer_cast<const ScalarArray>(to);
                if(!isCopyScalarArrayCompatible(xxx,yyy)) return false;
            }
            break;
        case structure:
            {
		StructureConstPtr xxx = static_pointer_cast<const Structure>(from);
                StructureConstPtr yyy = static_pointer_cast<const Structure>(to);
                if(!isCopyStructureCompatible(xxx,yyy)) return false;
            }
            break;
        case structureArray:
            {
		StructureArrayConstPtr xxx = static_pointer_cast<const StructureArray>(from);
                StructureArrayConstPtr yyy = static_pointer_cast<const StructureArray>(to);
                if(!isCopyStructureArrayCompatible(xxx,yyy)) return false;
            }
        case union_:
            {
		UnionConstPtr xxx = static_pointer_cast<const Union>(from);
                UnionConstPtr yyy = static_pointer_cast<const Union>(to);
                if(!isCopyUnionCompatible(xxx,yyy)) return false;
            }
            break;
        case unionArray:
            {
		UnionArrayConstPtr xxx = static_pointer_cast<const UnionArray>(from);
                UnionArrayConstPtr yyy = static_pointer_cast<const UnionArray>(to);
                if(!isCopyUnionArrayCompatible(xxx,yyy)) return false;
            }
            break;
        }
    }
    return true;
}

void Convert::copyStructure(PVStructurePtr const & from, PVStructurePtr const & to)
{
    if(to->isImmutable()) {
        if(from==to) return;
        throw std::invalid_argument("Convert.copyStructure destination is immutable");
    }
    if(from==to) return;
    PVFieldPtrArray const & fromDatas = from->getPVFields();
    PVFieldPtrArray const & toDatas = to->getPVFields();
    if(from->getStructure()->getNumberFields() 
    != to->getStructure()->getNumberFields()) {
        string message("Convert.copyStructure Illegal copyStructure");
        throw std::invalid_argument(message);
    }
    size_t numberFields = from->getStructure()->getNumberFields();
    if(numberFields>=2) {
        string name0 = fromDatas[0]->getFieldName();
        string name1 = fromDatas[1]->getFieldName();
        // look for enumerated structure and copy choices first
        if(name0.compare("index")==0 && name1.compare("choices")==0) {
            FieldConstPtr fieldIndex = fromDatas[0]->getField();
            FieldConstPtr fieldChoices = fromDatas[1]->getField();
            if(fieldIndex->getType()==scalar
            && fieldChoices->getType()==scalarArray) {
                PVScalarPtr pvScalar = static_pointer_cast<PVScalar>(fromDatas[0]);
                PVScalarArrayPtr pvArray =
                    static_pointer_cast<PVScalarArray>(fromDatas[1]);
                if((pvScalar->getScalar()->getScalarType()==pvInt)
                && (pvArray->getScalarArray()->getElementType()==pvString)) {
                   PVScalarArrayPtr toArray = 
                       static_pointer_cast<PVScalarArray>(toDatas[1]);
                   toArray->assign(*pvArray.get());
                   PVScalarPtr toScalar = static_pointer_cast<PVScalar>(toDatas[0]);
                   copyScalar(pvScalar,toScalar);
                   return;
                }
            }
        }
    }
    for(size_t i=0; i < numberFields; i++) {
        PVFieldPtr fromData = fromDatas[i];
        PVFieldPtr toData = toDatas[i];
        Type fromType = fromData->getField()->getType();
        Type toType = toData->getField()->getType();
        if(fromType!=toType) {
            string message("Convert.copyStructure Illegal copyStructure");
            throw std::invalid_argument(message);
        }
        if(toData->isImmutable()) {
            if(fromData==toData) return;
            throw std::invalid_argument("Convert.copyStructure destination is immutable");
        }
        switch(fromType) {
        case scalar:
            {
                PVScalarPtr xxx = static_pointer_cast<PVScalar>(fromData);
                PVScalarPtr yyy = static_pointer_cast<PVScalar>(toData);
                copyScalar(xxx,yyy);
                break;
            }
        case scalarArray:
            {
                PVScalarArrayPtr fromArray = static_pointer_cast<PVScalarArray>(fromData);
                PVScalarArrayPtr toArray = static_pointer_cast<PVScalarArray>(toData);
                toArray->assign(*fromArray.get());
                break;
            }
        case structure:
            {
                PVStructurePtr xxx = static_pointer_cast<PVStructure>(fromData);
                PVStructurePtr yyy = static_pointer_cast<PVStructure>(toData);
                copyStructure(xxx,yyy);
                break;
            }
        case structureArray:
            {
                PVStructureArrayPtr fromArray = 
                    static_pointer_cast<PVStructureArray>(fromData);
                PVStructureArrayPtr toArray = 
                    static_pointer_cast<PVStructureArray>(toData);
                copyStructureArray(fromArray,toArray);
                break;
            }
        case union_:
            {
                PVUnionPtr xxx = static_pointer_cast<PVUnion>(fromData);
                PVUnionPtr yyy = static_pointer_cast<PVUnion>(toData);
                copyUnion(xxx,yyy);
                break;
            }
        case unionArray:
            {
                PVUnionArrayPtr fromArray = 
                    static_pointer_cast<PVUnionArray>(fromData);
                PVUnionArrayPtr toArray = 
                    static_pointer_cast<PVUnionArray>(toData);
                copyUnionArray(fromArray,toArray);
                break;
            }
        }
    }
}

bool Convert::isCopyUnionCompatible(
    UnionConstPtr const &from, UnionConstPtr const &to)
{
    return *(from.get()) == *(to.get());
}

static PVDataCreatePtr pvDataCreate = getPVDataCreate();

void Convert::copyUnion(PVUnionPtr const & from, PVUnionPtr const & to)
{
    if(to->isImmutable()) {
        if(from==to) return;
        throw std::invalid_argument("Convert.copyUnion destination is immutable");
    }
    if(from==to) return;
    if(!isCopyUnionCompatible(from->getUnion(), to->getUnion())) {
        throw std::invalid_argument("Illegal copyUnion");
    }
            
    PVFieldPtr fromValue = from->get();
    if (from->getUnion()->isVariant())
    {
        if (fromValue.get() == 0)
            to->set(PVField::shared_pointer());
        else
        {
            PVFieldPtr toValue = to->get();
            if (toValue.get() == 0 || *toValue->getField() != *fromValue->getField())
            {
                toValue = pvDataCreate->createPVField(fromValue->getField());
                to->set(toValue);
            }
            copy(fromValue, toValue);
        }
    }
    else
    {
        if (fromValue.get() == 0)
            to->select(PVUnion::UNDEFINED_INDEX);
        else
        {
            copy(fromValue, to->select(from->getSelectedIndex()));
        }
    }
}

bool Convert::isCopyStructureArrayCompatible(
    StructureArrayConstPtr const &from, StructureArrayConstPtr const &to)
{
    StructureConstPtr xxx = from->getStructure();
    StructureConstPtr yyy = to->getStructure();
    return isCopyStructureCompatible(xxx,yyy);
}

void Convert::copyStructureArray(
    PVStructureArrayPtr const & from, PVStructureArrayPtr const & to)
{
    if(from==to) {
        return;
    } else if(to->isImmutable()) {
        throw std::invalid_argument("Convert.copyStructureArray destination is immutable");
    }
    to->replace(from->view());
}

bool Convert::isCopyUnionArrayCompatible(
    UnionArrayConstPtr const &from, UnionArrayConstPtr const &to)
{
    UnionConstPtr xxx = from->getUnion();
    UnionConstPtr yyy = to->getUnion();
    return isCopyUnionCompatible(xxx,yyy);
}

void Convert::copyUnionArray(
    PVUnionArrayPtr const & from, PVUnionArrayPtr const & to)
{
    if(from==to) {
        return;
    } else if(to->isImmutable()) {
        throw std::invalid_argument("Convert.copyUnionArray destination is immutable");
    }
    to->replace(from->view());
}

void Convert::newLine(string *buffer, int indentLevel)
{
    *buffer += "\n";
    *buffer += string(indentLevel*4, ' ');
}

ConvertPtr Convert::getConvert()
{
    static ConvertPtr convert;
    static Mutex mutex;
    Lock xx(mutex);

    if(convert.get()==0) {
        convert = ConvertPtr(new Convert());
    }
    return convert;
}

}}
