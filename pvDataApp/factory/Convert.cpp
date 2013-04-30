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
#include <pv/lock.h>
#include <pv/pvIntrospect.h>
#include <pv/pvData.h>
#include <pv/convert.h>
#include <pv/printer.h>

using std::tr1::static_pointer_cast;
using std::size_t;

namespace epics { namespace pvData { 


static void newLineImpl(StringBuilder buffer, int indentLevel)
{
    *buffer += "\n";
    for(int i=0; i<indentLevel; i++) *buffer += "    ";
}

static size_t convertFromByteArray(PVScalarArray * pv, size_t offset,
    size_t len,const int8 from[], size_t fromOffset);
static size_t convertToByteArray(PVScalarArray *pv, size_t offset,
    size_t len,int8 to[], size_t toOffset);
static size_t convertFromShortArray(PVScalarArray *pv, size_t offset,
    size_t len,const int16 from[], size_t fromOffset);
static size_t convertToShortArray(PVScalarArray *pv, size_t offset,
    size_t len,int16 to[], size_t toOffset);
static size_t convertFromIntArray(PVScalarArray *pv, size_t offset,
    size_t len,const int32 from[], size_t fromOffset);
static size_t convertToIntArray(PVScalarArray *pv, size_t offset,
    size_t len,int32 to[], size_t toOffset);
static size_t convertFromLongArray(PVScalarArray *pv, size_t offset,
    size_t len,const int64 from[], size_t fromOffset);
static size_t convertToLongArray(PVScalarArray * pv, size_t offset,
    size_t len,int64 to[], size_t toOffset);
static size_t convertFromUByteArray(PVScalarArray * pv, size_t offset,
    size_t len,const uint8 from[], size_t fromOffset);
static size_t convertToUByteArray(PVScalarArray *pv, size_t offset,
    size_t len,uint8 to[], size_t toOffset);
static size_t convertFromUShortArray(PVScalarArray *pv, size_t offset,
    size_t len,const uint16 from[], size_t fromOffset);
static size_t convertToUShortArray(PVScalarArray *pv, size_t offset,
    size_t len,uint16 to[], size_t toOffset);
static size_t convertFromUIntArray(PVScalarArray *pv, size_t offset,
    size_t len,const uint32 from[], size_t fromOffset);
static size_t convertToUIntArray(PVScalarArray *pv, size_t offset,
    size_t len,uint32 to[], size_t toOffset);
static size_t convertFromULongArray(PVScalarArray *pv, size_t offset,
    size_t len,const uint64 from[], size_t fromOffset);
static size_t convertToULongArray(PVScalarArray * pv, size_t offset,
    size_t len,uint64 to[], size_t toOffset);
static size_t convertFromFloatArray(PVScalarArray *pv, size_t offset,
    size_t len,const float from[], size_t fromOffset);
static size_t convertToFloatArray(PVScalarArray * pv, size_t offset,
    size_t len,float to[], size_t toOffset);
static size_t convertFromDoubleArray(PVScalarArray *pv, size_t offset,
    size_t len,const double from[], size_t fromOffset);
static size_t convertToDoubleArray(PVScalarArray *pv, size_t offset,
    size_t len,double to[], size_t toOffset);


static std::vector<String> split(String commaSeparatedList);

static std::vector<String> split(String commaSeparatedList) {
    String::size_type numValues = 1;
    String::size_type index=0;
    while(true) {
        String::size_type pos = commaSeparatedList.find(',',index);
        if(pos==String::npos) break;
        numValues++;
        index = pos +1;
    }
    std::vector<String> valueList(numValues,"");
    index=0;
    for(size_t i=0; i<numValues; i++) {
        size_t pos = commaSeparatedList.find(',',index);
        String value = commaSeparatedList.substr(index,pos);
        valueList[i] = value;
        index = pos +1;
    }
    return valueList;
}
    
Convert::Convert()
: pvDataCreate(getPVDataCreate()),
  trueString("true"),
  falseString("false"),
  illegalScalarType("Illegal ScalarType")
{}



Convert::~Convert(){}

void Convert::getString(StringBuilder buf,PVField const *pvField,int indentLevel)
{
    // TODO indextLevel ignored
    std::ostringstream strm;
    PrinterPlain p;
    p.setStream(strm);
    p.print(*pvField);
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
                // structureArray not supported
                String message("Convert::fromString unsupported fieldType ");
                TypeFunc::toString(&message,type);
                throw std::logic_error(message);
            }
        }
    }
    
    return processed;
}

size_t Convert::fromString(PVScalarArrayPtr const &pv, String from)
{
   if(from[0]=='[' && from[from.length()]==']') {
        size_t offset = from.rfind(']');
        from = from.substr(1, offset);
    }
    std::vector<String> valueList(split(from));
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
    if(fromOffset>alen) return 0;
    alen -= fromOffset;
    if(length>alen) length=alen;
    pv->putFrom<pvString>(&from[fromOffset], length, offset);
    return length;
}

size_t Convert::toStringArray(PVScalarArrayPtr const & pv,
                              size_t offset, size_t length,
                              StringArray  &to, size_t toOffset)
{
    size_t alen = pv->getLength();
    if(offset>alen) return 0;
    alen -= offset;
    if(length>alen) length=alen;
    pv->getAs<pvString>(&to[toOffset], length, offset);
    return length;
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
    }
    String message("Convert::isCopyCompatible should never get here");
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
        String message("Convert.copyScalar destination is immutable");
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
    FieldConstPtrArray fromFields = fromStruct->getFields();
    FieldConstPtrArray toFields = toStruct->getFields();
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
            break;
        }
    }
    return true;
}

void Convert::copyStructure(PVStructurePtr const & from, PVStructurePtr const & to)
{
    if(to->isImmutable()) {
        if(from==to) return;
        String message("Convert.copyStructure destination is immutable");
        throw std::invalid_argument(message);
    }
    if(from==to) return;
    PVFieldPtrArray const & fromDatas = from->getPVFields();
    PVFieldPtrArray const & toDatas = to->getPVFields();
    if(from->getStructure()->getNumberFields() 
    != to->getStructure()->getNumberFields()) {
        String message("Convert.copyStructure Illegal copyStructure");
        throw std::invalid_argument(message);
    }
    size_t numberFields = from->getStructure()->getNumberFields();
    if(numberFields>=2) {
        String name0 = fromDatas[0]->getFieldName();
        String name1 = fromDatas[1]->getFieldName();
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
            String message("Convert.copyStructure Illegal copyStructure");
            throw std::invalid_argument(message);
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
    if(to->isImmutable()) {
        if(from==to) return;
        String message("Convert.copyStructureArray destination is immutable");
        throw std::invalid_argument(message);
    }
    to->put(0,from->getLength(),from->getVector(),0);
}

size_t Convert::toByteArray(PVScalarArrayPtr const &pv, size_t offset, size_t length,
    int8 to[], size_t toOffset)
{
    return convertToByteArray(pv.get(), offset, length, to, toOffset);
}

size_t Convert::toShortArray(PVScalarArrayPtr const &pv, size_t offset, size_t length,
    int16 to[], size_t toOffset)
{
    return convertToShortArray(pv.get(), offset, length, to, toOffset);
}

size_t Convert::toIntArray(PVScalarArrayPtr const &pv, size_t offset, size_t length,
    int32 to[], size_t toOffset)
{
    return convertToIntArray(pv.get(), offset, length, to, toOffset);
}

size_t Convert::toLongArray(PVScalarArrayPtr const &pv, size_t offset, size_t length,
    int64 to[], size_t toOffset)
{
    return convertToLongArray(pv.get(), offset, length, to, toOffset);
}

size_t Convert::toUByteArray(PVScalarArrayPtr const &pv, size_t offset, size_t length,
    uint8 to[], size_t toOffset)
{
    return convertToUByteArray(pv.get(), offset, length, to, toOffset);
}

size_t Convert::toUShortArray(PVScalarArrayPtr const &pv, size_t offset, size_t length,
    uint16 to[], size_t toOffset)
{
    return convertToUShortArray(pv.get(), offset, length, to, toOffset);
}

size_t Convert::toUIntArray(PVScalarArrayPtr const &pv, size_t offset, size_t length,
    uint32 to[], size_t toOffset)
{
    return convertToUIntArray(pv.get(), offset, length, to, toOffset);
}

size_t Convert::toULongArray(PVScalarArrayPtr const &pv, size_t offset, size_t length,
    uint64 to[], size_t toOffset)
{
    return convertToULongArray(pv.get(), offset, length, to, toOffset);
}

size_t Convert::toFloatArray(PVScalarArrayPtr const &pv, size_t offset, size_t length,
    float to[], size_t toOffset)
{
    return convertToFloatArray(pv.get(), offset, length, to, toOffset);
}

size_t Convert::toDoubleArray(PVScalarArrayPtr const &pv, size_t offset, size_t length,
    double to[], size_t toOffset)
{
    return convertToDoubleArray(pv.get(), offset, length, to, toOffset);
}

size_t Convert::fromByteArray(PVScalarArrayPtr &pv, size_t offset, size_t length,
    const int8 from[], size_t fromOffset)
{
    return convertFromByteArray(pv.get(), offset, length, from, fromOffset);
}

size_t Convert::fromByteArray(PVScalarArrayPtr &pv, size_t offset, size_t length,
    const ByteArray & from, size_t fromOffset)
{
    return convertFromByteArray(pv.get(), offset, length, get(from), fromOffset);
}

size_t Convert::fromShortArray(PVScalarArrayPtr &pv, size_t offset, size_t length,
    const int16 from[], size_t fromOffset)
{
    return convertFromShortArray(pv.get(), offset, length, from, fromOffset);
}

size_t Convert::fromShortArray(PVScalarArrayPtr &pv, size_t offset, size_t length,
    const ShortArray & from, size_t fromOffset)
{
    return convertFromShortArray(pv.get(), offset, length, get(from), fromOffset);
}

size_t Convert::fromIntArray(PVScalarArrayPtr &pv, size_t offset, size_t length,
    const int32 from[], size_t fromOffset)
{
    return convertFromIntArray(pv.get(), offset, length, from, fromOffset);
}

size_t Convert::fromIntArray(PVScalarArrayPtr &pv, size_t offset, size_t length,
    const IntArray & from, size_t fromOffset)
{
    return convertFromIntArray(pv.get(), offset, length, get(from), fromOffset);
}

size_t Convert::fromLongArray(PVScalarArrayPtr &pv, size_t offset, size_t length,
    const int64 from[], size_t fromOffset)
{
    return convertFromLongArray(pv.get(), offset, length, from, fromOffset);
}

size_t Convert::fromLongArray(PVScalarArrayPtr &pv, size_t offset, size_t length,
    const LongArray & from, size_t fromOffset)
{
    return convertFromLongArray(pv.get(), offset, length, get(from), fromOffset);
}

size_t Convert::fromUByteArray(PVScalarArrayPtr &pv, size_t offset, size_t length,
    const uint8 from[], size_t fromOffset)
{
    return convertFromUByteArray(pv.get(), offset, length, from, fromOffset);
}

size_t Convert::fromUByteArray(PVScalarArrayPtr &pv, size_t offset, size_t length,
    const UByteArray & from, size_t fromOffset)
{
    return convertFromUByteArray(pv.get(), offset, length, get(from), fromOffset);
}

size_t Convert::fromUShortArray(PVScalarArrayPtr &pv, size_t offset, size_t length,
    const uint16 from[], size_t fromOffset)
{
    return convertFromUShortArray(pv.get(), offset, length, from, fromOffset);
}

size_t Convert::fromUShortArray(PVScalarArrayPtr &pv, size_t offset, size_t length,
    const UShortArray &from, size_t fromOffset)
{
    return convertFromUShortArray(pv.get(), offset, length, get(from), fromOffset);
}

size_t Convert::fromUIntArray(PVScalarArrayPtr &pv, size_t offset, size_t length,
    const uint32 from[], size_t fromOffset)
{
    return convertFromUIntArray(pv.get(), offset, length, from, fromOffset);
}

size_t Convert::fromUIntArray(PVScalarArrayPtr &pv, size_t offset, size_t length,
    const UIntArray & from, size_t fromOffset)
{
    return convertFromUIntArray(pv.get(), offset, length, get(from), fromOffset);
}

size_t Convert::fromULongArray(PVScalarArrayPtr &pv, size_t offset, size_t length,
    const uint64 from[], size_t fromOffset)
{
    return convertFromULongArray(pv.get(), offset, length, from, fromOffset);
}

size_t Convert::fromULongArray(PVScalarArrayPtr &pv, size_t offset, size_t length,
    const ULongArray &from, size_t fromOffset)
{
    return convertFromULongArray(pv.get(), offset, length, get(from), fromOffset);
}

size_t Convert::fromFloatArray(PVScalarArrayPtr &pv, size_t offset, size_t length,
    const float from[], size_t fromOffset)
{
    return convertFromFloatArray(pv.get(), offset, length, from, fromOffset);
}

size_t Convert::fromFloatArray(PVScalarArrayPtr &pv, size_t offset, size_t length,
    const FloatArray & from, size_t fromOffset)
{
    return convertFromFloatArray(pv.get(), offset, length, get(from), fromOffset);
}

size_t Convert::fromDoubleArray(PVScalarArrayPtr &pv, size_t offset, size_t length,
    const double from[], size_t fromOffset)
{
    return convertFromDoubleArray(pv.get(), offset, length, from, fromOffset);
}

size_t Convert::fromDoubleArray(PVScalarArrayPtr &pv, size_t offset, size_t length,
    const DoubleArray & from, size_t fromOffset)
{
    return convertFromDoubleArray(pv.get(), offset, length, get(from), fromOffset);
}

void Convert::newLine(StringBuilder buffer, int indentLevel)
{
    newLineImpl(buffer,indentLevel);
}

template<typename PVT,typename T>
size_t convertFromScalarArray(PVScalarArray *pv,
    size_t offset, size_t len,const T from[], size_t fromOffset)
{
    ScalarType elemType = pv->getScalarArray()->getElementType();
    size_t ntransfered = 0;
    String xx = typeid(PVT).name();
    String yy = typeid(T).name();
    if(xx.compare(yy)== 0) {
        PVT *pvdata = static_cast<PVT*>(pv);
        while (len > 0) {
            size_t n = pvdata->put(offset, len, from, fromOffset);
            if (n == 0)
                break;
            len -= n;
            offset += n;
            fromOffset += n;
            ntransfered += n;
        }
        return ntransfered;
    }
    switch (elemType) {
    case pvBoolean: {
         String message("convert from numeric array to BooleanArray not legal");
         throw std::invalid_argument(message);
    }
    case pvByte: {
        PVByteArray *pvdata = static_cast<PVByteArray*>(pv);
        int8 data[1];
        while (len > 0) {
            data[0] = static_cast<int8>(from[fromOffset]);
            if (pvdata->put(offset, 1, data, 0) == 0)
                return ntransfered;
            --len;
            ++ntransfered;
            ++offset;
            ++fromOffset;
        }
        return ntransfered;
    }
    case pvShort: {
        PVShortArray *pvdata = static_cast<PVShortArray*>(pv);
        int16 data[1];
        while (len > 0) {
            data[0] = static_cast<int16>(from[fromOffset]);
            if (pvdata->put(offset, 1, data, 0) == 0)
                return ntransfered;
            --len;
            ++ntransfered;
            ++offset;
            ++fromOffset;
        }
        return ntransfered;
    }
    case pvInt: {
        PVIntArray *pvdata = static_cast<PVIntArray*>(pv);
        int32 data[1];
        while (len > 0) {
            data[0] = static_cast<int32>(from[fromOffset]);
            if (pvdata->put(offset, 1, data, 0) == 0)
                return ntransfered;
            --len;
            ++ntransfered;
            ++offset;
            ++fromOffset;
        }
        return ntransfered;
    }
    case pvLong: {
        PVLongArray *pvdata = static_cast<PVLongArray*>(pv);
        int64 data[1];
        while (len > 0) {
            data[0] = static_cast<int64>(from[fromOffset]);
            if (pvdata->put(offset, 1, data, 0) == 0)
                return ntransfered;
            --len;
            ++ntransfered;
            ++offset;
            ++fromOffset;
        }
        return ntransfered;
    }
    case pvUByte: {
        PVUByteArray *pvdata = static_cast<PVUByteArray*>(pv);
        uint8 data[1];
        while (len > 0) {
            data[0] = static_cast<uint8>(from[fromOffset]);
            if (pvdata->put(offset, 1, data, 0) == 0)
                return ntransfered;
            --len;
            ++ntransfered;
            ++offset;
            ++fromOffset;
        }
        return ntransfered;
    }
    case pvUShort: {
        PVUShortArray *pvdata = static_cast<PVUShortArray*>(pv);
        uint16 data[1];
        while (len > 0) {
            data[0] = static_cast<uint16>(from[fromOffset]);
            if (pvdata->put(offset, 1, data, 0) == 0)
                return ntransfered;
            --len;
            ++ntransfered;
            ++offset;
            ++fromOffset;
        }
        return ntransfered;
    }
    case pvUInt: {
        PVUIntArray *pvdata = static_cast<PVUIntArray*>(pv);
        uint32 data[1];
        while (len > 0) {
            data[0] = static_cast<uint32>(from[fromOffset]);
            if (pvdata->put(offset, 1, data, 0) == 0)
                return ntransfered;
            --len;
            ++ntransfered;
            ++offset;
            ++fromOffset;
        }
        return ntransfered;
    }
    case pvULong: {
        PVULongArray *pvdata = static_cast<PVULongArray*>(pv);
        uint64 data[1];
        while (len > 0) {
            data[0] = static_cast<uint64>(from[fromOffset]);
            if (pvdata->put(offset, 1, data, 0) == 0)
                return ntransfered;
            --len;
            ++ntransfered;
            ++offset;
            ++fromOffset;
        }
        return ntransfered;
    }
    case pvFloat: {
        PVFloatArray *pvdata = static_cast<PVFloatArray*>(pv);
        float data[1];
        while (len > 0) {
            data[0] = static_cast<float>(from[fromOffset]);
            if (pvdata->put(offset, 1, data, 0) == 0)
                return ntransfered;
            --len;
            ++ntransfered;
            ++offset;
            ++fromOffset;
        }
        return ntransfered;
    }
    case pvDouble: {
        PVDoubleArray *pvdata = static_cast<PVDoubleArray*>(pv);
        double data[1];
        while (len > 0) {
            data[0] = static_cast<double>(from[fromOffset]);
            if (pvdata->put(offset, 1, data, 0) == 0)
                return ntransfered;
            --len;
            ++ntransfered;
            ++offset;
            ++fromOffset;
        }
        return ntransfered;
    }
    case pvString: {
         String message("convert from byte[] to StringArray not legal");
         throw std::invalid_argument(message);
    }
    }
    String message("Convert::convertFromByteArray should never get here");
    throw std::logic_error(message);
}

template<typename T>
size_t convertToScalarArray(PVScalarArray *pv,
    size_t offset, size_t len,T to[], size_t toOffset)
{
    ScalarType elemType = pv->getScalarArray()->getElementType();
    size_t ntransfered = 0;
    switch (elemType) {
    case pvBoolean: {
        String message("convert BooleanArray to byte[]] not legal");
        throw std::invalid_argument(message);
    }
    case pvByte: {
        PVByteArray *pvdata = static_cast<PVByteArray*>(pv);
        ByteArrayData data;
        while (len > 0) {
            size_t num = 0;
            num = pvdata->get(offset,len,data);
            if (num <= 0) break;
            ByteArray & dataArray = data.data;
            size_t dataOffset = data.offset;
            for(size_t i=0;i<num;i++) to[toOffset+i] = dataArray[dataOffset+1];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvShort: {
        PVShortArray *pvdata = static_cast<PVShortArray*>(pv);
        ShortArrayData data;
        while (len > 0) {
            size_t num = pvdata->get(offset, len, data);
            if (num == 0) break;
            ShortArray & dataArray = data.data;
            size_t dataOffset = data.offset;
            for (size_t i = 0; i < num; i++)
                to[i + toOffset] =  dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvInt: {
        PVIntArray *pvdata = static_cast<PVIntArray*>(pv);
        IntArrayData data;
        while (len > 0) {
            size_t num = pvdata->get(offset, len, data);
            if (num == 0) break;
            IntArray & dataArray = data.data;
            size_t dataOffset = data.offset;
            for (size_t i = 0; i < num; i++)
                to[i + toOffset] =  dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvLong: {
        PVLongArray *pvdata = static_cast<PVLongArray*>(pv);
        LongArrayData data;
        while (len > 0) {
            size_t num = pvdata->get(offset, len, data);
            if (num == 0)
                break;
            LongArray & dataArray = data.data;
            size_t dataOffset = data.offset;
            for (size_t i = 0; i < num; i++)
                to[i + toOffset] = dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvUByte: {
        PVUByteArray *pvdata = static_cast<PVUByteArray*>(pv);
        UByteArrayData data;
        while (len > 0) {
            size_t num = 0;
            num = pvdata->get(offset,len,data);
            if (num <= 0) break;
            UByteArray & dataArray = data.data;
            size_t dataOffset = data.offset;
            for(size_t i=0;i<num;i++) to[toOffset+i] = dataArray[dataOffset+1];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvUShort: {
        PVUShortArray *pvdata = static_cast<PVUShortArray*>(pv);
        UShortArrayData data;
        while (len > 0) {
            size_t num = pvdata->get(offset, len, data);
            if (num == 0) break;
            UShortArray & dataArray = data.data;
            size_t dataOffset = data.offset;
            for (size_t i = 0; i < num; i++)
                to[i + toOffset] =  dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvUInt: {
        PVUIntArray *pvdata = static_cast<PVUIntArray*>(pv);
        UIntArrayData data;
        while (len > 0) {
            size_t num = pvdata->get(offset, len, data);
            if (num == 0) break;
            UIntArray & dataArray = data.data;
            size_t dataOffset = data.offset;
            for (size_t i = 0; i < num; i++)
                to[i + toOffset] =  dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvULong: {
        PVULongArray *pvdata = static_cast<PVULongArray*>(pv);
        ULongArrayData data;
        while (len > 0) {
            size_t num = pvdata->get(offset, len, data);
            if (num == 0)
                break;
            ULongArray & dataArray = data.data;
            size_t dataOffset = data.offset;
            for (size_t i = 0; i < num; i++)
                to[i + toOffset] = dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvFloat: {
        PVFloatArray *pvdata = static_cast<PVFloatArray*>(pv);
        FloatArrayData data;
        while (len > 0) {
            size_t num = pvdata->get(offset, len, data);
            if (num == 0) break;
            FloatArray & dataArray = data.data;
            size_t dataOffset = data.offset;
            for (size_t i = 0; i < num; i++)
                to[i + toOffset] = static_cast<T>(dataArray[i + dataOffset]);
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvDouble: {
        PVDoubleArray *pvdata = static_cast<PVDoubleArray*>(pv);
        DoubleArrayData data;
        while (len > 0) {
            size_t num = pvdata->get(offset, len, data);
            if (num == 0)
                break;
            DoubleArray & dataArray = data.data;
            size_t dataOffset = data.offset;
            for (size_t i = 0; i < num; i++)
                to[i + toOffset] = static_cast<T>(dataArray[i + dataOffset]);
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvString: {
        String message("convert StringArray to numeric not legal");
        throw std::invalid_argument(message);
    }
    }
     String message("Convert::convertToScalarArray should never get here");
    throw std::logic_error(message);
}

size_t convertFromByteArray(PVScalarArray *pv,
    size_t offset, size_t len,const int8 from[], size_t fromOffset)
{
    return convertFromScalarArray<PVByteArray,int8>(pv,offset,len,from,fromOffset);
}

size_t convertToByteArray(PVScalarArray * pv,
    size_t offset, size_t len,int8 to[], size_t toOffset)
{
    return convertToScalarArray<int8>(pv,offset,len,to,toOffset);
}

size_t convertFromShortArray(PVScalarArray *pv,
    size_t offset, size_t len,const int16 from[], size_t fromOffset)
{
    return convertFromScalarArray<PVShortArray,int16>(pv,offset,len,from,fromOffset);
}

size_t convertToShortArray(PVScalarArray * pv,
    size_t offset, size_t len,int16 to[], size_t toOffset)
{
    return convertToScalarArray<int16>(pv,offset,len,to,toOffset);
}

size_t convertFromIntArray(PVScalarArray *pv,
    size_t offset, size_t len,const int32 from[], size_t fromOffset)
{
    return convertFromScalarArray<PVIntArray,int32>(pv,offset,len,from,fromOffset);
}

size_t convertToIntArray(PVScalarArray * pv,
    size_t offset, size_t len,int32 to[], size_t toOffset)
{
    return convertToScalarArray<int32>(pv,offset,len,to,toOffset);
}

size_t convertFromLongArray(PVScalarArray *pv,
    size_t offset, size_t len,const int64 from[], size_t fromOffset)
{
    return convertFromScalarArray<PVLongArray,int64>(pv,offset,len,from,fromOffset);
}

size_t convertToLongArray(PVScalarArray * pv,
    size_t offset, size_t len,int64 to[], size_t toOffset)
{
    return convertToScalarArray<int64>(pv,offset,len,to,toOffset);
}

size_t convertFromUByteArray(PVScalarArray *pv,
    size_t offset, size_t len,const uint8 from[], size_t fromOffset)
{
    return convertFromScalarArray<PVUByteArray,uint8>(pv,offset,len,from,fromOffset);
}

size_t convertToUByteArray(PVScalarArray * pv,
    size_t offset, size_t len,uint8 to[], size_t toOffset)
{
    return convertToScalarArray<uint8>(pv,offset,len,to,toOffset);
}

size_t convertFromUShortArray(PVScalarArray *pv,
    size_t offset, size_t len,const uint16 from[], size_t fromOffset)
{
    return convertFromScalarArray<PVUShortArray,uint16>(pv,offset,len,from,fromOffset);
}

size_t convertToUShortArray(PVScalarArray * pv,
    size_t offset, size_t len,uint16 to[], size_t toOffset)
{
    return convertToScalarArray<uint16>(pv,offset,len,to,toOffset);
}

size_t convertFromUIntArray(PVScalarArray *pv,
    size_t offset, size_t len,const uint32 from[], size_t fromOffset)
{
    return convertFromScalarArray<PVUIntArray,uint32>(pv,offset,len,from,fromOffset);
}

size_t convertToUIntArray(PVScalarArray * pv,
    size_t offset, size_t len,uint32 to[], size_t toOffset)
{
    return convertToScalarArray<uint32>(pv,offset,len,to,toOffset);
}

size_t convertFromULongArray(PVScalarArray *pv,
    size_t offset, size_t len,const uint64 from[], size_t fromOffset)
{
    return convertFromScalarArray<PVULongArray,uint64>(pv,offset,len,from,fromOffset);
}

size_t convertToULongArray(PVScalarArray * pv,
    size_t offset, size_t len,uint64 to[], size_t toOffset)
{
    return convertToScalarArray<uint64>(pv,offset,len,to,toOffset);
}

size_t convertFromFloatArray(PVScalarArray *pv,
    size_t offset, size_t len,const float from[], size_t fromOffset)
{
    return convertFromScalarArray<PVFloatArray,float>(pv,offset,len,from,fromOffset);
}

size_t convertToFloatArray(PVScalarArray * pv,
    size_t offset, size_t len,float to[], size_t toOffset)
{
    return convertToScalarArray<float>(pv,offset,len,to,toOffset);
}

size_t convertFromDoubleArray(PVScalarArray *pv,
    size_t offset, size_t len,const double from[], size_t fromOffset)
{
    return convertFromScalarArray<PVDoubleArray,double>(pv,offset,len,from,fromOffset);
}

size_t convertToDoubleArray(PVScalarArray * pv,
    size_t offset, size_t len,double to[], size_t toOffset)
{
    return convertToScalarArray<double>(pv,offset,len,to,toOffset);
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

ConvertPtr getConvert() {
    return Convert::getConvert();
}

}}
