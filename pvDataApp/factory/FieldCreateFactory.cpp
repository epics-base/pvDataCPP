/*FieldCreateFactory.cpp*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <pv/lock.h>
#include <pv/pvIntrospect.h>
#include <pv/convert.h>
#include <pv/factory.h>
#include <pv/serializeHelper.h>

using std::tr1::static_pointer_cast;
using std::size_t;

namespace epics { namespace pvData {

static DebugLevel debugLevel = lowDebug;

static void newLine(StringBuilder buffer, int indentLevel)
{
    *buffer += "\n";
    for(int i=0; i<indentLevel; i++) *buffer += "    ";
}

Field::Field(Type type)
    : m_fieldType(type)
{
}

Field::~Field() {
}


void Field::toString(StringBuilder buffer,int indentLevel) const{
}

Scalar::Scalar(ScalarType scalarType)
       : Field(scalar),scalarType(scalarType){}

Scalar::~Scalar(){}

void Scalar::toString(StringBuilder buffer,int indentLevel) const{
    ScalarTypeFunc::toString(buffer,scalarType);
}

void Scalar::serialize(ByteBuffer *buffer, SerializableControl *control) const {
/*
    control->ensureBuffer(1);
    buffer->putByte((int8)(epics::pvData::scalar << 4 | getScalarType()));
    SerializeHelper::serializeString(getFieldName(), buffer, control);
*/
}

void Scalar::deserialize(ByteBuffer *buffer, DeserializableControl *control) {
}

static void serializeStructureField(const Structure* structure, ByteBuffer* buffer, SerializableControl* control)
{
/*
	SerializeHelper::serializeString(structure->getFieldName(), buffer, control);
	FieldConstPtrArray fields = structure->getFields();
	SerializeHelper::writeSize(structure->getNumberFields(), buffer, control);
	for (int i = 0; i < structure->getNumberFields(); i++)
	{
		control->cachedSerialize(fields[i], buffer);
	}
*/
}

static StructureConstPtr deserializeStructureField(const FieldCreate* fieldCreate, ByteBuffer* buffer, DeserializableControl* control)
{
throw std::invalid_argument("for Matej to convert");
/*
	const String structureFieldName = SerializeHelper::deserializeString(buffer, control);
	const int32 size = SerializeHelper::readSize(buffer, control);
	FieldConstPtrArray fields = NULL;
	if (size > 0)
	{
		fields = new FieldConstPtr[size];
		for(int i = 0; i < size; i++)
		{
		  try {
			fields[i] = control->cachedDeserialize(buffer);
		  } catch (...) {
		      delete[] fields;
		      throw;
		  }
		}
	}

	StructureConstPtr structure = fieldCreate->createStructure(structureFieldName, size, fields);
	return structure;
*/
}

ScalarArray::ScalarArray(ScalarType elementType)
: Field(scalarArray),elementType(elementType){}

ScalarArray::~ScalarArray() {}

void ScalarArray::toString(StringBuilder buffer,int indentLevel) const{
    ScalarTypeFunc::toString(buffer,elementType);
    *buffer += "[]";
}

void ScalarArray::serialize(ByteBuffer *buffer, SerializableControl *control) const {
/*
    control->ensureBuffer(1);
	buffer->putByte((int8)(epics::pvData::scalarArray << 4 | getElementType()));
    SerializeHelper::serializeString(getFieldName(), buffer, control);
*/
}

void ScalarArray::deserialize(ByteBuffer *buffer, DeserializableControl *control) {
}

StructureArray::StructureArray(StructureConstPtr structure)
: Field(structureArray),pstructure(structure)
{
}

StructureArray::~StructureArray() {
    if(debugLevel==highDebug) printf("~StructureArray\n");
}

void StructureArray::toString(StringBuilder buffer,int indentLevel) const {
    if(indentLevel==0) {
        *buffer +=  "structure[]";
        newLine(buffer,indentLevel + 1);
        pstructure->toString(buffer,indentLevel + 1);
        return;
    }
    pstructure->toString(buffer,indentLevel);
}

void StructureArray::serialize(ByteBuffer *buffer, SerializableControl *control) const {
/*
    control->ensureBuffer(1);
    buffer->putByte((int8)(epics::pvData::structureArray << 4));
    SerializeHelper::serializeString(getFieldName(), buffer, control);
    // we also need to serialize structure field...
    serializeStructureField(getStructure().get(), buffer, control);
*/
}

void StructureArray::deserialize(ByteBuffer *buffer, DeserializableControl *control) {
}


Structure::Structure (StringArray fieldNames,FieldConstPtrArray infields)
: Field(structure),
      fieldNames(fieldNames),
      fields(infields)
{
    if(fieldNames.size()!=fields.size()) {
        throw std::invalid_argument("fieldNames.size()!=fields.size()");
    }
    size_t number = fields.size();
    for(size_t i=0; i<number; i++) {
        String name = fieldNames[i];
        // look for duplicates
        for(size_t j=i+1; j<number; j++) {
            String otherName = fieldNames[j];
            int result = name.compare(otherName);
            if(result==0) {
                String  message("duplicate fieldName ");
                message += name;
                throw std::invalid_argument(message);
            }
        }
    }
}

Structure::~Structure() { }


FieldConstPtr  Structure::getField(String fieldName) const {
    size_t numberFields = fields.size();
    for(size_t i=0; i<numberFields; i++) {
        FieldConstPtr pfield = fields[i];
        int result = fieldName.compare(fieldNames[i]);
        if(result==0) return pfield;
    }
    return FieldConstPtr();
}

size_t Structure::getFieldIndex(String fieldName) const {
    size_t numberFields = fields.size();
    for(size_t i=0; i<numberFields; i++) {
        FieldConstPtr pfield = fields[i];
        int result = fieldName.compare(fieldNames[i]);
        if(result==0) return i;
    }
    return -1;
}

void Structure::toString(StringBuilder buffer,int indentLevel) const{
    *buffer += "structure";
     toStringCommon(buffer,indentLevel+1);
}
    
void Structure::toStringCommon(StringBuilder buffer,int indentLevel) const{
    newLine(buffer,indentLevel);
    size_t numberFields = fields.size();
    for(size_t i=0; i<numberFields; i++) {
        FieldConstPtr pfield = fields[i];
        switch(pfield->getType()) {
            case scalar:
            case scalarArray:
                pfield->toString(buffer, indentLevel);
                *buffer += " ";
                *buffer += fieldNames[i];
                break;
            case structure:
            {
                Field const *xxx = pfield.get();
                Structure const *pstruct = static_cast<Structure const*>(xxx);
                *buffer += "structure ";
                *buffer += fieldNames[i];
                pstruct->toStringCommon(buffer,indentLevel + 1);
                break;
            }
            case structureArray:
                *buffer += "structure[] " + fieldNames[i];
                newLine(buffer,indentLevel +1);
                pfield->toString(buffer,indentLevel +1);
                break;
        }
        if(i<numberFields-1) newLine(buffer,indentLevel);
    }
}

void Structure::serialize(ByteBuffer *buffer, SerializableControl *control) const {
/*
    control->ensureBuffer(1);
    buffer->putByte((int8)(epics::pvData::structure << 4));
    serializeStructureField(this, buffer, control);
*/
}

void Structure::deserialize(ByteBuffer *buffer, DeserializableControl *control) {
}

ScalarConstPtr  FieldCreate::createScalar(ScalarType scalarType) const
{
     ScalarConstPtr scalar(new Scalar(scalarType), Field::Deleter());
     return scalar;
}
 
ScalarArrayConstPtr FieldCreate::createScalarArray(ScalarType elementType) const
{
      ScalarArrayConstPtr scalarArray(new ScalarArray(elementType), Field::Deleter());
      return scalarArray;
}

StructureConstPtr FieldCreate::createStructure (
    StringArray const & fieldNames,FieldConstPtrArray const & fields) const
{
      StructureConstPtr structure(
         new Structure(fieldNames,fields), Field::Deleter());
      return structure;
}

StructureArrayConstPtr FieldCreate::createStructureArray(
    StructureConstPtr structure) const
{
     StructureArrayConstPtr structureArray(
        new StructureArray(structure), Field::Deleter());
     return structureArray;
}

StructureConstPtr FieldCreate::appendField(
    StructureConstPtr structure,String fieldName, FieldConstPtr field) const
{
    StringArray oldNames = structure->getFieldNames();
    FieldConstPtrArray oldFields = structure->getFields();
    size_t oldLen = oldNames.size();
    StringArray newNames(oldLen+1);
    FieldConstPtrArray newFields(oldLen+1);
    for(size_t i = 0; i<oldLen; i++) {
        newNames[i] = oldNames[i];
        newFields[i] = oldFields[i];
    }
    newNames[oldLen] = fieldName;
    newFields[oldLen] = field;
    return createStructure(newNames,newFields);
}

StructureConstPtr FieldCreate::appendFields(
    StructureConstPtr structure,
    StringArray const & fieldNames,
    FieldConstPtrArray const & fields) const
{
    StringArray oldNames = structure->getFieldNames();
    FieldConstPtrArray oldFields = structure->getFields();
    size_t oldLen = oldNames.size();
    size_t extra = fieldNames.size();
    StringArray newNames(oldLen+extra);
    FieldConstPtrArray newFields(oldLen+extra);
    for(size_t i = 0; i<oldLen; i++) {
        newNames[i] = oldNames[i];
        newFields[i] = oldFields[i];
    }
    for(size_t i = 0; i<extra; i++) {
        newNames[oldLen +i] = fieldNames[i];
        newFields[oldLen +i] = fields[i];
    }
    return createStructure(newNames,newFields);
}

FieldConstPtr FieldCreate::deserialize(ByteBuffer* buffer, DeserializableControl* control) const
{
throw std::invalid_argument("for Matej to convert");
/*

	control->ensureData(1);
	const int8 typeCode = buffer->getByte();

	// high nibble means scalar/array/structure
	const Type type = (Type)(typeCode >> 4);
	switch (type)
	{
	case scalar:
	{
		const ScalarType scalar = (ScalarType)(typeCode & 0x0F);
		const String scalarFieldName = SerializeHelper::deserializeString(buffer, control);
		return static_cast<FieldConstPtr>(createScalar(scalarFieldName,scalar));
	}
	case scalarArray:
	{
		const ScalarType element = (ScalarType)(typeCode & 0x0F);
		const String arrayFieldName = SerializeHelper::deserializeString(buffer, control);
		return static_cast<FieldConstPtr>(createScalarArray(arrayFieldName,element));
	}
	case structure:
	{
		return static_cast<FieldConstPtr>(deserializeStructureField(this, buffer, control));
	}
	case structureArray:
	{
		const String structureArrayFieldName = SerializeHelper::deserializeString(buffer, control);
		const StructureConstPtr arrayElement = deserializeStructureField(this, buffer, control);
		return  static_cast<FieldConstPtr>(createStructureArray(structureArrayFieldName, arrayElement));
	}
	default:
	{
	   // TODO log
       return FieldConstPtr();
	}
	}
*/
}

FieldCreatePtr FieldCreate::getFieldCreate()
{
    static FieldCreatePtr fieldCreate;
    static Mutex mutex;
    Lock xx(mutex);

    if(fieldCreate.get()==0) fieldCreate = FieldCreatePtr(new FieldCreate());
    return fieldCreate;
}

FieldCreate::FieldCreate(){}

FieldCreatePtr getFieldCreate() {
    return FieldCreate::getFieldCreate();
}

}}
