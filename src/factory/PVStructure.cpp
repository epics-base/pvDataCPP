/*PVStructure.cpp*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <vector>

#define epicsExportSharedSymbols
#include <pv/pvData.h>
#include <pv/pvIntrospect.h>
#include <pv/convert.h>
#include <pv/factory.h>
#include <pv/bitSet.h>

using std::tr1::static_pointer_cast;
using std::size_t;

namespace epics { namespace pvData {

PVFieldPtr PVStructure::nullPVField;
PVBooleanPtr PVStructure::nullPVBoolean;
PVBytePtr PVStructure::nullPVByte;
PVShortPtr PVStructure::nullPVShort;
PVIntPtr PVStructure::nullPVInt;
PVLongPtr PVStructure::nullPVLong;
PVUBytePtr PVStructure::nullPVUByte;
PVUShortPtr PVStructure::nullPVUShort;
PVUIntPtr PVStructure::nullPVUInt;
PVULongPtr PVStructure::nullPVULong;
PVFloatPtr PVStructure::nullPVFloat;
PVDoublePtr PVStructure::nullPVDouble;
PVStringPtr PVStructure::nullPVString;
PVStructurePtr PVStructure::nullPVStructure;
PVStructureArrayPtr PVStructure::nullPVStructureArray;
PVUnionPtr PVStructure::nullPVUnion;
PVUnionArrayPtr PVStructure::nullPVUnionArray;
PVScalarArrayPtr PVStructure::nullPVScalarArray;

static PVFieldPtr findSubField(
    String const &fieldName,
    const PVStructure *pvStructure);

PVStructure::PVStructure(StructureConstPtr const & structurePtr)
: PVField(structurePtr),
  structurePtr(structurePtr),
  extendsStructureName("")
{
    size_t numberFields = structurePtr->getNumberFields();
    FieldConstPtrArray fields = structurePtr->getFields();
    StringArray fieldNames = structurePtr->getFieldNames();
//    PVFieldPtrArray * xxx = const_cast<PVFieldPtrArray *>(&pvFields);
    pvFields.reserve(numberFields);
    PVDataCreatePtr pvDataCreate = getPVDataCreate();
    for(size_t i=0; i<numberFields; i++) {
    	pvFields.push_back(pvDataCreate->createPVField(fields[i]));
    }
    for(size_t i=0; i<numberFields; i++) {
    	pvFields[i]->setParentAndName(this,fieldNames[i]);
    }
}

PVStructure::PVStructure(StructureConstPtr const & structurePtr,
    PVFieldPtrArray const & pvs
)
: PVField(structurePtr),
  structurePtr(structurePtr),
  extendsStructureName("")
{
    size_t numberFields = structurePtr->getNumberFields();
    StringArray fieldNames = structurePtr->getFieldNames();
    pvFields.reserve(numberFields);
    for(size_t i=0; i<numberFields; i++) {
        pvFields.push_back(pvs[i]);
    }
    for(size_t i=0; i<numberFields; i++) {
        pvFields[i]->setParentAndName(this,fieldNames[i]);
    }
}

PVStructure::~PVStructure()
{
}

void PVStructure::setImmutable()
{
    size_t numFields = pvFields.size();
    for(size_t i=0; i<numFields; i++) {
        PVFieldPtr pvField  = pvFields[i];
        pvField->setImmutable();
    }
    PVField::setImmutable();
}

StructureConstPtr PVStructure::getStructure() const
{
    return structurePtr;
}

const PVFieldPtrArray & PVStructure::getPVFields() const
{
    return pvFields;
}

PVFieldPtr  PVStructure::getSubField(String const &fieldName) const
{
    return findSubField(fieldName,this);
}


PVFieldPtr  PVStructure::getSubField(size_t fieldOffset) const
{
    if(fieldOffset<=getFieldOffset()) {
        return nullPVField;
    }
    if(fieldOffset>getNextFieldOffset()) return nullPVField;
    size_t numFields = pvFields.size();
    for(size_t i=0; i<numFields; i++) {
        PVFieldPtr pvField  = pvFields[i];
        if(pvField->getFieldOffset()==fieldOffset) return pvFields[i];
        if(pvField->getNextFieldOffset()<=fieldOffset) continue;
        if(pvField->getField()->getType()==structure) {
            PVStructure *pvStructure = static_cast<PVStructure *>(pvField.get());
            return pvStructure->getSubField(fieldOffset);
        }
    }
    throw std::logic_error("PVStructure.getSubField: Logic error");
}

void PVStructure::fixParentStructure()
{
    PVStructure *parent = getParent();
    if(parent==NULL) return;
    StructureConstPtr parentStructure = parent->structurePtr;
    String fieldName = getFieldName();
    size_t index = parentStructure->getFieldIndex(fieldName);
    StringArray const &fieldNames = parentStructure->getFieldNames();
    size_t num = fieldNames.size();
    FieldConstPtrArray fields(num);
    FieldConstPtrArray const & oldFields = parentStructure->getFields();
    for(size_t i=0; i< num; i++) {
        if(i==index) {
            fields[i] = structurePtr;
        } else {
            fields[i] = oldFields[i];
        }
    }
    FieldConstPtr field = getFieldCreate()->createStructure(
        parentStructure->getID(),fieldNames,fields);
    parent->replaceField(field);
    parent->fixParentStructure();
}

void PVStructure::appendPVField(
    String const &fieldName,
    PVFieldPtr const & pvField)
{
    size_t origLength = pvFields.size();
    size_t newLength = origLength+1;
    PVFieldPtrArray * xxx = const_cast<PVFieldPtrArray *>(&pvFields);
    xxx->push_back(pvField);
    FieldConstPtr field = getFieldCreate()->appendField(
        structurePtr,fieldName,pvField->getField());
    replaceField(field);
    structurePtr = static_pointer_cast<const Structure>(field);
    StringArray fieldNames = structurePtr->getFieldNames();
    for(size_t i=0; i<newLength; i++) {
        pvFields[i]->setParentAndName(this,fieldNames[i]);
    }
    fixParentStructure();
}

void PVStructure::appendPVFields(
    StringArray const & fieldNames,
    PVFieldPtrArray const & pvFields)
{
    size_t origLength = this->pvFields.size();
    size_t extra = fieldNames.size();
    if(extra==0) return;
    size_t newLength = origLength + extra;
    PVFieldPtrArray * xxx = const_cast<PVFieldPtrArray *>(&this->pvFields);
    xxx->reserve(newLength);
    for(size_t i=0; i<extra; i++) {
        xxx->push_back(pvFields[i]);
    }
    FieldConstPtrArray fields;
    fields.reserve(extra);
    for(size_t i=0; i<extra; i++) fields.push_back(pvFields[i]->getField());
    FieldConstPtr field = getFieldCreate()->appendFields(
        structurePtr,fieldNames,fields);
    replaceField(field);
    structurePtr = static_pointer_cast<const Structure>(field);
    StringArray names = structurePtr->getFieldNames();
    for(size_t i=0; i<newLength; i++) {
        (*xxx)[i]->setParentAndName(this,names[i]);
    }
    fixParentStructure();
}

void PVStructure::removePVField(String const &fieldName)
{
    PVFieldPtr pvField = getSubField(fieldName);
    if(pvField.get()==NULL) {
        return;
    }
    size_t origLength = pvFields.size();
    size_t newLength = origLength - 1;
    PVFieldPtrArray const & origPVFields = pvFields;
    FieldConstPtrArray origFields = structurePtr->getFields();
    PVFieldPtrArray newPVFields;
    newPVFields.reserve(newLength);
    StringArray newFieldNames;
    newFieldNames.reserve(newLength);
    FieldConstPtrArray fields;
    fields.reserve(newLength);
    for(size_t i=0; i<origLength; i++) {
        if(origPVFields[i]!=pvField) {
            newFieldNames.push_back(origPVFields[i]->getFieldName());
            newPVFields.push_back(origPVFields[i]);
            fields.push_back(origFields[i]);
        }
    }
    PVFieldPtrArray * xxx = const_cast<PVFieldPtrArray *>(&pvFields);
    xxx->swap(newPVFields);
    FieldConstPtr field = getFieldCreate()->createStructure(
        structurePtr->getID(),newFieldNames,fields);
    replaceField(field);
    structurePtr = static_pointer_cast<const Structure>(field);
    StringArray fieldNames = structurePtr->getFieldNames();
    for(size_t i=0; i<newLength; i++) {
        pvFields[i]->setParentAndName(this,fieldNames[i]);
    }
}

PVBooleanPtr PVStructure::getBooleanField(String const &fieldName)
{
    return getSubField<PVBoolean>(fieldName);
}

PVBytePtr PVStructure::getByteField(String const &fieldName)
{
    return getSubField<PVByte>(fieldName);
}

PVShortPtr PVStructure::getShortField(String const &fieldName)
{
    return getSubField<PVShort>(fieldName);
}

PVIntPtr PVStructure::getIntField(String const &fieldName)
{
    return getSubField<PVInt>(fieldName);
}

PVLongPtr PVStructure::getLongField(String const &fieldName)
{
    return getSubField<PVLong>(fieldName);
}

PVUBytePtr PVStructure::getUByteField(String const &fieldName)
{
    return getSubField<PVUByte>(fieldName);
}

PVUShortPtr PVStructure::getUShortField(String const &fieldName)
{
    return getSubField<PVUShort>(fieldName);
}

PVUIntPtr PVStructure::getUIntField(String const &fieldName)
{
    return getSubField<PVUInt>(fieldName);
}

PVULongPtr PVStructure::getULongField(String const &fieldName)
{
    return getSubField<PVULong>(fieldName);
}

PVFloatPtr PVStructure::getFloatField(String const &fieldName)
{
    return getSubField<PVFloat>(fieldName);
}

PVDoublePtr PVStructure::getDoubleField(String const &fieldName)
{
    return getSubField<PVDouble>(fieldName);
}

PVStringPtr PVStructure::getStringField(String const &fieldName)
{
    return getSubField<PVString>(fieldName);
}

PVStructurePtr PVStructure::getStructureField(String const &fieldName)
{
    return getSubField<PVStructure>(fieldName);
}

PVUnionPtr PVStructure::getUnionField(String const &fieldName)
{
    return getSubField<PVUnion>(fieldName);
}

PVScalarArrayPtr PVStructure::getScalarArrayField(
    String const &fieldName,ScalarType elementType)
{
    PVFieldPtr pvField  = findSubField(fieldName,this);
    if(pvField.get()==NULL) {
        return nullPVScalarArray;
    }
    FieldConstPtr field = pvField->getField();
    Type type = field->getType();
    if(type!=scalarArray) {
        return nullPVScalarArray;
    }
    ScalarArrayConstPtr pscalarArray
        = static_pointer_cast<const ScalarArray>(pvField->getField());
    if(pscalarArray->getElementType()!=elementType) {
        return nullPVScalarArray;
    }
    return std::tr1::static_pointer_cast<PVScalarArray>(pvField);
}

PVStructureArrayPtr PVStructure::getStructureArrayField(
    String const &fieldName)
{
    return getSubField<PVStructureArray>(fieldName);
}

PVUnionArrayPtr PVStructure::getUnionArrayField(
    String const &fieldName)
{
    return getSubField<PVUnionArray>(fieldName);
}

String PVStructure::getExtendsStructureName() const
{
    return extendsStructureName;
}

bool PVStructure::putExtendsStructureName(
    String const &xxx)
{
    if(extendsStructureName.length()!=0) return false;
    extendsStructureName = xxx;
    return true;
}

void PVStructure::serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher) const {
    size_t fieldsSize = pvFields.size();
    for(size_t i = 0; i<fieldsSize; i++)
        pvFields[i]->serialize(pbuffer, pflusher);
}

void PVStructure::deserialize(ByteBuffer *pbuffer,
        DeserializableControl *pcontrol) {
    size_t fieldsSize = pvFields.size();
    for(size_t i = 0; i<fieldsSize; i++)
        pvFields[i]->deserialize(pbuffer, pcontrol);

}

void PVStructure::serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher, BitSet *pbitSet) const {
	PVStructure* nonConstThis = const_cast<PVStructure*>(this);
    size_t numberFields = nonConstThis->getNumberFields();
    size_t offset = nonConstThis->getFieldOffset();
    int32 next = pbitSet->nextSetBit(static_cast<uint32>(offset));

    // no more changes or no changes in this structure
    if(next<0||next>=static_cast<int32>(offset+numberFields)) return;

    // entire structure
    if(static_cast<int32>(offset)==next) {
        serialize(pbuffer, pflusher);
        return;
    }

    size_t fieldsSize = pvFields.size();
    for(size_t i = 0; i<fieldsSize; i++) {
        PVFieldPtr pvField = pvFields[i];
        offset = pvField->getFieldOffset();
        int32 inumberFields = static_cast<int32>(pvField->getNumberFields());
        next = pbitSet->nextSetBit(static_cast<uint32>(offset));

        // no more changes
        if(next<0) return;
        //  no change in this pvField
        if(next>=static_cast<int32>(offset+inumberFields)) continue;

        // serialize field or fields
        if(inumberFields==1) {
            pvField->serialize(pbuffer, pflusher);
        } else {
            PVStructurePtr pvStructure = std::tr1::static_pointer_cast<PVStructure>(pvField);
            pvStructure->serialize(pbuffer, pflusher, pbitSet);
       }
    }
}

void PVStructure::deserialize(ByteBuffer *pbuffer,
        DeserializableControl *pcontrol, BitSet *pbitSet) {
    size_t offset = getFieldOffset();
    size_t numberFields = getNumberFields();
    int32 next = pbitSet->nextSetBit(static_cast<uint32>(offset));

    // no more changes or no changes in this structure
    if(next<0||next>=static_cast<int32>(offset+numberFields)) return;

    // entire structure
    if(static_cast<int32>(offset)==next) {
        deserialize(pbuffer, pcontrol);
        return;
    }

    size_t fieldsSize = pvFields.size();
    for(size_t i = 0; i<fieldsSize; i++) {
        PVFieldPtr pvField = pvFields[i];
        offset = pvField->getFieldOffset();
        int32 inumberFields = static_cast<int32>(pvField->getNumberFields());
        next = pbitSet->nextSetBit(static_cast<uint32>(offset));
        // no more changes
        if(next<0) return;
        //  no change in this pvField
        if(next>=static_cast<int32>(offset+inumberFields)) continue;

        // deserialize field or fields
        if(inumberFields==1) {
            pvField->deserialize(pbuffer, pcontrol);
        } else {
            PVStructurePtr pvStructure = std::tr1::static_pointer_cast<PVStructure>(pvField);
            pvStructure->deserialize(pbuffer, pcontrol, pbitSet);
        }
    }
}

static PVFieldPtr findSubField(
    String const & fieldName,
    PVStructure const *pvStructure)
{
    if( fieldName.length()<1) return PVFieldPtr();
    String::size_type index = fieldName.find('.');
    String name = fieldName;
    String restOfName = String();
    if(index>0) {
        name = fieldName.substr(0, index);
        if(fieldName.length()>index) {
            restOfName = fieldName.substr(index+1);
        }
    }
    PVFieldPtrArray  pvFields = pvStructure->getPVFields();
    PVFieldPtr pvField;
    size_t numFields = pvStructure->getStructure()->getNumberFields();
    for(size_t i=0; i<numFields; i++) {
        pvField = pvFields[i];
        size_t result = pvField->getFieldName().compare(name);
        if(result==0) {
            if(restOfName.length()==0) return pvFields[i];
            if(pvField->getField()->getType()!=structure) return PVFieldPtr();
            PVStructurePtr pvStructure =
                std::tr1::static_pointer_cast<PVStructure>(pvField);
            return findSubField(restOfName,pvStructure.get());
        }
    }
    return PVFieldPtr();
}

std::ostream& PVStructure::dumpValue(std::ostream& o) const
{
    o << format::indent() << getStructure()->getID() << ' ' << getFieldName();
    String extendsName = getExtendsStructureName();
    if(extendsName.length()>0) {
        o << " extends " << extendsName;
    }
    o << std::endl;
    {
    	format::indent_scope s(o);

		PVFieldPtrArray const & fieldsData = getPVFields();
		if (fieldsData.size() != 0) {
			size_t length = getStructure()->getNumberFields();
			for(size_t i=0; i<length; i++) {
				PVFieldPtr fieldField = fieldsData[i];
				Type type = fieldField->getField()->getType();
				if (type == scalar || type == scalarArray)
					o << format::indent() << fieldField->getField()->getID() << ' ' << fieldField->getFieldName() << ' ' << *(fieldField.get()) << std::endl;
				else
					o << *(fieldField.get());
			}
		}
    }
 	return o;
}

}}
