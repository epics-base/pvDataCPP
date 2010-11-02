/* Convert.cpp */
#include <cstdio>
#include <string>
#include <vector>
#include <stdexcept>
#include "lock.h"
#include "pvIntrospect.h"
#include "pvData.h"
#include "convert.h"

namespace epics { namespace pvData { 

static Convert* convert = 0;
static PVDataCreate* pvDataCreate = 0;

static String trueString("true");
static String falseString("false");
static String logicError("Logic error. Should never get here.");
static String illegalScalarType("Illegal ScalarType");

static bool convertEquals(PVField *a,PVField *b);
static int convertFromByteArray(PVScalarArray *pv, int offset,
    int len,int8 from[], int fromOffset);
static int convertToByteArray(PVScalarArray *pv, int offset,
    int len,int8 to[], int toOffset);
static int convertFromShortArray(PVScalarArray *pv, int offset,
    int len,int16 from[], int fromOffset);
static int convertToShortArray(PVScalarArray *pv, int offset,
    int len,int16 to[], int toOffset);
static int convertFromIntArray(PVScalarArray *pv, int offset,
    int len,int32 from[], int fromOffset);
static int convertToIntArray(PVScalarArray *pv, int offset,
    int len,int32 to[], int toOffset);
static int convertFromLongArray(PVScalarArray *pv, int offset,
    int len,int64 from[], int fromOffset);
static int convertToLongArray(PVScalarArray * pv, int offset,
    int len,int64 to[], int toOffset);
static int convertFromFloatArray(PVScalarArray *pv, int offset,
    int len,float from[], int fromOffset);
static int convertToFloatArray(PVScalarArray * pv, int offset,
    int len,float to[], int toOffset);
static int convertFromDoubleArray(PVScalarArray *pv, int offset,
    int len,double from[], int fromOffset);
static int convertToDoubleArray(PVScalarArray * pv, int offset,
    int len,double to[], int toOffset);
static int convertFromStringArray(PVScalarArray *pv, int offset,
    int len,String from[], int fromOffset);
static int convertToStringArray(PVScalarArray * pv, int offset,
    int len,String to[], int toOffset);

static void convertToString(StringBuilder buffer,
    PVField * pv,int indentLevel);
static void convertStructure(StringBuilder buffer,
    PVStructure *data,int indentLevel);
static void convertArray(StringBuilder buffer,
    PVScalarArray * pv,int indentLevel);
static void convertStructureArray(StringBuilder buffer,
    PVStructureArray * pvdata,int indentLevel);
static int copyArrayDataReference(PVScalarArray *from,PVArray *to);
static int copyNumericArray(PVScalarArray *from,
    int offset, PVScalarArray *to, int toOffset, int len);

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
        int pos = commaSeparatedList.find(',',index);
        String value = commaSeparatedList.substr(index,pos);
        valueList[i] = value;
        index = pos +1;
    }
    return valueList;
}
    
Convert::Convert(){}

Convert::~Convert(){}

void Convert::getFullName(StringBuilder buf,PVField * pvField)
{
    buf->empty();
    *buf += pvField->getField()->getFieldName();
    PVStructure *parent;
    while((parent=pvField->getParent())!=0) {
         pvField = pvField->getParent();
         String name = pvField->getField()->getFieldName();
         if(name.length()>0) {
             buf->insert(0,".");
             buf->insert(0,name);
         }
    }
}

bool Convert::equals(PVField *a,PVField *b)
{
    return convertEquals(a,b);
}

void Convert::getString(StringBuilder buf,PVField * pvField,int indentLevel)
{
    convertToString(buf,pvField,indentLevel);
}

void Convert::getString(StringBuilder buf,PVField * pvField)
{
    convertToString(buf,pvField,0);
}

void Convert::fromString(PVScalar *pvScalar, String from)
{
    ScalarConstPtr scalar = pvScalar->getScalar();
    ScalarType scalarType = scalar->getScalarType();
    switch(scalarType) {
        case pvBoolean: {
                PVBoolean *pv = (PVBoolean *)pvScalar;
                bool value = 
                  ((from.compare("true")==0) ? true : false);
                pv->put(value);
                break;
            }
        case pvByte : {
                PVByte *pv = (PVByte*)pvScalar;
                int ival;
                sscanf(from.c_str(),"%d",&ival);
                int8 value = ival;
                pv->put(value);
                break;
            }
        case pvShort : {
                PVShort *pv = (PVShort*)pvScalar;
                int ival;
                sscanf(from.c_str(),"%d",&ival);
                int16 value = ival;
                pv->put(value);
                break;
            }
        case pvInt : {
                PVInt *pv = (PVInt*)pvScalar;
                int ival;
                sscanf(from.c_str(),"%d",&ival);
                int32 value = ival;
                pv->put(value);
                break;
            }
        case pvLong : {
                PVLong *pv = (PVLong*)pvScalar;
                int64 ival;
                sscanf(from.c_str(),"%lld",&ival);
                int64 value = ival;
                pv->put(value);
                break;
            }
        case pvFloat : {
                PVFloat *pv = (PVFloat*)pvScalar;
                float value;
                sscanf(from.c_str(),"%f",&value);
                pv->put(value);
                break;
            }
        case pvDouble : {
                PVDouble*pv = (PVDouble*)pvScalar;
                double value;
                sscanf(from.c_str(),"%lf",&value);
                pv->put(value);
                break;
            }
        case pvString: {
                PVString *value = (PVString*)pvScalar;
                value->put(from);
                break;
            }
    }
    String message("Convert::fromString unknown scalarType ");
    ScalarTypeFunc::toString(&message,scalarType);
    throw std::logic_error(message);
}

int Convert::fromString(PVScalarArray *pv, String from)
{
   if(from[0]=='[' && from[from.length()]==']') {
        int offset = from.rfind(']');
        from = from.substr(1, offset);
    }
    std::vector<String> valueList = split(from);
    int length = valueList.size();
    StringArray valueArray = new String[length];
    for(int i=0; i<length; i++) {
        String value = valueList[i];
        valueArray[i] = value;
    }
    int num = fromStringArray(pv,0,length,valueArray,0);
    if(num<length) length = num;
    pv->setLength(length);
    delete[] valueArray;
    return length;
}

int Convert::fromStringArray(PVScalarArray *pv, int offset, int length,
    String from[], int fromOffset)
{
    return convertFromStringArray(pv,offset,length,from,fromOffset);
}

int Convert::toStringArray(PVScalarArray * pv, int offset, int length,
    String to[], int toOffset)
{
    return convertToStringArray(pv,offset,length,to,toOffset);
}

bool Convert::isCopyCompatible(FieldConstPtr from, FieldConstPtr to)
{
    if(from->getType()!=to->getType()) return false;
    switch(from->getType()) {
    case scalar: 
        return isCopyScalarCompatible((ScalarConstPtr)from,(ScalarConstPtr)to);
    case scalarArray:
        return isCopyScalarArrayCompatible((ScalarArrayConstPtr)from,(ScalarArrayConstPtr)to);
    case structure:
        return isCopyStructureCompatible((StructureConstPtr)from,(StructureConstPtr)to);
    case structureArray:
        return isCopyStructureArrayCompatible((StructureArrayConstPtr)from,(StructureArrayConstPtr)to);
    }
    String message("Convert::isCopyCompatible should never get here");
    throw std::logic_error(message);
}

void Convert::copy(PVField *from,PVField *to)
{
    switch(from->getField()->getType()) {
    case scalar: 
        copyScalar((PVScalar *)from,(PVScalar *)to);
        return;
    case scalarArray: {
        PVScalarArray  *fromArray = (PVScalarArray *)from;
        PVScalarArray  *toArray = (PVScalarArray *)to;
        int length = copyScalarArray(fromArray,0,toArray,0,fromArray->getLength());
        if(toArray->getLength()!=length) toArray->setLength(length);
        return;
    }
    case structure:
        copyStructure((PVStructure *)from,(PVStructure *)to);
        return;
    case structureArray: {
    	PVStructureArray  *fromArray = (PVStructureArray *)from;
        PVStructureArray  *toArray = (PVStructureArray *)to;
        copyStructureArray(fromArray,toArray);
        return;
    }
    }
}

bool Convert::isCopyScalarCompatible(
     ScalarConstPtr fromField, ScalarConstPtr toField)
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

void Convert::copyScalar(PVScalar *from, PVScalar *to)
{
    if(to->isImmutable()) {
        if(from==to) return;
        String message("Convert.copyScalar destination is immutable");
        throw std::invalid_argument(message);
    }
    ScalarType fromType = from->getScalar()->getScalarType();
    ScalarType toType = to->getScalar()->getScalarType();
    switch(fromType) {
    case pvBoolean: {
            if(toType!=pvBoolean) {
                if(toType!=pvString) {
                    String message("Convert.copyScalar arguments are not compatible");
                    throw std::invalid_argument(message);
                }
            }
            PVBoolean *data = (PVBoolean*)from;
            if(toType==pvString) {
                PVString *dataTo = (PVString*)to;
                String buf("");
                data->toString(&buf);
                dataTo->put(buf);
            } else {
                bool value = data->get();
                PVBoolean *dataTo = (PVBoolean*)to;
                dataTo->put(value);
            }
            break;
        }
    case pvByte : {
            PVByte *data = (PVByte*)from;
            int8 value = data->get();
            convert->fromByte(to,value);
            break;
        }
    case pvShort : {
            PVShort *data = (PVShort*)from;
            short value = data->get();
            convert->fromShort(to,value);
            break;
        } 
    case pvInt :{
            PVInt *data = (PVInt*)from;
            int value = data->get();
            convert->fromInt(to,value);
            break;
        }    
    case pvLong : {
            PVLong *data = (PVLong*)from;
            long value = data->get();
            convert->fromLong(to,value);
            break;
        }  
    case pvFloat : {
            PVFloat *data = (PVFloat*)from;
            float value = data->get();
            convert->fromFloat(to,value);
            break;
        }     
    case pvDouble : {
            PVDouble *data = (PVDouble*)from;
            double value = data->get();
            convert->fromDouble(to,value);
            break;
        }  
    case pvString: {
            PVString *data = (PVString*)from;
            String value = data->get();
            convert->fromString(to,value);
            break;
        }
    }
    String message("Convert::copyScalar should never get here");
    throw std::logic_error(message);
}

bool Convert::isCopyScalarArrayCompatible(ScalarArrayConstPtr fromArray,
    ScalarArrayConstPtr toArray)
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

int Convert::copyScalarArray(PVScalarArray *from, int offset,
    PVScalarArray *to, int toOffset, int length)
{
    if(to->isImmutable()) {
        if(from==to) return from->getLength();
        String message("Convert.copyArray destination is immutable");
        throw std::invalid_argument(message);
    }
    ScalarType fromElementType = from->getScalarArray()->getElementType();
    ScalarType toElementType = to->getScalarArray()->getElementType();
    
    if(from->isImmutable() && (fromElementType==toElementType)) {
        if(offset==0 && toOffset==0 && length==from->getLength()) {
            return copyArrayDataReference(from,to);
        }
    }
    
    int ncopy = 0;
    if(ScalarTypeFunc::isNumeric(fromElementType)
    && ScalarTypeFunc::isNumeric(toElementType)) {
        return copyNumericArray(from,offset,to,toOffset,length);
    } else if(toElementType==pvBoolean && fromElementType==pvBoolean) {
        PVBooleanArray *pvfrom = (PVBooleanArray*)from;
        PVBooleanArray *pvto = (PVBooleanArray*)to;
        while(length>0) {
            int num = 0;
            BooleanArray data = 0;
            int fromOffset = 0;
            BooleanArrayData booleanArrayData = BooleanArrayData();
            num = pvfrom->get(offset,length,&booleanArrayData);
            data = booleanArrayData.data;
            fromOffset = booleanArrayData.offset;
            if(num<=0) return ncopy;
            while(num>0) {
                int n = pvto->put(toOffset,num,data,fromOffset);
                if(n<=0) return ncopy;
                length -= n; num -= n; ncopy+=n; offset += n; toOffset += n; 
            }
        }
    } else if(toElementType==pvString && fromElementType==pvString) {
        PVStringArray *pvfrom = (PVStringArray*)from;
        PVStringArray *pvto = (PVStringArray*)to;
        while(length>0) {
            int num = 0;
            String *data;
            int fromOffset = 0;
            StringArrayData stringArrayData = StringArrayData();
            num = pvfrom->get(offset,length,&stringArrayData);
            data = stringArrayData.data;
            fromOffset = stringArrayData.offset;
            if(num<=0) return ncopy;
            while(num>0) {
                int n = pvto->put(toOffset,num,data,fromOffset);
                if(n<=0) return ncopy;
                length -= n; num -= n; ncopy+=n; offset += n; toOffset += n; 
            }
        }
    } else if(toElementType==pvString) {
        PVStringArray *pvto = (PVStringArray*)to;
        ncopy = from->getLength();
        if(ncopy>length) ncopy = length;
        int num = ncopy;
        String toData[1];
        while(num>0) {
            convert->toStringArray(from,offset,1,toData,0);
            if(pvto->put(toOffset,1,toData,0)<=0) break;
            num--; offset++; toOffset++;
        }
        return ncopy;
    } else if(fromElementType==pvString) {
        PVStringArray *pvfrom = (PVStringArray*)from;
        while(length>0) {
            int num = 0;
            String *data = 0;
            int fromOffset = 0;
            StringArrayData stringArrayData = StringArrayData();
            num = pvfrom->get(offset,length,&stringArrayData);
            data = stringArrayData.data;
            fromOffset = stringArrayData.offset;
            if(num<=0) return ncopy;
            while(num>0) {
                int n = fromStringArray(to,toOffset,num,data,fromOffset);
                if(n<=0) return ncopy;
                length -= n; num -= n; ncopy+=n; offset += n; toOffset += n; 
            }
        }
    }
    String message("Convert::copyScalarArray should not get here");
    throw std::logic_error(message);
}

bool Convert::isCopyStructureCompatible(
    StructureConstPtr fromStruct, StructureConstPtr toStruct)
{
    FieldConstPtrArray fromFields = fromStruct->getFields();
    FieldConstPtrArray toFields = toStruct->getFields();
    int length = fromStruct->getNumberFields();
    if(length!=toStruct->getNumberFields()) return false;
    for(int i=0; i<length; i++) {
        FieldConstPtr from = fromFields[i];
        FieldConstPtr to = toFields[i];
        Type fromType = from->getType();
        Type toType = to->getType();
        if(fromType!=toType) return false;
        switch(fromType) {
        case scalar:
            if(!convert->isCopyScalarCompatible((ScalarConstPtr)from,(ScalarConstPtr)to)) return false;
            break;
        case scalarArray:
            if(!isCopyScalarArrayCompatible((ScalarArrayConstPtr)from,(ScalarArrayConstPtr)to))
                return false;
            break;
        case structure:
            if(!isCopyStructureCompatible((StructureConstPtr)from,(StructureConstPtr)to))
                return false;
            break;
        case structureArray:
            if(!isCopyStructureArrayCompatible((StructureArrayConstPtr)from,
                (StructureArrayConstPtr)to)) return false;
        }
    }
    return true;
}

void Convert::copyStructure(PVStructure *from, PVStructure *to)
{
    if(to->isImmutable()) {
        if(from==to) return;
        String message("Convert.copyStructure destination is immutable");
        throw std::invalid_argument(message);
    }
    if(from==to) return;
    PVFieldPtrArray fromDatas = from->getPVFields();
    PVFieldPtrArray toDatas = to->getPVFields();
    if(from->getStructure()->getNumberFields() 
    != to->getStructure()->getNumberFields()) {
        String message("Convert.copyStructure Illegal copyStructure");
        throw std::invalid_argument(message);
    }
    int numberFields = from->getStructure()->getNumberFields();
    if(numberFields==2) {
        // look for enumerated structure and copy choices first
        String fieldName = fromDatas[0]->getField()->getFieldName();
        if(fieldName.compare("index")==0) {
            FieldConstPtr fieldIndex = fromDatas[0]->getField();
            FieldConstPtr fieldChoices = fromDatas[1]->getField();
            if(fieldIndex->getType()==scalar
            && fieldChoices->getFieldName().compare("choices")
            && fieldChoices->getType()==scalarArray) {
                PVScalar *pvScalar = (PVScalar*)fromDatas[0];
                PVScalarArray *pvArray = (PVScalarArray*)fromDatas[1];
                if((pvScalar->getScalar()->getScalarType()==pvInt)
                && (pvArray->getScalarArray()->getElementType()==pvString)) {
                   PVScalarArray* toArray = (PVScalarArray*)toDatas[1];
                   copyScalarArray(pvArray,0,toArray,0,pvArray->getLength());
                   PVScalar *toScalar = (PVScalar*)toDatas[0];
                   copyScalar(pvScalar,toScalar);
                   return;
                }
            }
        }
    }
    for(int i=0; i < numberFields; i++) {
        PVField *fromData = fromDatas[i];
        PVField *toData = toDatas[i];
        Type fromType = fromData->getField()->getType();
        Type toType = toData->getField()->getType();
        if(fromType!=toType) {
            String message("Convert.copyStructure Illegal copyStructure");
            throw std::invalid_argument(message);
        }
        switch(fromType) {
        case scalar:
            copyScalar((PVScalar*)fromData,(PVScalar*)toData);
            break;
        case scalarArray: {
            PVScalarArray *fromArray = (PVScalarArray*)fromData;
            PVScalarArray *toArray = (PVScalarArray*)toData;
            int length = copyScalarArray(fromArray,0,toArray,0,fromArray->getLength());
            if(toArray->getLength()!=length) toArray->setLength(length);
            break;
        }
        case structure:
            copyStructure((PVStructure*)fromData,(PVStructure*)toData);
            break;
        case structureArray: {
        	PVStructureArray *fromArray = (PVStructureArray*)fromData;
            PVStructureArray *toArray = (PVStructureArray*)toData;
            copyStructureArray(fromArray,toArray);
            break;
        }
        }
    }
}

bool Convert::isCopyStructureArrayCompatible(
    StructureArrayConstPtr from, StructureArrayConstPtr to)
{
    return isCopyStructureCompatible(from->getStructure(),to->getStructure());
}

void Convert::copyStructureArray(
    PVStructureArray *from, PVStructureArray *to)
{
    if(to->isImmutable()) {
        if(from==to) return;
        String message("Convert.copyStructureArray destination is immutable");
        throw std::invalid_argument(message);
    }
    if(!isCopyStructureCompatible(
    from->getStructureArray()->getStructure(),
    to->getStructureArray()->getStructure())) {
        String message("Convert.copyStructureArray from and to are not compatible");
        throw std::invalid_argument(message);
    }
    PVStructurePtrArray fromArray = 0;
    int length = from->getLength();
    StructureArrayData structureArrayData = StructureArrayData();
    from->get(0, length,&structureArrayData);
    fromArray = structureArrayData.data;
    PVStructurePtrArray toArray = 0;
    if(to->getCapacity()<length) to->setCapacity(length);
    to->get(0, length,&structureArrayData);
    toArray = structureArrayData.data;
    for(int i=0; i<length; i++) {
    	if(fromArray[i]==0) {
            toArray[i] = 0;
    	} else {
    	    if(toArray[i]==0) {
    		StructureConstPtr structure = to->getStructureArray()->getStructure();
    		toArray[i] = pvDataCreate->createPVStructure(0,structure);
    	    }
    	    copyStructure(fromArray[i],toArray[i]);
    	}
    }
    to->setLength(length);
    to->postPut();
}

int8 Convert::toByte(PVScalar * pv)
{
    ScalarConstPtr scalar = pv->getScalar();
    ScalarType scalarType = scalar->getScalarType();
    switch(scalarType) {
        case pvBoolean:
           throw std::logic_error(String("boolean can not be converted to byte"));
        case pvByte: {
            PVByte *value = (PVByte *)pv;
            return value->get();
        }
        case pvShort: {
            PVShort *value = (PVShort *)pv;
            return value->get();
        }
        case pvInt: {
            PVInt *value = (PVInt *)pv;
            return value->get();
        }
        case pvLong: {
            PVLong *value = (PVLong *)pv;
            return value->get();
        }
        case pvFloat: {
            PVFloat *value = (PVFloat *)pv;
            return value->get();
        }
        case pvDouble: {
            PVDouble *value = (PVDouble *)pv;
            return value->get();
        }
        case pvString:
           throw std::logic_error(String("string can not be converted to byte"));
    }
    throw std::logic_error(logicError);
}

int16 Convert::toShort(PVScalar * pv)
{
    ScalarConstPtr scalar = pv->getScalar();
    ScalarType scalarType = scalar->getScalarType();
    switch(scalarType) {
        case pvBoolean:
           throw std::logic_error(String("boolean can not be converted to short"));
        case pvByte: {
            PVByte *value = (PVByte *)pv;
            return value->get();
        }
        case pvShort: {
            PVShort *value = (PVShort *)pv;
            return value->get();
        }
        case pvInt: {
            PVInt *value = (PVInt *)pv;
            return value->get();
        }
        case pvLong: {
            PVLong *value = (PVLong *)pv;
            return value->get();
        }
        case pvFloat: {
            PVFloat *value = (PVFloat *)pv;
            return value->get();
        }
        case pvDouble: {
            PVDouble *value = (PVDouble *)pv;
            return value->get();
        }
        case pvString:
           throw std::logic_error(String("string can not be converted to short"));
    }
    throw std::logic_error(logicError);
}

int32 Convert::toInt(PVScalar * pv)
{
    ScalarConstPtr scalar = pv->getScalar();
    ScalarType scalarType = scalar->getScalarType();
    switch(scalarType) {
        case pvBoolean:
           throw std::logic_error(String("boolean can not be converted to int"));
        case pvByte: {
            PVByte *value = (PVByte *)pv;
            return value->get();
        }
        case pvShort: {
            PVShort *value = (PVShort *)pv;
            return value->get();
        }
        case pvInt: {
            PVInt *value = (PVInt *)pv;
            return value->get();
        }
        case pvLong: {
            PVLong *value = (PVLong *)pv;
            return value->get();
        }
        case pvFloat: {
            PVFloat *value = (PVFloat *)pv;
            return value->get();
        }
        case pvDouble: {
            PVDouble *value = (PVDouble *)pv;
            return value->get();
        }
        case pvString:
           throw std::logic_error(String("string can not be converted to int"));
    }
    throw std::logic_error(logicError);
}

int64 Convert::toLong(PVScalar * pv)
{
    ScalarConstPtr scalar = pv->getScalar();
    ScalarType scalarType = scalar->getScalarType();
    switch(scalarType) {
        case pvBoolean:
           throw std::logic_error(String("boolean can not be converted to long"));
        case pvByte: {
            PVByte *value = (PVByte *)pv;
            return value->get();
        }
        case pvShort: {
            PVShort *value = (PVShort *)pv;
            return value->get();
        }
        case pvInt: {
            PVInt *value = (PVInt *)pv;
            return value->get();
        }
        case pvLong: {
            PVLong *value = (PVLong *)pv;
            return value->get();
        }
        case pvFloat: {
            PVFloat *value = (PVFloat *)pv;
            return value->get();
        }
        case pvDouble: {
            PVDouble *value = (PVDouble *)pv;
            return value->get();
        }
        case pvString:
           throw std::logic_error(String("string can not be converted to long"));
    }
    throw std::logic_error(logicError);
}

float Convert::toFloat(PVScalar * pv)
{
    ScalarConstPtr scalar = pv->getScalar();
    ScalarType scalarType = scalar->getScalarType();
    switch(scalarType) {
        case pvBoolean:
           throw std::logic_error(String("boolean can not be converted to float"));
        case pvByte: {
            PVByte *value = (PVByte *)pv;
            return value->get();
        }
        case pvShort: {
            PVShort *value = (PVShort *)pv;
            return value->get();
        }
        case pvInt: {
            PVInt *value = (PVInt *)pv;
            return value->get();
        }
        case pvLong: {
            PVLong *value = (PVLong *)pv;
            return value->get();
        }
        case pvFloat: {
            PVFloat *value = (PVFloat *)pv;
            return value->get();
        }
        case pvDouble: {
            PVDouble *value = (PVDouble *)pv;
            return value->get();
        }
        case pvString:
           throw std::logic_error(String("string can not be converted to float"));
    }
    throw std::logic_error(logicError);
}

double Convert::toDouble(PVScalar * pv)
{
    ScalarConstPtr scalar = pv->getScalar();
    ScalarType scalarType = scalar->getScalarType();
    switch(scalarType) {
        case pvBoolean:
           throw std::logic_error(String("boolean can not be converted to double"));
        case pvByte: {
            PVByte *value = (PVByte *)pv;
            return value->get();
        }
        case pvShort: {
            PVShort *value = (PVShort *)pv;
            return value->get();
        }
        case pvInt: {
            PVInt *value = (PVInt *)pv;
            return value->get();
        }
        case pvLong: {
            PVLong *value = (PVLong *)pv;
            return value->get();
        }
        case pvFloat: {
            PVFloat *value = (PVFloat *)pv;
            return value->get();
        }
        case pvDouble: {
            PVDouble *value = (PVDouble *)pv;
            return value->get();
        }
        case pvString:
           throw std::logic_error(String("string can not be converted to double"));
    }
    throw std::logic_error(logicError);
}

void Convert::fromByte(PVScalar *pv,int8 from)
{
    ScalarConstPtr scalar = pv->getScalar();
    ScalarType scalarType = scalar->getScalarType();
    switch(scalarType) {
        case pvBoolean:
           throw std::logic_error(String("byte can not be converted to boolean"));
        case pvByte: {
            PVByte *value = (PVByte *)pv;
            value->put(from); return;
        }
        case pvShort: {
            PVShort *value = (PVShort *)pv;
            value->put(from); return;
        }
        case pvInt: {
            PVInt *value = (PVInt *)pv;
            value->put(from); return;
        }
        case pvLong: {
            PVLong *value = (PVLong *)pv;
            value->put(from); return;
        }
        case pvFloat: {
            PVFloat *value = (PVFloat *)pv;
            value->put(from); return;
        }
        case pvDouble: {
            PVDouble *value = (PVDouble *)pv;
            value->put(from); return;
        }
        case pvString: {
            PVString *value = (PVString *)pv;
            char buffer[20];
            int ival = from;
            sprintf(buffer,"%d",ival);
            String xxx(buffer);
            value->put(xxx);
            return;
        }
    }
    throw std::logic_error(logicError);
}

void  Convert::fromShort(PVScalar *pv,int16 from)
{
    ScalarConstPtr scalar = pv->getScalar();
    ScalarType scalarType = scalar->getScalarType();
    switch(scalarType) {
        case pvBoolean:
           throw std::logic_error(String("short can not be converted to boolean"));
        case pvByte: {
            PVByte *value = (PVByte *)pv;
            value->put(from); return;
        }
        case pvShort: {
            PVShort *value = (PVShort *)pv;
            value->put(from); return;
        }
        case pvInt: {
            PVInt *value = (PVInt *)pv;
            value->put(from); return;
        }
        case pvLong: {
            PVLong *value = (PVLong *)pv;
            value->put(from); return;
        }
        case pvFloat: {
            PVFloat *value = (PVFloat *)pv;
            value->put(from); return;
        }
        case pvDouble: {
            PVDouble *value = (PVDouble *)pv;
            value->put(from); return;
        }
        case pvString: {
            PVString *value = (PVString *)pv;
            char buffer[20];
            int ival = from;
            sprintf(buffer,"%d",ival);
            String xxx(buffer);
            value->put(xxx);
            return;
        }
    }
    throw std::logic_error(logicError);
}

void  Convert::fromInt(PVScalar *pv, int32 from)
{
    ScalarConstPtr scalar = pv->getScalar();
    ScalarType scalarType = scalar->getScalarType();
    switch(scalarType) {
        case pvBoolean:
           throw std::logic_error(String("int can not be converted to boolean"));
        case pvByte: {
            PVByte *value = (PVByte *)pv;
            value->put(from); return;
        }
        case pvShort: {
            PVShort *value = (PVShort *)pv;
            value->put(from); return;
        }
        case pvInt: {
            PVInt *value = (PVInt *)pv;
            value->put(from); return;
        }
        case pvLong: {
            PVLong *value = (PVLong *)pv;
            value->put(from); return;
        }
        case pvFloat: {
            PVFloat *value = (PVFloat *)pv;
            value->put(from); return;
        }
        case pvDouble: {
            PVDouble *value = (PVDouble *)pv;
            value->put(from); return;
        }
        case pvString: {
            PVString *value = (PVString *)pv;
            char buffer[20];
            int ival = from;
            sprintf(buffer,"%d",ival);
            String xxx(buffer);
            value->put(xxx);
            return;
        }
    }
    throw std::logic_error(logicError);
}

void  Convert::fromLong(PVScalar *pv, int64 from)
{
    ScalarConstPtr scalar = pv->getScalar();
    ScalarType scalarType = scalar->getScalarType();
    switch(scalarType) {
        case pvBoolean:
           throw std::logic_error(String("long can not be converted to boolean"));
        case pvByte: {
            PVByte *value = (PVByte *)pv;
            value->put(from); return;
        }
        case pvShort: {
            PVShort *value = (PVShort *)pv;
            value->put(from); return;
        }
        case pvInt: {
            PVInt *value = (PVInt *)pv;
            value->put(from); return;
        }
        case pvLong: {
            PVLong *value = (PVLong *)pv;
            value->put(from); return;
        }
        case pvFloat: {
            PVFloat *value = (PVFloat *)pv;
            value->put(from); return;
        }
        case pvDouble: {
            PVDouble *value = (PVDouble *)pv;
            value->put(from); return;
        }
        case pvString: {
            PVString *value = (PVString *)pv;
            char buffer[20];
            int64 ival = from;
            sprintf(buffer,"%lld",ival);
            String xxx(buffer);
            value->put(xxx);
            return;
        }
    }
    throw std::logic_error(logicError);
}

void  Convert::fromFloat(PVScalar* pv, float from)
{
    ScalarConstPtr scalar = pv->getScalar();
    ScalarType scalarType = scalar->getScalarType();
    switch(scalarType) {
        case pvBoolean:
           throw std::logic_error(String("float can not be converted to boolean"));
        case pvByte: {
            PVByte *value = (PVByte *)pv;
            value->put(from); return;
        }
        case pvShort: {
            PVShort *value = (PVShort *)pv;
            value->put(from); return;
        }
        case pvInt: {
            PVInt *value = (PVInt *)pv;
            value->put(from); return;
        }
        case pvLong: {
            PVLong *value = (PVLong *)pv;
            value->put(from); return;
        }
        case pvFloat: {
            PVFloat *value = (PVFloat *)pv;
            value->put(from); return;
        }
        case pvDouble: {
            PVDouble *value = (PVDouble *)pv;
            value->put(from); return;
        }
        case pvString: {
            PVString *value = (PVString *)pv;
            char buffer[20];
            double dval = from;
            sprintf(buffer,"%g",dval);
            String xxx(buffer);
            value->put(xxx);
            return;
        }
    }
    throw std::logic_error(logicError);
}

void  Convert::fromDouble(PVScalar *pv, double from)
{
    ScalarConstPtr scalar = pv->getScalar();
    ScalarType scalarType = scalar->getScalarType();
    switch(scalarType) {
        case pvBoolean:
           throw std::logic_error(String("double can not be converted to boolean"));
        case pvByte: {
            PVByte *value = (PVByte *)pv;
            value->put(from); return;
        }
        case pvShort: {
            PVShort *value = (PVShort *)pv;
            value->put(from); return;
        }
        case pvInt: {
            PVInt *value = (PVInt *)pv;
            value->put(from); return;
        }
        case pvLong: {
            PVLong *value = (PVLong *)pv;
            value->put(from); return;
        }
        case pvFloat: {
            PVFloat *value = (PVFloat *)pv;
            value->put(from); return;
        }
        case pvDouble: {
            PVDouble *value = (PVDouble *)pv;
            value->put(from); return;
        }
        case pvString: {
            PVString *value = (PVString *)pv;
            char buffer[20];
            double dval = from;
            sprintf(buffer,"%g",dval);
            String xxx(buffer);
            value->put(xxx);
            return;
        }
    }
    throw std::logic_error(logicError);
}

int Convert::toByteArray(PVScalarArray * pv, int offset, int length,
    int8 to[], int toOffset)
{
    return convertToByteArray(pv, offset, length, to, toOffset);
}

int Convert::toShortArray(PVScalarArray * pv, int offset, int length,
    int16 to[], int toOffset)
{
    return convertToShortArray(pv, offset, length, to, toOffset);
}

int Convert::toIntArray(PVScalarArray * pv, int offset, int length,
    int32 to[], int toOffset)
{
    return convertToIntArray(pv, offset, length, to, toOffset);
}

int Convert::toLongArray(PVScalarArray * pv, int offset, int length,
    int64 to[], int toOffset)
{
    return convertToLongArray(pv, offset, length, to, toOffset);
}

int Convert::toFloatArray(PVScalarArray * pv, int offset, int length,
    float to[], int toOffset)
{
    return convertToFloatArray(pv, offset, length, to, toOffset);
}

int Convert::toDoubleArray(PVScalarArray * pv, int offset, int length,
    double to[], int toOffset)
{
    return convertToDoubleArray(pv, offset, length, to, toOffset);
}

int Convert::fromByteArray(PVScalarArray *pv, int offset, int length,
    int8 from[], int fromOffset)
{
    return convertFromByteArray(pv, offset, length, from, fromOffset);
}

int Convert::fromShortArray(PVScalarArray *pv, int offset, int length,
    int16 from[], int fromOffset)
{
    return convertFromShortArray(pv, offset, length, from, fromOffset);
}

int Convert::fromIntArray(PVScalarArray *pv, int offset, int length,
    int32 from[], int fromOffset)
{
    return convertFromIntArray(pv, offset, length, from, fromOffset);
}

int Convert::fromLongArray(PVScalarArray *pv, int offset, int length,
    int64 from[], int fromOffset)
{
    return convertFromLongArray(pv, offset, length, from, fromOffset);
}

int Convert::fromFloatArray(PVScalarArray *pv, int offset, int length,
    float from[], int fromOffset)
{
    return convertFromFloatArray(pv, offset, length, from, fromOffset);
}

int Convert::fromDoubleArray(PVScalarArray *pv, int offset, int length,
    double from[], int fromOffset)
{
    return convertFromDoubleArray(pv, offset, length, from, fromOffset);
}

void Convert::newLine(StringBuilder buffer, int indentLevel)
{
    *buffer += "\n";
    for(int i=0; i<indentLevel; i++) *buffer += "    ";
}

static bool scalarEquals(PVScalar *a,PVScalar *b)
{
    ScalarType ascalarType = a->getScalar()->getScalarType();
    ScalarType bscalarType = b->getScalar()->getScalarType();
    if(ascalarType!=bscalarType) return false;
    switch(ascalarType) {
        case pvBoolean: {
            PVBoolean *pa = (PVBoolean *)a;
            PVBoolean *pb = (PVBoolean *)b;
            bool avalue = pa->get();
            bool bvalue = pb->get();
            return ((avalue==bvalue) ? true : false);
        }
        case pvByte: {
            PVByte *pa = (PVByte *)a;
            PVByte *pb = (PVByte *)b;
            int8 avalue = pa->get();
            int8 bvalue = pb->get();
            return ((avalue==bvalue) ? true : false);
        }
        case pvShort: {
            PVShort *pa = (PVShort *)a;
            PVShort *pb = (PVShort *)b;
            int16 avalue = pa->get();
            int16 bvalue = pb->get();
            return ((avalue==bvalue) ? true : false);
        }
        case pvInt: {
            PVInt *pa = (PVInt *)a;
            PVInt *pb = (PVInt *)b;
            int32 avalue = pa->get();
            int32 bvalue = pb->get();
            return ((avalue==bvalue) ? true : false);
        }
        case pvLong: {
            PVLong *pa = (PVLong *)a;
            PVLong *pb = (PVLong *)b;
            int64 avalue = pa->get();
            int64 bvalue = pb->get();
            return ((avalue==bvalue) ? true : false);
        }
        case pvFloat: {
            PVFloat *pa = (PVFloat *)a;
            PVFloat *pb = (PVFloat *)b;
            float avalue = pa->get();
            float bvalue = pb->get();
            return ((avalue==bvalue) ? true : false);
        }
        case pvDouble: {
            PVDouble *pa = (PVDouble *)a;
            PVDouble *pb = (PVDouble *)b;
            double avalue = pa->get();
            double bvalue = pb->get();
            return ((avalue==bvalue) ? true : false);
        }
        case pvString: {
            PVString *pa = (PVString *)a;
            PVString *pb = (PVString *)b;
            String avalue = pa->get();
            String bvalue = pb->get();
            return ((avalue==bvalue) ? true : false);
        }
    }
    String message("should not get here");
    throw std::logic_error(message);
}

static bool arrayEquals(PVScalarArray *a,PVScalarArray *b)
{
    if(a==b) return true;
    ScalarType aType = a->getScalarArray()->getElementType();
    ScalarType bType = b->getScalarArray()->getElementType();
    if(aType!=bType) return false;
    if(a->getLength()!=b->getLength()) return false;
    int length = a->getLength();
    switch(aType) {
        case pvBoolean: {
            PVBooleanArray *aarray = (PVBooleanArray *)a;
            PVBooleanArray *barray = (PVBooleanArray *)b;
            BooleanArrayData adata = BooleanArrayData();
            BooleanArrayData bdata = BooleanArrayData();
            aarray->get(0,length,&adata);
            barray->get(0,length,&bdata);
            BooleanArray avalue = adata.data;
            BooleanArray bvalue = bdata.data;
            for(int i=0; i<length; i++) {
                if(avalue[i]!=bvalue[i]) return false;
            }
            return true;
        }
        case pvByte: {
            PVByteArray *aarray = (PVByteArray *)a;
            PVByteArray *barray = (PVByteArray *)b;
            ByteArrayData adata = ByteArrayData();
            ByteArrayData bdata = ByteArrayData();
            aarray->get(0,length,&adata);
            barray->get(0,length,&bdata);
            ByteArray avalue = adata.data;
            ByteArray bvalue = bdata.data;
            for(int i=0; i<length; i++) {
                if(avalue[i]!=bvalue[i]) return false;
            }
            return true;
        }
        case pvShort: {
            PVShortArray *aarray = (PVShortArray *)a;
            PVShortArray *barray = (PVShortArray *)b;
            ShortArrayData adata = ShortArrayData();
            ShortArrayData bdata = ShortArrayData();
            aarray->get(0,length,&adata);
            barray->get(0,length,&bdata);
            ShortArray avalue = adata.data;
            ShortArray bvalue = bdata.data;
            for(int i=0; i<length; i++) {
                if(avalue[i]!=bvalue[i]) return false;
            }
            return true;
        }
        case pvInt: {
            PVIntArray *aarray = (PVIntArray *)a;
            PVIntArray *barray = (PVIntArray *)b;
            IntArrayData adata = IntArrayData();
            IntArrayData bdata = IntArrayData();
            aarray->get(0,length,&adata);
            barray->get(0,length,&bdata);
            IntArray avalue = adata.data;
            IntArray bvalue = bdata.data;
            for(int i=0; i<length; i++) {
                if(avalue[i]!=bvalue[i]) return false;
            }
            return true;
        }
        case pvLong: {
            PVLongArray *aarray = (PVLongArray *)a;
            PVLongArray *barray = (PVLongArray *)b;
            LongArrayData adata = LongArrayData();
            LongArrayData bdata = LongArrayData();
            aarray->get(0,length,&adata);
            barray->get(0,length,&bdata);
            LongArray avalue = adata.data;
            LongArray bvalue = bdata.data;
            for(int i=0; i<length; i++) {
                if(avalue[i]!=bvalue[i]) return false;
            }
            return true;
        }
        case pvFloat: {
            PVFloatArray *aarray = (PVFloatArray *)a;
            PVFloatArray *barray = (PVFloatArray *)b;
            FloatArrayData adata = FloatArrayData();
            FloatArrayData bdata = FloatArrayData();
            aarray->get(0,length,&adata);
            barray->get(0,length,&bdata);
            FloatArray avalue = adata.data;
            FloatArray bvalue = bdata.data;
            for(int i=0; i<length; i++) {
                if(avalue[i]!=bvalue[i]) return false;
            }
            return true;
        }
        case pvDouble: {
            PVDoubleArray *aarray = (PVDoubleArray *)a;
            PVDoubleArray *barray = (PVDoubleArray *)b;
            DoubleArrayData adata = DoubleArrayData();
            DoubleArrayData bdata = DoubleArrayData();
            aarray->get(0,length,&adata);
            barray->get(0,length,&bdata);
            DoubleArray avalue = adata.data;
            DoubleArray bvalue = bdata.data;
            for(int i=0; i<length; i++) {
                if(avalue[i]!=bvalue[i]) return false;
            }
            return true;
        }
        case pvString: {
            PVStringArray *aarray = (PVStringArray *)a;
            PVStringArray *barray = (PVStringArray *)b;
            StringArrayData adata = StringArrayData();
            StringArrayData bdata = StringArrayData();
            aarray->get(0,length,&adata);
            barray->get(0,length,&bdata);
            String *avalue = adata.data;
            String *bvalue = bdata.data;
            for(int i=0; i<length; i++) {
                if(avalue[i]!=bvalue[i]) return false;
            }
            return true;
        }
    }
    String message("should not get here");
    throw std::logic_error(message);
}

static bool structureArrayEquals(PVStructureArray *a,PVStructureArray *b)
{
    StructureConstPtr aStructure = a->getStructureArray()->getStructure();
    StructureConstPtr bStructure = b->getStructureArray()->getStructure();
    if(aStructure!=bStructure) return false;
    if(a->getLength()!=b->getLength()) return false;
    StructureArrayData aData = StructureArrayData();
    StructureArrayData bData = StructureArrayData();
    int length = a->getLength();
    PVStructurePtrArray aArray = aData.data;
    PVStructurePtrArray bArray = bData.data;
    if(aArray==bArray) return true;
    for(int i=0; i<length; i++) {
        if(aArray[i]==0) {
            if(bArray[i]==0) continue;
            return false;
        } else {
            if(bArray[i]==0) return false;
        }
        if(aArray[i]!=bArray[i]) return false;
    }
    return true;
}

static bool structureEquals(PVStructure *a,PVStructure *b)
{
    StructureConstPtr aStructure = a->getStructure();
    StructureConstPtr bStructure = a->getStructure();
    int length = aStructure->getNumberFields();
    if(length!=bStructure->getNumberFields()) return false;
    PVFieldPtrArray aFields = a->getPVFields();
    PVFieldPtrArray bFields = b->getPVFields();
    for(int i=0; i<length; i++) {
        if(aFields[i]!=bFields[i]) return false;
    }
    return true;
}

bool convertEquals(PVField *a,PVField *b)
{
    void * avoid = (void *)a;
    void * bvoid = (void *)b;
    if(avoid==bvoid) return true;
    Type atype = a->getField()->getType();
    Type btype = b->getField()->getType();
    if(atype!=btype) return false;
    if(atype==scalar) return scalarEquals((PVScalar *)a,(PVScalar *)b);
    if(atype==scalarArray) {
         return arrayEquals((PVScalarArray *)a,(PVScalarArray *)b);
    }
    if(atype==structureArray) {
         return structureArrayEquals(
             (PVStructureArray *)a,(PVStructureArray *)b);
    }
    if(atype==structure) {
        return structureEquals((PVStructure *)a,(PVStructure *)b);
    }
    String message("should not get here");
    throw std::logic_error(message);
}

int convertFromByteArray(PVScalarArray *pv, int offset, int len,int8 from[], int fromOffset)
{
    ScalarType elemType = pv->getScalarArray()->getElementType();
    int ntransfered = 0;
    switch (elemType) {
    case pvBoolean: {
         String message("convert from byte[] to BooleanArray not legal");
         throw std::invalid_argument(message);
    }
    case pvByte: {
        PVByteArray *pvdata = (PVByteArray*) pv;
        while (len > 0) {
            int n = pvdata->put(offset, len, from, fromOffset);
            if (n == 0)
                break;
            len -= n;
            offset += n;
            fromOffset += n;
            ntransfered += n;
        }
        return ntransfered;
    }
    case pvShort: {
        PVShortArray *pvdata = (PVShortArray*) pv;
        int16 data[1];
        while (len > 0) {
            data[0] = (int16) from[fromOffset];
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
        PVIntArray *pvdata = (PVIntArray*) pv;
        int32 data[1];
        while (len > 0) {
            data[0] = (int32) from[fromOffset];
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
        PVLongArray *pvdata = (PVLongArray*) pv;
        int64 data[1];
        while (len > 0) {
            data[0] = (int64) from[fromOffset];
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
        PVFloatArray *pvdata = (PVFloatArray*) pv;
        float data[1];
        while (len > 0) {
            data[0] = (float) from[fromOffset];
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
        PVDoubleArray *pvdata = (PVDoubleArray*) pv;
        double data[1];
        while (len > 0) {
            data[0] = (double) from[fromOffset];
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

int convertToByteArray(PVScalarArray * pv, int offset, int len,int8 to[], int toOffset)
{
    ScalarType elemType = pv->getScalarArray()->getElementType();
    int ntransfered = 0;
    switch (elemType) {
    case pvBoolean: {
        String message("convert BooleanArray to byte[]] not legal");
        throw std::invalid_argument(message);
    }
    case pvByte: {
        PVByteArray *pvdata = (PVByteArray*) pv;
        ByteArrayData data = ByteArrayData();
        while (len > 0) {
            int num = 0;
            num = pvdata->get(offset,len,&data);
            if (num <= 0) break;
            ByteArray dataArray = data.data;
            int dataOffset = data.offset;
            for(int i=0;i<num;i++) to[toOffset+i] = dataArray[dataOffset+1];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvShort: {
        PVShortArray *pvdata = (PVShortArray*) pv;
        ShortArrayData data = ShortArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0) break;
            ShortArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvInt: {
        PVIntArray *pvdata = (PVIntArray*) pv;
        IntArrayData data = IntArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0) break;
            IntArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvLong: {
        PVLongArray *pvdata = (PVLongArray*) pv;
        LongArrayData data = LongArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0)
                break;
            LongArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvFloat: {
        PVFloatArray *pvdata = (PVFloatArray*) pv;
        FloatArrayData data = FloatArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0) break;
            FloatArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvDouble: {
        PVDoubleArray *pvdata = (PVDoubleArray*) pv;
        DoubleArrayData data = DoubleArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0)
                break;
            DoubleArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvString: {
        String message("convert StringArray to byte[]] not legal");
        throw std::invalid_argument(message);
    }
    }
     String message("Convert::convertToByteArray should never get here");
    throw std::logic_error(message);
}

int convertFromShortArray(PVScalarArray *pv, int offset, int len,int16 from[], int fromOffset)
{
    ScalarType elemType = pv->getScalarArray()->getElementType();
    int ntransfered = 0;
    switch (elemType) {
    case pvBoolean: {
         String message("convert from short[] to BooleanArray not legal");
         throw std::invalid_argument(message);
    }
    case pvByte: {
        PVByteArray *pvdata = (PVByteArray*) pv;
        int8 data[1];
        while (len > 0) {
            data[0] = (int8) from[fromOffset];
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
        PVShortArray *pvdata = (PVShortArray*) pv;
        while (len > 0) {
            int n = pvdata->put(offset, len, from, fromOffset);
            if (n == 0)
                break;
            len -= n;
            offset += n;
            fromOffset += n;
            ntransfered += n;
        }
        return ntransfered;
    }
    case pvInt: {
        PVIntArray *pvdata = (PVIntArray*) pv;
        int32 data[1];
        while (len > 0) {
            data[0] = (int32) from[fromOffset];
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
        PVLongArray *pvdata = (PVLongArray*) pv;
        int64 data[1];
        while (len > 0) {
            data[0] = (int64) from[fromOffset];
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
        PVFloatArray *pvdata = (PVFloatArray*) pv;
        float data[1];
        while (len > 0) {
            data[0] = (float) from[fromOffset];
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
        PVDoubleArray *pvdata = (PVDoubleArray*) pv;
        double data[1];
        while (len > 0) {
            data[0] = (double) from[fromOffset];
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
         String message("convert from short[] to StringArray not legal");
         throw std::invalid_argument(message);
    }
    }
    String message("Convert::convertFromShortArray should never get here");
    throw std::logic_error(message);
}

int convertToShortArray(PVScalarArray * pv, int offset, int len,int16 to[], int toOffset)
{
    ScalarType elemType = pv->getScalarArray()->getElementType();
    int ntransfered = 0;
    switch (elemType) {
    case pvBoolean: {
        String message("convert BooleanArray to short[]] not legal");
        throw std::invalid_argument(message);
    }
    case pvByte: {
        PVByteArray *pvdata = (PVByteArray*) pv;
        ByteArrayData data = ByteArrayData();
        while (len > 0) {
            int num = 0;
            num = pvdata->get(offset,len,&data);
            if (num <= 0) break;
            ByteArray dataArray = data.data;
            int dataOffset = data.offset;
            for(int i=0;i<num;i++) to[toOffset+i] = dataArray[dataOffset+1];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvShort: {
        PVShortArray *pvdata = (PVShortArray*) pv;
        ShortArrayData data = ShortArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0) break;
            ShortArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvInt: {
        PVIntArray *pvdata = (PVIntArray*) pv;
        IntArrayData data = IntArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0) break;
            IntArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvLong: {
        PVLongArray *pvdata = (PVLongArray*) pv;
        LongArrayData data = LongArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0)
                break;
            LongArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvFloat: {
        PVFloatArray *pvdata = (PVFloatArray*) pv;
        FloatArrayData data = FloatArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0) break;
            FloatArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvDouble: {
        PVDoubleArray *pvdata = (PVDoubleArray*) pv;
        DoubleArrayData data = DoubleArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0)
                break;
            DoubleArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvString: {
        String message("convert StringArray to short[]] not legal");
        throw std::invalid_argument(message);
    }
    }
     String message("Convert::convertToShortArray should never get here");
    throw std::logic_error(message);
}

int convertFromIntArray(PVScalarArray *pv, int offset, int len,int32 from[], int fromOffset)
{
    ScalarType elemType = pv->getScalarArray()->getElementType();
    int ntransfered = 0;
    switch (elemType) {
    case pvBoolean: {
         String message("convert from int[] to BooleanArray not legal");
         throw std::invalid_argument(message);
    }
    case pvByte: {
        PVByteArray *pvdata = (PVByteArray*) pv;
        int8 data[1];
        while (len > 0) {
            data[0] = (int8) from[fromOffset];
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
        PVShortArray *pvdata = (PVShortArray*) pv;
        int16 data[1];
        while (len > 0) {
            data[0] = (int16) from[fromOffset];
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
        PVIntArray *pvdata = (PVIntArray*) pv;
        while (len > 0) {
            int n = pvdata->put(offset, len, from, fromOffset);
            if (n == 0)
                break;
            len -= n;
            offset += n;
            fromOffset += n;
            ntransfered += n;
        }
        return ntransfered;
    }
    case pvLong: {
        PVLongArray *pvdata = (PVLongArray*) pv;
        int64 data[1];
        while (len > 0) {
            data[0] = (int64) from[fromOffset];
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
        PVFloatArray *pvdata = (PVFloatArray*) pv;
        float data[1];
        while (len > 0) {
            data[0] = (float) from[fromOffset];
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
        PVDoubleArray *pvdata = (PVDoubleArray*) pv;
        double data[1];
        while (len > 0) {
            data[0] = (double) from[fromOffset];
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
         String message("convert from int[] to StringArray not legal");
         throw std::invalid_argument(message);
    }
    }
    String message("Convert::convertFromIntArray should never get here");
    throw std::logic_error(message);
}

int convertToIntArray(PVScalarArray * pv, int offset, int len,int32 to[], int toOffset)
{
    ScalarType elemType = pv->getScalarArray()->getElementType();
    int ntransfered = 0;
    switch (elemType) {
    case pvBoolean: {
        String message("convert BooleanArray to byte[]] not legal");
        throw std::invalid_argument(message);
    }
    case pvByte: {
        PVByteArray *pvdata = (PVByteArray*) pv;
        ByteArrayData data = ByteArrayData();
        while (len > 0) {
            int num = 0;
            num = pvdata->get(offset,len,&data);
            if (num <= 0) break;
            ByteArray dataArray = data.data;
            int dataOffset = data.offset;
            for(int i=0;i<num;i++) to[toOffset+i] = dataArray[dataOffset+1];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvShort: {
        PVShortArray *pvdata = (PVShortArray*) pv;
        ShortArrayData data = ShortArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0) break;
            ShortArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvInt: {
        PVIntArray *pvdata = (PVIntArray*) pv;
        IntArrayData data = IntArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0) break;
            IntArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvLong: {
        PVLongArray *pvdata = (PVLongArray*) pv;
        LongArrayData data = LongArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0)
                break;
            LongArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvFloat: {
        PVFloatArray *pvdata = (PVFloatArray*) pv;
        FloatArrayData data = FloatArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0) break;
            FloatArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvDouble: {
        PVDoubleArray *pvdata = (PVDoubleArray*) pv;
        DoubleArrayData data = DoubleArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0)
                break;
            DoubleArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvString: {
        String message("convert StringArray to int[]] not legal");
        throw std::invalid_argument(message);
    }
    }
     String message("Convert::convertToIntArray should never get here");
    throw std::logic_error(message);
}

int convertFromLongArray(PVScalarArray *pv, int offset, int len,int64 from[], int fromOffset)
{
    ScalarType elemType = pv->getScalarArray()->getElementType();
    int ntransfered = 0;
    switch (elemType) {
    case pvBoolean: {
         String message("convert from long[] to BooleanArray not legal");
         throw std::invalid_argument(message);
    }
    case pvByte: {
        PVByteArray *pvdata = (PVByteArray*) pv;
        int8 data[1];
        while (len > 0) {
            data[0] = (int8) from[fromOffset];
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
        PVShortArray *pvdata = (PVShortArray*) pv;
        int16 data[1];
        while (len > 0) {
            data[0] = (int16) from[fromOffset];
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
        PVIntArray *pvdata = (PVIntArray*) pv;
        int32 data[1];
        while (len > 0) {
            data[0] = (int32) from[fromOffset];
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
        PVLongArray *pvdata = (PVLongArray*) pv;
        while (len > 0) {
            int n = pvdata->put(offset, len, from, fromOffset);
            if (n == 0)
                break;
            len -= n;
            offset += n;
            fromOffset += n;
            ntransfered += n;
        }
        return ntransfered;
    }
    case pvFloat: {
        PVFloatArray *pvdata = (PVFloatArray*) pv;
        float data[1];
        while (len > 0) {
            data[0] = (float) from[fromOffset];
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
        PVDoubleArray *pvdata = (PVDoubleArray*) pv;
        double data[1];
        while (len > 0) {
            data[0] = (double) from[fromOffset];
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
         String message("convert from long[] to StringArray not legal");
         throw std::invalid_argument(message);
    }
    }
    String message("Convert::convertFromLongArray should never get here");
    throw std::logic_error(message);
}

int convertToLongArray(PVScalarArray * pv, int offset, int len,int64 to[], int toOffset)
{
    ScalarType elemType = pv->getScalarArray()->getElementType();
    int ntransfered = 0;
    switch (elemType) {
    case pvBoolean: {
        String message("convert BooleanArray to long[]] not legal");
        throw std::invalid_argument(message);
    }
    case pvByte: {
        PVByteArray *pvdata = (PVByteArray*) pv;
        ByteArrayData data = ByteArrayData();
        while (len > 0) {
            int num = 0;
            num = pvdata->get(offset,len,&data);
            if (num <= 0) break;
            ByteArray dataArray = data.data;
            int dataOffset = data.offset;
            for(int i=0;i<num;i++) to[toOffset+i] = dataArray[dataOffset+1];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvShort: {
        PVShortArray *pvdata = (PVShortArray*) pv;
        ShortArrayData data = ShortArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0) break;
            ShortArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvInt: {
        PVIntArray *pvdata = (PVIntArray*) pv;
        IntArrayData data = IntArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0) break;
            IntArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvLong: {
        PVLongArray *pvdata = (PVLongArray*) pv;
        LongArrayData data = LongArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0)
                break;
            LongArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvFloat: {
        PVFloatArray *pvdata = (PVFloatArray*) pv;
        FloatArrayData data = FloatArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0) break;
            FloatArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvDouble: {
        PVDoubleArray *pvdata = (PVDoubleArray*) pv;
        DoubleArrayData data = DoubleArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0)
                break;
            DoubleArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvString: {
        String message("convert StringArray to long[]] not legal");
        throw std::invalid_argument(message);
    }
    }
     String message("Convert::convertToLongArray should never get here");
    throw std::logic_error(message);
}

int convertFromFloatArray(PVScalarArray *pv, int offset, int len,float from[], int fromOffset)
{
    ScalarType elemType = pv->getScalarArray()->getElementType();
    int ntransfered = 0;
    switch (elemType) {
    case pvBoolean: {
         String message("convert from float[] to BooleanArray not legal");
         throw std::invalid_argument(message);
    }
    case pvByte: {
        PVByteArray *pvdata = (PVByteArray*) pv;
        int8 data[1];
        while (len > 0) {
            data[0] = (int8) from[fromOffset];
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
        PVShortArray *pvdata = (PVShortArray*) pv;
        int16 data[1];
        while (len > 0) {
            data[0] = (int16) from[fromOffset];
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
        PVIntArray *pvdata = (PVIntArray*) pv;
        int32 data[1];
        while (len > 0) {
            data[0] = (int32) from[fromOffset];
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
        PVLongArray *pvdata = (PVLongArray*) pv;
        int64 data[1];
        while (len > 0) {
            data[0] = (int64) from[fromOffset];
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
        PVFloatArray *pvdata = (PVFloatArray*) pv;
        while (len > 0) {
            int n = pvdata->put(offset, len, from, fromOffset);
            if (n == 0)
                break;
            len -= n;
            offset += n;
            fromOffset += n;
            ntransfered += n;
        }
        return ntransfered;
    }
    case pvDouble: {
        PVDoubleArray *pvdata = (PVDoubleArray*) pv;
        double data[1];
        while (len > 0) {
            data[0] = (double) from[fromOffset];
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
         String message("convert from float[] to StringArray not legal");
         throw std::invalid_argument(message);
    }
    }
    String message("Convert::convertFromFloatArray should never get here");
    throw std::logic_error(message);
}

int convertToFloatArray(PVScalarArray * pv, int offset, int len,float to[], int toOffset)
{
    ScalarType elemType = pv->getScalarArray()->getElementType();
    int ntransfered = 0;
    switch (elemType) {
    case pvBoolean: {
        String message("convert BooleanArray to float[]] not legal");
        throw std::invalid_argument(message);
    }
    case pvByte: {
        PVByteArray *pvdata = (PVByteArray*) pv;
        ByteArrayData data = ByteArrayData();
        while (len > 0) {
            int num = 0;
            num = pvdata->get(offset,len,&data);
            if (num <= 0) break;
            ByteArray dataArray = data.data;
            int dataOffset = data.offset;
            for(int i=0;i<num;i++) to[toOffset+i] = dataArray[dataOffset+1];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvShort: {
        PVShortArray *pvdata = (PVShortArray*) pv;
        ShortArrayData data = ShortArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0) break;
            ShortArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvInt: {
        PVIntArray *pvdata = (PVIntArray*) pv;
        IntArrayData data = IntArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0) break;
            IntArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvLong: {
        PVLongArray *pvdata = (PVLongArray*) pv;
        LongArrayData data = LongArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0)
                break;
            LongArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvFloat: {
        PVFloatArray *pvdata = (PVFloatArray*) pv;
        FloatArrayData data = FloatArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0) break;
            FloatArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvDouble: {
        PVDoubleArray *pvdata = (PVDoubleArray*) pv;
        DoubleArrayData data = DoubleArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0)
                break;
            DoubleArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvString: {
        String message("convert StringArray to float[]] not legal");
        throw std::invalid_argument(message);
    }
    }
     String message("Convert::convertToFloatArray should never get here");
    throw std::logic_error(message);
}

int convertFromDoubleArray(PVScalarArray *pv, int offset, int len,double from[], int fromOffset)
{
    ScalarType elemType = pv->getScalarArray()->getElementType();
    int ntransfered = 0;
    switch (elemType) {
    case pvBoolean: {
         String message("convert from double[] to BooleanArray not legal");
         throw std::invalid_argument(message);
    }
    case pvByte: {
        PVByteArray *pvdata = (PVByteArray*) pv;
        int8 data[1];
        while (len > 0) {
            data[0] = (int8) from[fromOffset];
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
        PVShortArray *pvdata = (PVShortArray*) pv;
        int16 data[1];
        while (len > 0) {
            data[0] = (int16) from[fromOffset];
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
        PVIntArray *pvdata = (PVIntArray*) pv;
        int32 data[1];
        while (len > 0) {
            data[0] = (int32) from[fromOffset];
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
        PVLongArray *pvdata = (PVLongArray*) pv;
        int64 data[1];
        while (len > 0) {
            data[0] = (int64) from[fromOffset];
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
        PVFloatArray *pvdata = (PVFloatArray*) pv;
        float data[1];
        while (len > 0) {
            data[0] = (float) from[fromOffset];
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
        PVDoubleArray *pvdata = (PVDoubleArray*) pv;
        while (len > 0) {
            int n = pvdata->put(offset, len, from, fromOffset);
            if (n == 0)
                break;
            len -= n;
            offset += n;
            fromOffset += n;
            ntransfered += n;
        }
        return ntransfered;
    }
    case pvString: {
         String message("convert from double[] to StringArray not legal");
         throw std::invalid_argument(message);
    }
    }
    String message("Convert::convertFromDoubleArray should never get here");
    throw std::logic_error(message);
}

int convertToDoubleArray(PVScalarArray * pv, int offset, int len,double to[], int toOffset)
{
    ScalarType elemType = pv->getScalarArray()->getElementType();
    int ntransfered = 0;
    switch (elemType) {
    case pvBoolean: {
        String message("convert BooleanArray to float[]] not legal");
        throw std::invalid_argument(message);
    }
    case pvByte: {
        PVByteArray *pvdata = (PVByteArray*) pv;
        ByteArrayData data = ByteArrayData();
        while (len > 0) {
            int num = 0;
            num = pvdata->get(offset,len,&data);
            if (num <= 0) break;
            ByteArray dataArray = data.data;
            int dataOffset = data.offset;
            for(int i=0;i<num;i++) to[toOffset+i] = dataArray[dataOffset+1];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvShort: {
        PVShortArray *pvdata = (PVShortArray*) pv;
        ShortArrayData data = ShortArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0) break;
            ShortArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvInt: {
        PVIntArray *pvdata = (PVIntArray*) pv;
        IntArrayData data = IntArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0) break;
            IntArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvLong: {
        PVLongArray *pvdata = (PVLongArray*) pv;
        LongArrayData data = LongArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0)
                break;
            LongArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvFloat: {
        PVFloatArray *pvdata = (PVFloatArray*) pv;
        FloatArrayData data = FloatArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0) break;
            FloatArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvDouble: {
        PVDoubleArray *pvdata = (PVDoubleArray*) pv;
        DoubleArrayData data = DoubleArrayData();
        while (len > 0) {
            int num = pvdata->get(offset, len, &data);
            if (num == 0)
                break;
            DoubleArray dataArray = data.data;
            int dataOffset = data.offset;
            for (int i = 0; i < num; i++)
                to[i + toOffset] = (int8) dataArray[i + dataOffset];
            len -= num;
            offset += num;
            toOffset += num;
            ntransfered += num;
        }
        return ntransfered;
    }
    case pvString: {
        String message("convert StringArray to double[]] not legal");
        throw std::invalid_argument(message);
    }
    }
    String message("Convert::convertToDoubleArray should never get here");
    throw std::logic_error(message);
}

int convertFromStringArray(PVScalarArray *pv, int offset, int len,String from[], int fromOffset)
{
    ScalarType elemType = pv->getScalarArray()->getElementType();
    int ntransfered = 0;
    switch (elemType) {
    case pvBoolean: {
        PVBooleanArray *pvdata = (PVBooleanArray*) pv;
        bool data[1];
        while (len > 0) {
            String fromString = from[fromOffset];
            data[0] = (fromString.compare("true")==0) ? true : false;
            if (pvdata->put(offset, 1, data, 0) == 0)
                return ntransfered;
            --len;
            ++ntransfered;
            ++offset;
            ++fromOffset;
        }
        return ntransfered;
    }
    case pvByte: {
        PVByteArray *pvdata = (PVByteArray*) pv;
        int8 data[1];
        while (len > 0) {
            String fromString = from[fromOffset];
            int ival;
            sscanf(fromString.c_str(),"%d",&ival);
            data[0] = ival;
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
        PVShortArray *pvdata = (PVShortArray*) pv;
        int16 data[1];
        while (len > 0) {
            String fromString = from[fromOffset];
            int ival;
            sscanf(fromString.c_str(),"%d",&ival);
            data[0] = ival;
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
        PVIntArray *pvdata = (PVIntArray*) pv;
        int32 data[1];
        while (len > 0) {
            String fromString = from[fromOffset];
            int ival;
            sscanf(fromString.c_str(),"%d",&ival);
            data[0] = ival;
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
        PVLongArray *pvdata = (PVLongArray*) pv;
        int64 data[1];
        while (len > 0) {
            String fromString = from[fromOffset];
            int64 ival;
            sscanf(fromString.c_str(),"%lld",&ival);
            data[0] = ival;
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
        PVFloatArray *pvdata = (PVFloatArray*) pv;
        float data[1];
        while (len > 0) {
            String fromString = from[fromOffset];
            float fval;
            sscanf(fromString.c_str(),"%f",&fval);
            data[0] = fval;
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
        PVDoubleArray *pvdata = (PVDoubleArray*) pv;
        double data[1];
        while (len > 0) {
            String fromString = from[fromOffset];
            double fval;
            sscanf(fromString.c_str(),"%lf",&fval);
            data[0] = fval;
            if (pvdata->put(offset, 1, data, 0) == 0)
                return ntransfered;
            --len;
            ++ntransfered;
            ++offset;
            ++fromOffset;
        }
        return ntransfered;
    }
    case pvString:
        PVStringArray *pvdata = (PVStringArray*) pv;
        while (len > 0) {
            int n = pvdata->put(offset, len, from, fromOffset);
            if (n == 0)
                break;
            len -= n;
            offset += n;
            fromOffset += n;
            ntransfered += n;
        }
        return ntransfered;
    }
    String message("Convert::convertFromStringArray should never get here");
    throw std::logic_error(message);
}

int convertToStringArray(PVScalarArray * pv, int offset, int len,String to[], int toOffset)
{
    ScalarType elementType = pv->getScalarArray()->getElementType();
    int ncopy = pv->getLength();
    if (ncopy > len) ncopy = len;
    int num = ncopy;
    switch (elementType) {
    case pvBoolean: {
        PVBooleanArray *pvdata = (PVBooleanArray*) pv;
        BooleanArrayData data = BooleanArrayData();
        for (int i = 0; i < num; i++) {
            if (pvdata->get(offset + i, 1, &data) == 1) {
                BooleanArray dataArray = data.data;
                bool value = dataArray[data.offset];
                to[toOffset + i] = value ? trueString : falseString;
            } else {
                to[toOffset + i] = "bad pv";
            }
        }
    }
    break;
    case pvByte: {
        PVByteArray *pvdata = (PVByteArray*) pv;
        ByteArrayData data = ByteArrayData();
        char cr[30];
        for (int i = 0; i < num; i++) {
            if (pvdata->get(offset + i, 1, &data) == 1) {
                ByteArray dataArray = data.data;
                int ival = dataArray[data.offset];
                sprintf(cr,"%d",ival);
                to[toOffset + i] = String(cr);
            } else {
                to[toOffset + i] = "bad pv";
            }
        }
    }
    break;
    case pvShort: {
        PVShortArray *pvdata = (PVShortArray*) pv;
        ShortArrayData data = ShortArrayData();
        char cr[30];
        for (int i = 0; i < num; i++) {
            if (pvdata->get(offset + i, 1, &data) == 1) {
                ShortArray dataArray = data.data;
                int ival = dataArray[data.offset];
                sprintf(cr,"%d",ival);
                to[toOffset + i] = String(cr);
            } else {
                to[toOffset + i] = "bad pv";
            }
        }
    }
    break;
    case pvInt: {
        PVIntArray *pvdata = (PVIntArray*) pv;
        IntArrayData data = IntArrayData();
        char cr[30];
        for (int i = 0; i < num; i++) {
            if (pvdata->get(offset + i, 1, &data) == 1) {
                IntArray dataArray = data.data;
                int ival = dataArray[data.offset];
                sprintf(cr,"%d",ival);
                to[toOffset + i] = String(cr);
            } else {
                to[toOffset + i] = "bad pv";
            }
        }
    }
    break;
    case pvLong: {
        PVLongArray *pvdata = (PVLongArray*) pv;
        LongArrayData data = LongArrayData();
        char cr[30];
        for (int i = 0; i < num; i++) {
            if (pvdata->get(offset + i, 1, &data) == 1) {
                LongArray dataArray = data.data;
                int64 ival = dataArray[data.offset];
                sprintf(cr,"%lld",ival);
                to[toOffset + i] = String(cr);
            } else {
                to[toOffset + i] = "bad pv";
            }
        }
    }
    break;
    case pvFloat: {
        PVFloatArray *pvdata = (PVFloatArray*) pv;
        FloatArrayData data = FloatArrayData();
        char cr[30];
        for (int i = 0; i < num; i++) {
            if (pvdata->get(offset + i, 1, &data) == 1) {
                FloatArray dataArray = data.data;
                float fval = dataArray[data.offset];
                sprintf(cr,"%g",fval);
                to[toOffset + i] = String(cr);
            } else {
                to[toOffset + i] = "bad pv";
            }
        }
    }
    break;
    case pvDouble: {
        PVDoubleArray *pvdata = (PVDoubleArray*) pv;
        DoubleArrayData data = DoubleArrayData();
        char cr[30];
        for (int i = 0; i < num; i++) {
            if (pvdata->get(offset + i, 1, &data) == 1) {
                DoubleArray dataArray = data.data;
                double fval = dataArray[data.offset];
                sprintf(cr,"%g",fval);
                to[toOffset + i] = String(cr);
            } else {
                to[toOffset + i] = "bad pv";
            }
        }
    }
    break;
    case pvString: {
        PVStringArray *pvdata = (PVStringArray*) pv;
        while (num > 0) {
            int numnow = 0;
            StringArray dataArray = 0;
            int dataOffset = 0;
            StringArrayData stringArrayData = StringArrayData();
            numnow = pvdata->get(offset, num, &stringArrayData);
            dataArray = stringArrayData.data;
            dataOffset = stringArrayData.offset;
            if (numnow <= 0) {
                for (int i = 0; i < num; i++)
                    to[toOffset + i] = "bad pv";
                break;
            }
            for(int i=0; i<num; i++) to[toOffset+i] = dataArray[dataOffset+i];
            num -= numnow;
            offset += numnow;
            toOffset += numnow;
        }
    }
    break;
    default:
        String message("Convert::convertToStringArray should never get here");
        throw std::logic_error(message);
    }
    return ncopy;
}


void convertToString(StringBuilder buffer,PVField * pv,int indentLevel)
{
    Type type = pv->getField()->getType();
    if(type==scalarArray) {
        return convertArray(buffer,(PVScalarArray *)pv,indentLevel);
    }
    if(type==structure) {
        return convertStructure(buffer,(PVStructure*)pv,indentLevel);
    }
    if(type==structureArray) {
    	return convertStructureArray(buffer,(PVStructureArray*)pv,indentLevel);
    }
    PVScalar *pvScalar = (PVScalar*)pv;
    switch(pvScalar->getScalar()->getScalarType()) {
    case pvBoolean: {
            PVBoolean *data = (PVBoolean*)pv;
            bool value = data->get();
            if(value) {
                *buffer += "true";
            } else {
                *buffer += "false";
            }
        }
        return;
    case pvByte: {
            PVByte *data = (PVByte*)pv;
            char xxx[30];
            sprintf(xxx,"%d",(int)data->get());
            *buffer += xxx;
        }
        return;
    case pvShort: {
            PVShort *data = (PVShort*)pv;
            char xxx[30];
            sprintf(xxx,"%d",(int)data->get());
            *buffer += xxx;
        }
        return;
    case pvInt: {
            PVInt *data = (PVInt*)pv;
            char xxx[30];
            sprintf(xxx,"%d",(int)data->get());
            *buffer += xxx;
        }
        return;
    case pvLong: {
            PVLong *data = (PVLong*)pv;
            char xxx[30];
            sprintf(xxx,"%lld",(int64)data->get());
            *buffer += xxx;
        }
        return;
    case pvFloat: {
            PVFloat *data = (PVFloat*)pv;
            char xxx[30];
            sprintf(xxx,"%g",data->get());
            *buffer += xxx;
        }
        return;
    case pvDouble: {
            PVDouble *data = (PVDouble*)pv;
            char xxx[30];
            sprintf(xxx,"%lg",data->get());
            *buffer += xxx;
        }
        return;
    case pvString: {
            PVString *data = (PVString*)pv;
            *buffer += data->get();
        }
        return;
    default:
        *buffer += "unknown ScalarType";
    }
}

void convertStructure(StringBuilder buffer,PVStructure *data,int indentLevel)
{
    convert->newLine(buffer, indentLevel);
    *buffer += "structure {";
    PVFieldPtrArray fieldsData = data->getPVFields();
    if (fieldsData != 0) {
        int length = data->getStructure()->getNumberFields();
        for(int i=0; i<length; i++) {
            PVField *fieldField = fieldsData[i];
            FieldConstPtr fieldnow = fieldField->getField();
            convert->newLine(buffer, indentLevel + 1);
            int size = fieldnow->getFieldName().length();
            char buff[size+2];
            sprintf(buff,"%s = ",fieldnow->getFieldName().c_str());
            *buffer += buff;
            convert->getString(buffer,fieldField,indentLevel + 1);
        }
    }
    convert->newLine(buffer, indentLevel);
    *buffer += "}";
}

void convertArray(StringBuilder buffer,PVScalarArray * pv,int indentLevel)
{
    ScalarArrayConstPtr array = pv->getScalarArray();
    ScalarType type = array->getElementType();
    switch(type) {
    case pvBoolean: {
            PVBooleanArray *pvdata = (PVBooleanArray*)pv;
            BooleanArrayData data = BooleanArrayData();
            *buffer += "[";
            for(int i=0; i < pvdata->getLength(); i++) {
                if(i!=0) *buffer += ",";
                int num = pvdata->get(i,1,&data);
                if(num==1) {
                     BooleanArray  value = data.data;
                     if(value[data.offset]) {
                         *buffer += "true";
                     } else {
                         *buffer += "false";
                     }
                } else {
                     *buffer += "???? ";
                }
            }
            *buffer += "]";
            break;
        }
    case pvByte: {
            PVByteArray *pvdata = (PVByteArray*)pv;
            ByteArrayData data = ByteArrayData();
            *buffer += "[";
            for(int i=0; i < pvdata->getLength(); i++) {
                if(i!=0) *buffer += ",";
                int num = pvdata->get(i,1,&data);
                if(num==1) {
                     int val = data.data[data.offset];
                     char buf[16];
                     sprintf(buf,"%d",val);
                     *buffer += buf;
                } else {
                     *buffer += "???? ";
                }
            }
            *buffer += "]";
            break;
        }
    case pvShort: {
            PVShortArray *pvdata = (PVShortArray*)pv;
            ShortArrayData data = ShortArrayData();
            *buffer += "[";
            for(int i=0; i < pvdata->getLength(); i++) {
                if(i!=0) *buffer += ',';
                int num = pvdata->get(i,1,&data);
                if(num==1) {
                     int val = data.data[data.offset];
                     char buf[16];
                     sprintf(buf,"%d",val);
                     *buffer += buf;
                } else {
                     *buffer += "???? ";
                }
            }
            *buffer += "]";
            break;
        }
    case pvInt: {
            PVIntArray *pvdata = (PVIntArray*)pv;
            IntArrayData data = IntArrayData();
            *buffer += "[";
            for(int i=0; i < pvdata->getLength(); i++) {
                if(i!=0) *buffer += ',';
                int num = pvdata->get(i,1,&data);
                if(num==1) {
                     int val = data.data[data.offset];
                     char buf[16];
                     sprintf(buf,"%d",val);
                     *buffer += buf;
                } else {
                     *buffer += "???? ";
                }
            }
            *buffer += "]";
            break;
        }
    case pvLong: {
            PVLongArray *pvdata = (PVLongArray*)pv;
            LongArrayData data = LongArrayData();
            *buffer += "[";
            for(int i=0; i < pvdata->getLength(); i++) {
                if(i!=0) *buffer += ',';
                int num = pvdata->get(i,1,&data);
                if(num==1) {
                     int64 val = data.data[data.offset];
                     char buf[16];
                     sprintf(buf,"%lld",val);
                     *buffer += buf;
                } else {
                     *buffer += "???? ";
                }
            }
            *buffer += "]";
            break;
        }
    case pvFloat: {
            PVFloatArray *pvdata = (PVFloatArray*)pv;
            FloatArrayData data = FloatArrayData();
            *buffer += "[";
            for(int i=0; i < pvdata->getLength(); i++) {
                if(i!=0) *buffer += ',';
                int num = pvdata->get(i,1,&data);
                if(num==1) {
                     float val = data.data[data.offset];
                     char buf[16];
                     sprintf(buf,"%g",val);
                     *buffer += buf;
                } else {
                     *buffer += "???? ";
                }
            }
            *buffer += "]";
            break;
        }
    case pvDouble: {
            PVDoubleArray *pvdata = (PVDoubleArray*)pv;
            DoubleArrayData data = DoubleArrayData();
            *buffer += "[";
            for(int i=0; i < pvdata->getLength(); i++) {
                if(i!=0) *buffer += ',';
                int num = pvdata->get(i,1,&data);
                if(num==1) {
                     double val = data.data[data.offset];
                     char buf[16];
                     sprintf(buf,"%lg",val);
                     *buffer += buf;
                } else {
                     *buffer += "???? ";
                }
            }
            *buffer += ("]");
            break;
        }
    case pvString: {
    	PVStringArray *pvdata = (PVStringArray*)pv;
    	StringArrayData data = StringArrayData();
    	*buffer += "[";
    	for(int i=0; i < pvdata->getLength(); i++) {
    		if(i!=0) *buffer += ",";
    		int num = pvdata->get(i,1,&data);
    		StringPtrArray value = data.data;
                if(num==1) {
                    if(value[data.offset].length()>0) {
                         *buffer += value[data.offset].c_str();
                    } else {
                         *buffer += "null";
                    }
    		} else {
    			*buffer += "null";
    		}
    	}
    	*buffer += "]";
    	break;
    }
    default:
        *buffer += " array element is unknown ScalarType";
    }
    if(pv->isImmutable()) {
        *buffer += " immutable ";
    }
}

void convertStructureArray(StringBuilder buffer,
    PVStructureArray * pvdata,int indentLevel)
{
    StructureArrayData data = StructureArrayData();
    int length = pvdata->getLength();
    pvdata->get(0, length, &data);
    *buffer += "[";
    for (int i = 0; i < length; i++) {
        if (i != 0) {
            *buffer += ",";
        }
        convert->newLine(buffer, indentLevel + 1);
        PVStructure *pvStructure = data.data[i];
        if (pvStructure == 0) {
            *buffer += "null";
        } else {
            pvStructure->toString(buffer,indentLevel+1);
        }
    }
    convert->newLine(buffer, indentLevel);
    *buffer += "]";
}

int copyArrayDataReference(PVScalarArray *from,PVArray *to)
{
    ScalarType scalarType = from->getScalarArray()->getElementType();
    switch (scalarType) {
    case pvBoolean: {
        PVBooleanArray *pvfrom = (PVBooleanArray*) from;
        PVBooleanArray *pvto = (PVBooleanArray*) to;
        BooleanArrayData booleanArrayData = BooleanArrayData();
        pvfrom->get(0, pvfrom->getLength(), &booleanArrayData);
        BooleanArray booleanArray = booleanArrayData.data;
        pvto->shareData(booleanArray,from->getCapacity(),from->getLength());
        break;
    }
    case pvByte: {
        PVByteArray *pvfrom = (PVByteArray*) from;
        PVByteArray *pvto = (PVByteArray*) to;
        ByteArrayData byteArrayData = ByteArrayData();
        pvfrom->get(0, pvfrom->getLength(), &byteArrayData);
        ByteArray byteArray = byteArrayData.data;
        pvto->shareData(byteArray,from->getCapacity(),from->getLength());
        break;
    }
    case pvShort: {
        PVShortArray *pvfrom = (PVShortArray*) from;
        PVShortArray *pvto = (PVShortArray*) to;
        ShortArrayData shortArrayData = ShortArrayData();
        pvfrom->get(0, pvfrom->getLength(), &shortArrayData);
        ShortArray shortArray = shortArrayData.data;
        pvto->shareData(shortArray,from->getCapacity(),from->getLength());
        break;
    }
    case pvInt: {
        PVIntArray *pvfrom = (PVIntArray*) from;
        PVIntArray *pvto = (PVIntArray*) to;
        IntArrayData intArrayData = IntArrayData();
        pvfrom->get(0, pvfrom->getLength(), &intArrayData);
        IntArray intArray = intArrayData.data;
        pvto->shareData(intArray,from->getCapacity(),from->getLength());
        break;
    }
    case pvLong: {
        PVLongArray *pvfrom = (PVLongArray*) from;
        PVLongArray *pvto = (PVLongArray*) to;
        LongArrayData longArrayData = LongArrayData();
        pvfrom->get(0, pvfrom->getLength(), &longArrayData);
        LongArray longArray = longArrayData.data;
        pvto->shareData(longArray,from->getCapacity(),from->getLength());
        break;
    }
    case pvFloat: {
        PVFloatArray *pvfrom = (PVFloatArray*) from;
        PVFloatArray *pvto = (PVFloatArray*) to;
        FloatArrayData longArrayData = FloatArrayData();
        pvfrom->get(0, pvfrom->getLength(), &longArrayData);
        FloatArray longArray = longArrayData.data;
        pvto->shareData(longArray,from->getCapacity(),from->getLength());
        break;
    }
    case pvDouble: {
        PVDoubleArray *pvfrom = (PVDoubleArray*) from;
        PVDoubleArray *pvto = (PVDoubleArray*) to;
        DoubleArrayData doubleArrayData = DoubleArrayData();
        pvfrom->get(0, pvfrom->getLength(), &doubleArrayData);
        DoubleArray doubleArray = doubleArrayData.data;
        pvto->shareData(doubleArray,from->getCapacity(),from->getLength());
        break;
    }
    case pvString: {
        PVStringArray *pvfrom = (PVStringArray*) from;
        PVStringArray *pvto = (PVStringArray*) to;
        StringArrayData stringArrayData = StringArrayData();
        pvfrom->get(0, pvfrom->getLength(), &stringArrayData);
        StringArray stringArray = stringArrayData.data;
        pvto->shareData(stringArray,from->getCapacity(),from->getLength());
        break;
    }
    }
    to->setImmutable();
    return from->getLength();
}

int copyNumericArray(PVScalarArray *from, int offset, PVScalarArray *to, int toOffset, int len)
{
    ScalarType fromElementType = from->getScalarArray()->getElementType();
    int ncopy = 0;
    switch (fromElementType) {
    case pvBoolean:
       throw std::logic_error(String("PVBooleanArray is not a numeric array"));
    case pvByte: {
        PVByteArray *pvfrom = (PVByteArray*) from;
        while (len > 0) {
            int num = 0;
            ByteArrayData byteArrayData = ByteArrayData();
            num = pvfrom->get(offset, len, &byteArrayData);
            ByteArray data = byteArrayData.data;
            int dataOffset = byteArrayData.offset;
            if (num <= 0) break;
            while (num > 0) {
                int n = convert->fromByteArray(
                     to, toOffset, num, data, dataOffset);
                if (n <= 0) break;
                len -= n;
                num -= n;
                ncopy += n;
                offset += n;
                toOffset += n;
            }
        }
        break;
    }
    case pvShort: {
        PVShortArray *pvfrom = (PVShortArray*) from;
        while (len > 0) {
            int num = 0;
            ShortArrayData shortArrayData = ShortArrayData();
            num = pvfrom->get(offset, len, &shortArrayData);
            ShortArray data = shortArrayData.data;
            int dataOffset = shortArrayData.offset;
            if (num <= 0) break;
            while (num > 0) {
                int n = convert->fromShortArray(
                    to, toOffset, num, data, dataOffset);
                if (n <= 0) break;
                len -= n;
                num -= n;
                ncopy += n;
                offset += n;
                toOffset += n;
            }
        }
        break;
    }
    case pvInt: {
        PVIntArray *pvfrom = (PVIntArray*) from;
        while (len > 0) {
            int num = 0;
            IntArrayData shortArrayData = IntArrayData();
            num = pvfrom->get(offset, len, &shortArrayData);
            IntArray data = shortArrayData.data;
            int dataOffset = shortArrayData.offset;
            if (num <= 0) break;
            while (num > 0) {
                int n = convert->fromIntArray(
                    to, toOffset, num, data, dataOffset);
                if (n <= 0) break;
                len -= n;
                num -= n;
                ncopy += n;
                offset += n;
                toOffset += n;
            }
        }
        break;
    }
    case pvLong: {
        PVLongArray *pvfrom = (PVLongArray*) from;
        while (len > 0) {
            int num = 0;
            LongArrayData longArrayData = LongArrayData();
            num = pvfrom->get(offset, len, &longArrayData);
            LongArray data = longArrayData.data;
            int dataOffset = longArrayData.offset;
            if (num <= 0) break;
            while (num > 0) {
                int n = convert->fromLongArray(
                    to, toOffset, num, data, dataOffset);
                if (n <= 0) break;
                len -= n;
                num -= n;
                ncopy += n;
                offset += n;
                toOffset += n;
            }
        }
        break;
    }
    case pvFloat: {
        PVFloatArray *pvfrom = (PVFloatArray*) from;
        while (len > 0) {
            int num = 0;
            FloatArrayData floatArrayData = FloatArrayData();
            num = pvfrom->get(offset, len, &floatArrayData);
            FloatArray data = floatArrayData.data;
            int dataOffset = floatArrayData.offset;
            if (num <= 0) break;
            while (num > 0) {
                int n = convert->fromFloatArray(
                    to, toOffset, num, data, dataOffset);
                if (n <= 0) break;
                len -= n;
                num -= n;
                ncopy += n;
                offset += n;
                toOffset += n;
            }
        }
        break;
    }
    case pvDouble: {
        PVDoubleArray *pvfrom = (PVDoubleArray*) from;
        while (len > 0) {
            int num = 0;
            DoubleArrayData doubleArrayData = DoubleArrayData();
            num = pvfrom->get(offset, len, &doubleArrayData);
            DoubleArray data = doubleArrayData.data;
            int dataOffset = doubleArrayData.offset;
            if (num <= 0) break;
            while (num > 0) {
                int n = convert->fromDoubleArray(
                    to, toOffset, num, data, dataOffset);
                if (n <= 0) break;
                len -= n;
                num -= n;
                ncopy += n;
                offset += n;
                toOffset += n;
            }
        }
        break;
    }
    case pvString:
       throw std::logic_error(String("PVStringArray is not a numeric array"));
    }
    return ncopy;
}

class ConvertExt : public Convert {
public:
    ConvertExt(): Convert(){};
};

Convert * getConvert() {
    static Mutex mutex = Mutex();
    Lock xx(&mutex);

    if(convert==0){
        convert = new ConvertExt();
        pvDataCreate = getPVDataCreate();
    }
    return convert;
}

}}
