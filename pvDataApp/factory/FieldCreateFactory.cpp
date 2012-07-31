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


// TODO move all these to a header file

struct ScalarHashFunction {
    size_t operator() (const Scalar& scalar) const { return scalar.getScalarType(); }
};

struct ScalarArrayHashFunction {
    size_t operator() (const ScalarArray& scalarArray) const { return 0x10 | scalarArray.getElementType(); }
};

struct StructureHashFunction {
    size_t operator() (const Structure& structure) const { return 0; }
    // TODO
//        final int PRIME = 31;
//        return PRIME * Arrays.hashCode(fieldNames) + Arrays.hashCode(fields);
};

struct StructureArrayHashFunction {
    size_t operator() (const StructureArray& structureArray) const { StructureHashFunction shf; return (0x10 | shf(*(structureArray.getStructure()))); }
};


Scalar::Scalar(ScalarType scalarType)
       : Field(scalar),scalarType(scalarType){}

Scalar::~Scalar(){}

void Scalar::toString(StringBuilder buffer,int indentLevel) const{
    ScalarTypeFunc::toString(buffer,scalarType);
}

static const String idScalarLUT[] = {
	"boolean", // pvBoolean
	"byte",    // pvByte
	"short",   // pvShort
	"int",     // pvInt
	"long",    // pvLong
	"ubyte",   // pvUByte
	"ushort",  // pvUShort
	"uint",    // pvUInt
	"ulong",   // pvULong
	"float",   // pvFloat
	"double",  // pvDouble
	"string"   // pvString
};

String Scalar::getID() const
{
    return idScalarLUT[scalarType];
}

static const int8 typeCodeLUT[] = {
    0x00, // pvBoolean
    0x20, // pvByte
    0x21, // pvShort
    0x22, // pvInt
    0x23, // pvLong
    0x28, // pvUByte
    0x29, // pvUShort
    0x2A, // pvUInt
    0x2B, // pvULong
    0x42, // pvFloat
    0x43, // pvDouble
    0x60  // pvString
};

void Scalar::serialize(ByteBuffer *buffer, SerializableControl *control) const {
    control->ensureBuffer(1);
    buffer->putByte(typeCodeLUT[scalarType]);
}

void Scalar::deserialize(ByteBuffer *buffer, DeserializableControl *control) {
    // must be done via FieldCreate
    throw std::runtime_error("not valid operation, use FieldCreate::deserialize instead");
}

static void serializeStructureField(const Structure* structure, ByteBuffer* buffer, SerializableControl* control)
{
    SerializeHelper::serializeString(structure->getID(), buffer, control);
    FieldConstPtrArray const & fields = structure->getFields();
    StringArray const & fieldNames = structure->getFieldNames();
    std::size_t len = fields.size();
    SerializeHelper::writeSize(len, buffer, control);
    for (std::size_t i = 0; i < len; i++)
    {
        SerializeHelper::serializeString(fieldNames[i], buffer, control);
        control->cachedSerialize(fields[i], buffer);
    }
}

static StructureConstPtr deserializeStructureField(const FieldCreate* fieldCreate, ByteBuffer* buffer, DeserializableControl* control)
{
    String id = SerializeHelper::deserializeString(buffer, control);
    const std::size_t size = SerializeHelper::readSize(buffer, control);
    FieldConstPtrArray fields; fields.reserve(size);
    StringArray fieldNames; fieldNames.reserve(size);
    for (std::size_t i = 0; i < size; i++)
    {
        fieldNames.push_back(SerializeHelper::deserializeString(buffer, control));
        fields.push_back(control->cachedDeserialize(buffer));
    }

    return fieldCreate->createStructure(id, fieldNames, fields);
}

ScalarArray::ScalarArray(ScalarType elementType)
: Field(scalarArray),elementType(elementType){}

ScalarArray::~ScalarArray() {}

static const String idScalarArrayLUT[] = {
	"boolean[]", // pvBoolean
	"byte[]",    // pvByte
	"short[]",   // pvShort
	"int[]",     // pvInt
	"long[]",    // pvLong
	"ubyte[]",   // pvUByte
	"ushort[]",  // pvUShort
	"uint[]",    // pvUInt
	"ulong[]",   // pvULong
	"float[]",   // pvFloat
	"double[]",  // pvDouble
	"string[]"   // pvString
};

String ScalarArray::getID() const
{
    return idScalarArrayLUT[elementType];
}

void ScalarArray::toString(StringBuilder buffer,int indentLevel) const{
    ScalarTypeFunc::toString(buffer,elementType);
    *buffer += "[]";
}

void ScalarArray::serialize(ByteBuffer *buffer, SerializableControl *control) const {
    control->ensureBuffer(1);
    buffer->putByte(0x10 | typeCodeLUT[elementType]);
}

void ScalarArray::deserialize(ByteBuffer *buffer, DeserializableControl *control) {
    throw std::runtime_error("not valid operation, use FieldCreate::deserialize instead");
}

StructureArray::StructureArray(StructureConstPtr const & structure)
: Field(structureArray),pstructure(structure)
{
}

StructureArray::~StructureArray() {
    if(debugLevel==highDebug) printf("~StructureArray\n");
}

String StructureArray::getID() const
{
	// NOTE: structure->getID() can return an empty string
	return pstructure->getID() + "[]";
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
    control->ensureBuffer(1);
    buffer->putByte(0x90);
    control->cachedSerialize(pstructure, buffer);
}

void StructureArray::deserialize(ByteBuffer *buffer, DeserializableControl *control) {
    throw std::runtime_error("not valid operation, use FieldCreate::deserialize instead");
}

Structure::Structure (StringArray const & fieldNames,FieldConstPtrArray const & infields, String inid)
: Field(structure),
      fieldNames(fieldNames),
      fields(infields),
      id(inid)
{
    if(fieldNames.size()!=fields.size()) {
        throw std::invalid_argument("fieldNames.size()!=fields.size()");
    }
    size_t number = fields.size();
    for(size_t i=0; i<number; i++) {
        String name = fieldNames[i];
        if(name.size()<1) {
            throw std::invalid_argument("fieldNames has a zero length string");
        }
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


String Structure::getID() const
{
	return id;
}

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
    if (!id.empty()) { *buffer += ' '; *buffer += id; };
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
    control->ensureBuffer(1);
    buffer->putByte(0x80);
    serializeStructureField(this, buffer, control);
}

void Structure::deserialize(ByteBuffer *buffer, DeserializableControl *control) {
    throw std::runtime_error("not valid operation, use FieldCreate::deserialize instead");
}

ScalarConstPtr FieldCreate::createScalar(ScalarType scalarType) const
{
    // TODO use singleton instance
    ScalarConstPtr scalar(new Scalar(scalarType), Field::Deleter());
    return scalar;
}
 
ScalarArrayConstPtr FieldCreate::createScalarArray(ScalarType elementType) const
{
    // TODO use singleton instance
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

StructureConstPtr FieldCreate::createStructure (
	String id,
    StringArray const & fieldNames,FieldConstPtrArray const & fields) const
{
      StructureConstPtr structure(
         new Structure(fieldNames,fields,id), Field::Deleter());
      return structure;
}

StructureArrayConstPtr FieldCreate::createStructureArray(
    StructureConstPtr const & structure) const
{
     StructureArrayConstPtr structureArray(
        new StructureArray(structure), Field::Deleter());
     return structureArray;
}

StructureConstPtr FieldCreate::appendField(
    StructureConstPtr const & structure,String fieldName, FieldConstPtr const & field) const
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
    StructureConstPtr const & structure,
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





static const int integerLUT[] =
{
    pvByte,  // 8-bits
    pvShort, // 16-bits
    pvInt,   // 32-bits
    pvLong,  // 64-bits
    -1,
    -1,
    -1,
    -1,
    pvUByte,  // unsigned 8-bits
    pvUShort, // unsigned 16-bits
    pvUInt,   // unsigned 32-bits
    pvULong,  // unsigned 64-bits
    -1,
    -1,
    -1,
    -1
};

static const int floatLUT[] =
{
    -1, // reserved
    -1, // 16-bits
    pvFloat,   // 32-bits
    pvDouble,  // 64-bits
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1
};

static int decodeScalar(int8 code)
{
    // bits 7-5
    switch (code >> 5)
    {
    case 0: return pvBoolean;
    case 1: return integerLUT[code & 0x0F];
    case 2: return floatLUT[code & 0x0F];
    case 3: return pvString;
    default: return -1;
    }
}

FieldConstPtr FieldCreate::deserialize(ByteBuffer* buffer, DeserializableControl* control) const
{
    control->ensureData(1);
    int8 code = buffer->getByte();
    if (code == -1)
        return FieldConstPtr();

    int typeCode = code & 0xE0;
    bool notArray = ((code & 0x10) == 0);
    if (notArray)
    {
        if (typeCode < 0x80)
        {
            // Type type = Type.scalar;
            int scalarType = decodeScalar(code);
            if (scalarType == -1)
                throw std::invalid_argument("invalid scalar type encoding");
            return FieldConstPtr(new Scalar(static_cast<ScalarType>(scalarType)), Field::Deleter());
        }
        else if (typeCode == 0x80)
        {
            // Type type = Type.structure;
            return deserializeStructureField(this, buffer, control);
        }
        else
            throw std::invalid_argument("invalid type encoding");
    }
    else // array
    {
        if (typeCode < 0x80)
        {
            // Type type = Type.scalarArray;
            int scalarType = decodeScalar(code);
            if (scalarType == -1)
                throw std::invalid_argument("invalid scalarArray type encoding");
            return FieldConstPtr(new ScalarArray(static_cast<ScalarType>(scalarType)), Field::Deleter());
        }
        else if (typeCode == 0x80)
        {
            // Type type = Type.structureArray;
            StructureConstPtr elementStructure = std::tr1::static_pointer_cast<const Structure>(control->cachedDeserialize(buffer));
            return FieldConstPtr(new StructureArray(elementStructure), Field::Deleter());
        }
        else
            throw std::invalid_argument("invalid type encoding");
    }
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
