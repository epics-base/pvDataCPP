/*FieldCreateFactory.cpp*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */

#ifdef _WIN32
#define NOMINMAX
#endif

#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <stdexcept>

#define epicsExportSharedSymbols
#include <pv/lock.h>
#include <pv/pvIntrospect.h>
#include <pv/convert.h>
#include <pv/factory.h>
#include <pv/serializeHelper.h>

using std::tr1::static_pointer_cast;
using std::size_t;
using std::string;

namespace epics { namespace pvData {

static DebugLevel debugLevel = lowDebug;

Field::Field(Type type)
    : m_fieldType(type)
{
}

Field::~Field() {
}


std::ostream& operator<<(std::ostream& o, const Field& f)
{
	return f.dump(o);
};

Scalar::Scalar(ScalarType scalarType)
       : Field(scalar),scalarType(scalarType)
{
    if(scalarType<0 || scalarType>MAX_SCALAR_TYPE)
        throw std::invalid_argument("Can't construct Scalar from invalid ScalarType");
}

Scalar::~Scalar(){}

std::ostream& Scalar::dump(std::ostream& o) const
{
    return o << format::indent() << getID();
}

string Scalar::getID() const
{
    static const string idScalarLUT[] = {
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
    return idScalarLUT[scalarType];
}

int8 Scalar::getTypeCodeLUT() const
{
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
   return typeCodeLUT[scalarType];
}


void Scalar::serialize(ByteBuffer *buffer, SerializableControl *control) const {
    control->ensureBuffer(1);
    buffer->putByte(getTypeCodeLUT());
}

void Scalar::deserialize(ByteBuffer* /*buffer*/, DeserializableControl* /*control*/) {
    // must be done via FieldCreate
    throw std::runtime_error("not valid operation, use FieldCreate::deserialize instead");
}

static string emptyStringtring;

static void serializeStructureField(const Structure* structure, ByteBuffer* buffer, SerializableControl* control)
{
	// to optimize default (non-empty) IDs optimization
	// empty IDs are not allowed
	string id = structure->getID();
	if (id == Structure::DEFAULT_ID)	// TODO slow comparison
		SerializeHelper::serializeString(emptyStringtring, buffer, control);
	else
	    SerializeHelper::serializeString(id, buffer, control);

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
    string id = SerializeHelper::deserializeString(buffer, control);
    const std::size_t size = SerializeHelper::readSize(buffer, control);
    FieldConstPtrArray fields; fields.reserve(size);
    StringArray fieldNames; fieldNames.reserve(size);
    for (std::size_t i = 0; i < size; i++)
    {
        fieldNames.push_back(SerializeHelper::deserializeString(buffer, control));
        fields.push_back(control->cachedDeserialize(buffer));
    }

    if (id.empty())
        return fieldCreate->createStructure(fieldNames, fields);
    else
    	return fieldCreate->createStructure(id, fieldNames, fields);
}

static void serializeUnionField(const Union* punion, ByteBuffer* buffer, SerializableControl* control)
{
	// to optimize default (non-empty) IDs optimization
	// empty IDs are not allowed
	string id = punion->getID();
	if (id == Union::DEFAULT_ID)	// TODO slow comparison
		SerializeHelper::serializeString(emptyStringtring, buffer, control);
	else
	    SerializeHelper::serializeString(id, buffer, control);

    FieldConstPtrArray const & fields = punion->getFields();
    StringArray const & fieldNames = punion->getFieldNames();
    std::size_t len = fields.size();
    SerializeHelper::writeSize(len, buffer, control);
    for (std::size_t i = 0; i < len; i++)
    {
        SerializeHelper::serializeString(fieldNames[i], buffer, control);
        control->cachedSerialize(fields[i], buffer);
    }
}

static UnionConstPtr deserializeUnionField(const FieldCreate* fieldCreate, ByteBuffer* buffer, DeserializableControl* control)
{
    string id = SerializeHelper::deserializeString(buffer, control);
    const std::size_t size = SerializeHelper::readSize(buffer, control);
    FieldConstPtrArray fields; fields.reserve(size);
    StringArray fieldNames; fieldNames.reserve(size);
    for (std::size_t i = 0; i < size; i++)
    {
        fieldNames.push_back(SerializeHelper::deserializeString(buffer, control));
        fields.push_back(control->cachedDeserialize(buffer));
    }

    if (id.empty())
        return fieldCreate->createUnion(fieldNames, fields);
    else
    	return fieldCreate->createUnion(id, fieldNames, fields);
}

Array::Array(Type type)
    : Field(type)
{
}

Array::~Array() {}

ScalarArray::ScalarArray(ScalarType elementType)
: Array(scalarArray),elementType(elementType)
{
    if(elementType<0 || elementType>MAX_SCALAR_TYPE)
        throw std::invalid_argument("Can't construct ScalarArray from invalid ScalarType");
}

ScalarArray::~ScalarArray() {}

int8 ScalarArray::getTypeCodeLUT() const
{
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
   return typeCodeLUT[elementType];
}

const string ScalarArray::getIDScalarArrayLUT() const
{
    static const string idScalarArrayLUT[] = {
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
    return idScalarArrayLUT[elementType];
}

string ScalarArray::getID() const
{
    return getIDScalarArrayLUT();
}

std::ostream& ScalarArray::dump(std::ostream& o) const
{
    return o << format::indent() << getID();
}

void ScalarArray::serialize(ByteBuffer *buffer, SerializableControl *control) const {
    control->ensureBuffer(1);
    buffer->putByte((int8)0x10 | getTypeCodeLUT());
}

void ScalarArray::deserialize(ByteBuffer* /*buffer*/, DeserializableControl* /*control*/) {
    throw std::runtime_error("not valid operation, use FieldCreate::deserialize instead");
}

StructureArray::StructureArray(StructureConstPtr const & structure)
: Array(structureArray),pstructure(structure)
{
}

StructureArray::~StructureArray() {
    if(debugLevel==highDebug) printf("~StructureArray\n");
}

string StructureArray::getID() const
{
	return pstructure->getID() + "[]";
}

std::ostream& StructureArray::dump(std::ostream& o) const
{
    o << format::indent() << getID() << std::endl;
    {
        format::indent_scope s(o);
        o << *pstructure;
    }
    return o;
}

void StructureArray::serialize(ByteBuffer *buffer, SerializableControl *control) const {
    control->ensureBuffer(1);
    buffer->putByte((int8)0x90);
    control->cachedSerialize(pstructure, buffer);
}

void StructureArray::deserialize(ByteBuffer* /*buffer*/, DeserializableControl* /*control*/) {
    throw std::runtime_error("not valid operation, use FieldCreate::deserialize instead");
}

UnionArray::UnionArray(UnionConstPtr const & _punion)
: Array(unionArray),punion(_punion)
{
}

UnionArray::~UnionArray() {
    if(debugLevel==highDebug) printf("~UnionArray\n");
}

string UnionArray::getID() const
{
	return punion->getID() + "[]";
}

std::ostream& UnionArray::dump(std::ostream& o) const
{
    o << format::indent() << getID() << std::endl;
    {
        format::indent_scope s(o);
        o << *punion;
    }
    return o;
}

void UnionArray::serialize(ByteBuffer *buffer, SerializableControl *control) const {
    control->ensureBuffer(1);
    if (punion->isVariant())
    {
        // unrestricted/variant union
        buffer->putByte((int8)0x92);
    }
    else
    {
        buffer->putByte((int8)0x91);
        control->cachedSerialize(punion, buffer);
    }
}

void UnionArray::deserialize(ByteBuffer* /*buffer*/, DeserializableControl* /*control*/) {
    throw std::runtime_error("not valid operation, use FieldCreate::deserialize instead");
}

string Structure::DEFAULT_ID = "structure";

Structure::Structure (
    StringArray const & fieldNames,
    FieldConstPtrArray const & infields,
    string const & inid)
: Field(structure),
      fieldNames(fieldNames),
      fields(infields),
      id(inid)
{
    if(inid.empty()) {
        throw std::invalid_argument("id is empty");
    }
    if(fieldNames.size()!=fields.size()) {
        throw std::invalid_argument("fieldNames.size()!=fields.size()");
    }
    size_t number = fields.size();
    for(size_t i=0; i<number; i++) {
        const string& name = fieldNames[i];
        if(name.empty()) {
            throw std::invalid_argument("fieldNames has a zero length string");
        }
        if(fields[i].get()==NULL)
            throw std::invalid_argument("Can't construct Structure with NULL Field");
        // look for duplicates
        for(size_t j=i+1; j<number; j++) {
            string otherName = fieldNames[j];
            int result = name.compare(otherName);
            if(result==0) {
                string  message("duplicate fieldName ");
                message += name;
                throw std::invalid_argument(message);
            }
        }
    }
}

Structure::~Structure() { }


string Structure::getID() const
{
	return id;
}

FieldConstPtr  Structure::getField(string const & fieldName) const {
    size_t numberFields = fields.size();
    for(size_t i=0; i<numberFields; i++) {
        FieldConstPtr pfield = fields[i];
        int result = fieldName.compare(fieldNames[i]);
        if(result==0) return pfield;
    }
    return FieldConstPtr();
}

size_t Structure::getFieldIndex(string const &fieldName) const {
    size_t numberFields = fields.size();
    for(size_t i=0; i<numberFields; i++) {
        FieldConstPtr pfield = fields[i];
        int result = fieldName.compare(fieldNames[i]);
        if(result==0) return i;
    }
    return -1;
}

std::ostream& Structure::dump(std::ostream& o) const
{
    o << format::indent() << getID() << std::endl;
    {
        format::indent_scope s(o);
        dumpFields(o);
    }
    return o;
}

void Structure::dumpFields(std::ostream& o) const
{
    size_t numberFields = fields.size();
    for(size_t i=0; i<numberFields; i++) {
        FieldConstPtr pfield = fields[i];
        o << format::indent() << pfield->getID() << ' ' << fieldNames[i] << std::endl;
        switch(pfield->getType()) {
            case scalar:
            case scalarArray:
                break;
            case structure:
            {
                Field const *xxx = pfield.get();
                Structure const *pstruct = static_cast<Structure const*>(xxx);
                format::indent_scope s(o);
                pstruct->dumpFields(o);
                break;
            }
            case structureArray:
            {
                format::indent_scope s(o);
                o << *pfield;
                break;
            }
            case union_:
            {
                Field const *xxx = pfield.get();
                Union const *punion = static_cast<Union const*>(xxx);
                format::indent_scope s(o);
                punion->dumpFields(o);
                break;
            }
            case unionArray:
            {
                format::indent_scope s(o);
                o << *pfield;
                break;
            }
        }
    }
}

void Structure::serialize(ByteBuffer *buffer, SerializableControl *control) const {
    control->ensureBuffer(1);
    buffer->putByte((int8)0x80);
    serializeStructureField(this, buffer, control);
}

void Structure::deserialize(ByteBuffer* /*buffer*/, DeserializableControl* /*control*/) {
    throw std::runtime_error("not valid operation, use FieldCreate::deserialize instead");
}

string Union::DEFAULT_ID = "union";

#define UNION_ANY_ID "any"
string Union::ANY_ID = UNION_ANY_ID;

Union::Union ()
: Field(union_),
      fieldNames(),
      fields(),
      id(UNION_ANY_ID)
{
}

#undef UNION_ANY_ID

Union::Union (
    StringArray const & fieldNames,
    FieldConstPtrArray const & infields,
    string const & inid)
: Field(union_),
      fieldNames(fieldNames),
      fields(infields),
      id(inid)
{
    if(inid.empty()) {
        throw std::invalid_argument("id is empty");
    }
    if(fieldNames.size()!=fields.size()) {
        throw std::invalid_argument("fieldNames.size()!=fields.size()");
    }
    if(fields.size()==0 && inid!=ANY_ID) {
        throw std::invalid_argument("no fields but id is different than " + ANY_ID);
    }

    size_t number = fields.size();
    for(size_t i=0; i<number; i++) {
        const string& name = fieldNames[i];
        if(name.empty()) {
            throw std::invalid_argument("fieldNames has a zero length string");
        }
        if(fields[i].get()==NULL)
            throw std::invalid_argument("Can't construct Union with NULL Field");
        // look for duplicates
        for(size_t j=i+1; j<number; j++) {
            string otherName = fieldNames[j];
            int result = name.compare(otherName);
            if(result==0) {
                string  message("duplicate fieldName ");
                message += name;
                throw std::invalid_argument(message);
            }
        }
    }
}

Union::~Union() { }


string Union::getID() const
{
	return id;
}

FieldConstPtr  Union::getField(string const & fieldName) const {
    size_t numberFields = fields.size();
    for(size_t i=0; i<numberFields; i++) {
        FieldConstPtr pfield = fields[i];
        int result = fieldName.compare(fieldNames[i]);
        if(result==0) return pfield;
    }
    return FieldConstPtr();
}

size_t Union::getFieldIndex(string const &fieldName) const {
    size_t numberFields = fields.size();
    for(size_t i=0; i<numberFields; i++) {
        FieldConstPtr pfield = fields[i];
        int result = fieldName.compare(fieldNames[i]);
        if(result==0) return i;
    }
    return -1;
}

std::ostream& Union::dump(std::ostream& o) const
{
    o << format::indent() << getID() << std::endl;
    {
        format::indent_scope s(o);
        dumpFields(o);
    }
    return o;
}

void Union::dumpFields(std::ostream& o) const
{
    size_t numberFields = fields.size();
    for(size_t i=0; i<numberFields; i++) {
        FieldConstPtr pfield = fields[i];
        o << format::indent() << pfield->getID() << ' ' << fieldNames[i] << std::endl;
        switch(pfield->getType()) {
            case scalar:
            case scalarArray:
                break;
            case structure:
            {
                Field const *xxx = pfield.get();
                Structure const *pstruct = static_cast<Structure const*>(xxx);
                format::indent_scope s(o);
                pstruct->dumpFields(o);
                break;
            }
            case structureArray:
            {
                format::indent_scope s(o);
                o << *pfield;
                break;
            }
            case union_:
            {
                Field const *xxx = pfield.get();
                Union const *punion = static_cast<Union const*>(xxx);
                format::indent_scope s(o);
                punion->dumpFields(o);
                break;
            }
            case unionArray:
            {
                format::indent_scope s(o);
                o << *pfield;
                break;
            }
        }
    }
}

void Union::serialize(ByteBuffer *buffer, SerializableControl *control) const {
    control->ensureBuffer(1);
    if (fields.size() == 0)
    {
        // unrestricted/variant union
        buffer->putByte((int8)0x82);
    }
    else
    {
        buffer->putByte((int8)0x81);
        serializeUnionField(this, buffer, control);
    }
}

void Union::deserialize(ByteBuffer* /*buffer*/, DeserializableControl* /*control*/) {
    throw std::runtime_error("not valid operation, use FieldCreate::deserialize instead");
}


FieldBuilder::FieldBuilder() : fieldCreate(getFieldCreate()), idSet(false) {}

FieldBuilder::FieldBuilder(FieldBuilderPtr const & _parentBuilder,
			string const & _nestedName,
			Type _nestedClassToBuild, bool _nestedArray) :
		fieldCreate(getFieldCreate()),
		idSet(false),
		parentBuilder(_parentBuilder),
		nestedClassToBuild(_nestedClassToBuild),
		nestedName(_nestedName),
		nestedArray(_nestedArray)
{}

void FieldBuilder::reset()
{
	id.erase();
    idSet = false;
	fieldNames.clear();
	fields.clear();
}

FieldBuilderPtr FieldBuilder::setId(string const & id)
{
    this->id = id;
    idSet = true; 
    return shared_from_this();
}

FieldBuilderPtr FieldBuilder::add(string const & name, ScalarType scalarType)
{
    fields.push_back(fieldCreate->createScalar(scalarType)); fieldNames.push_back(name); 
	return shared_from_this();
}

FieldBuilderPtr FieldBuilder::add(string const & name, FieldConstPtr const & field)
{
    fields.push_back(field); fieldNames.push_back(name);
	return shared_from_this();
}

FieldBuilderPtr FieldBuilder::addArray(string const & name, ScalarType scalarType)
{
    fields.push_back(fieldCreate->createScalarArray(scalarType)); fieldNames.push_back(name);
	return shared_from_this();
}

FieldBuilderPtr FieldBuilder::addArray(string const & name, FieldConstPtr const & element)
{
    switch (element->getType())
    {
        case structure:
            fields.push_back(fieldCreate->createStructureArray(static_pointer_cast<const Structure>(element)));
            break;
        case union_:
            fields.push_back(fieldCreate->createUnionArray(static_pointer_cast<const Union>(element)));
            break;
        case scalar:
            fields.push_back(fieldCreate->createScalarArray(static_pointer_cast<const Scalar>(element)->getScalarType()));
            break;
        default:
            throw std::invalid_argument("unsupported array element type:" + element->getType());
    }
    
    fieldNames.push_back(name);
	return shared_from_this();
}

FieldConstPtr FieldBuilder::createFieldInternal(Type type)
{
    // minor optimization
    if (fieldNames.size() == 0 && type == union_)
        return fieldCreate->createVariantUnion();

    if (type == structure)
    {
        return (idSet) ?
            fieldCreate->createStructure(id, fieldNames, fields) :
            fieldCreate->createStructure(fieldNames, fields);
    }
    else if (type == union_)
    {
        return (idSet) ?
            fieldCreate->createUnion(id, fieldNames, fields) :
            fieldCreate->createUnion(fieldNames, fields);
    }
    else
        throw std::invalid_argument("unsupported type: " + type);    
}


StructureConstPtr FieldBuilder::createStructure()
{
    if (parentBuilder.get())
        throw std::runtime_error("createStructure() called in nested FieldBuilder");
	
    StructureConstPtr field(static_pointer_cast<const Structure>(createFieldInternal(structure)));
    reset();
    return field;
}

UnionConstPtr FieldBuilder::createUnion()
{
    if (parentBuilder.get())
        throw std::runtime_error("createUnion() called in nested FieldBuilder");
	
    UnionConstPtr field(static_pointer_cast<const Union>(createFieldInternal(union_)));
    reset();
    return field;
}

FieldBuilderPtr FieldBuilder::addNestedStructure(string const & name)
{
    return FieldBuilderPtr(new FieldBuilder(shared_from_this(), name, structure, false));
}


FieldBuilderPtr FieldBuilder::addNestedUnion(string const & name)
{
    return FieldBuilderPtr(new FieldBuilder(shared_from_this(), name, union_, false));
}


FieldBuilderPtr FieldBuilder::addNestedStructureArray(string const & name)
{
    return FieldBuilderPtr(new FieldBuilder(shared_from_this(), name, structure, true));
}

FieldBuilderPtr FieldBuilder::addNestedUnionArray(string const & name)
{
    return FieldBuilderPtr(new FieldBuilder(shared_from_this(), name, union_, true));
}

FieldBuilderPtr FieldBuilder::endNested()
{
    if (!parentBuilder.get())
        throw std::runtime_error("this method can only be called to create nested fields");
        
    FieldConstPtr nestedField = createFieldInternal(nestedClassToBuild);
    if (nestedArray)
        parentBuilder->addArray(nestedName, nestedField);
    else
        parentBuilder->add(nestedName, nestedField);
        
    return parentBuilder;
}


FieldBuilderPtr FieldCreate::createFieldBuilder() const
{
    return FieldBuilderPtr(new FieldBuilder());
}

ScalarConstPtr FieldCreate::createScalar(ScalarType scalarType) const
{
    if(scalarType<0 || scalarType>MAX_SCALAR_TYPE)
        throw std::invalid_argument("Can't construct Scalar from invalid ScalarType");

    return scalars[scalarType];
}
 
ScalarArrayConstPtr FieldCreate::createScalarArray(ScalarType elementType) const
{
    if(elementType<0 || elementType>MAX_SCALAR_TYPE)
        throw std::invalid_argument("Can't construct ScalarArray from invalid ScalarType");
        
    return scalarArrays[elementType];
}

StructureConstPtr FieldCreate::createStructure () const
{
      StringArray fieldNames;
      FieldConstPtrArray fields;
      return createStructure(fieldNames,fields);
}

StructureConstPtr FieldCreate::createStructure (
    StringArray const & fieldNames,FieldConstPtrArray const & fields) const
{
      StructureConstPtr structure(
         new Structure(fieldNames,fields), Field::Deleter());
      return structure;
}

StructureConstPtr FieldCreate::createStructure (
    string const & id,
    StringArray const & fieldNames,
    FieldConstPtrArray const & fields) const
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

UnionConstPtr FieldCreate::createUnion (
    StringArray const & fieldNames,FieldConstPtrArray const & fields) const
{
      UnionConstPtr punion(
         new Union(fieldNames,fields), Field::Deleter());
      return punion;
}

UnionConstPtr FieldCreate::createUnion (
    string const & id,
    StringArray const & fieldNames,
    FieldConstPtrArray const & fields) const
{
      UnionConstPtr punion(
         new Union(fieldNames,fields,id), Field::Deleter());
      return punion;
}

UnionConstPtr FieldCreate::createVariantUnion () const
{
    return variantUnion;
}

UnionArrayConstPtr FieldCreate::createUnionArray(
    UnionConstPtr const & punion) const
{
     UnionArrayConstPtr unionArray(
        new UnionArray(punion), Field::Deleter());
     return unionArray;
}

UnionArrayConstPtr FieldCreate::createVariantUnionArray () const
{
    return variantUnionArray;
}

StructureConstPtr FieldCreate::appendField(
    StructureConstPtr const & structure,
    string const & fieldName,
    FieldConstPtr const & field) const
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
    return createStructure(structure->getID(),newNames,newFields);
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
    return createStructure(structure->getID(),newNames,newFields);
}


static int decodeScalar(int8 code)
{
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

    int typeCode = code & 0xEF;
    bool notArray = ((code & 0x10) == 0);
    if (notArray)
    {
        if (typeCode < 0x80)
        {
            // Type type = Type.scalar;
            int scalarType = decodeScalar(code);
            if (scalarType == -1)
                throw std::invalid_argument("invalid scalar type encoding");
            return scalars[scalarType];
        }
        else if (typeCode == 0x80)
        {
            // Type type = Type.structure;
            return deserializeStructureField(this, buffer, control);
        }
        else if (typeCode == 0x81)
        {
            // Type type = Type.union;
            return deserializeUnionField(this, buffer, control);
        }
        else if (typeCode == 0x82)
        {
            // Type type = Type.union; variant union (aka any type)
            return variantUnion;
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
            return scalarArrays[scalarType];
        }
        else if (typeCode == 0x80)
        {
            // Type type = Type.structureArray;
            StructureConstPtr elementStructure = std::tr1::static_pointer_cast<const Structure>(control->cachedDeserialize(buffer));
            return FieldConstPtr(new StructureArray(elementStructure), Field::Deleter());
        }
        else if (typeCode == 0x81)
        {
            // Type type = Type.unionArray;
            UnionConstPtr elementUnion = std::tr1::static_pointer_cast<const Union>(control->cachedDeserialize(buffer));
            return FieldConstPtr(new UnionArray(elementUnion), Field::Deleter());
        }
        else if (typeCode == 0x82)
        {
            // Type type = Type.unionArray; variant union (aka any type)
            return variantUnionArray;
        }
        else
            throw std::invalid_argument("invalid type encoding");
    }
}

// TODO replace with non-locking singleton pattern
FieldCreatePtr FieldCreate::getFieldCreate()
{
	LOCAL_STATIC_LOCK;
	static FieldCreatePtr fieldCreate;
	static Mutex mutex;

	Lock xx(mutex);
    if(fieldCreate.get()==0) fieldCreate = FieldCreatePtr(new FieldCreate());
    return fieldCreate;
}

FieldCreate::FieldCreate()
{
    for (int i = 0; i <= MAX_SCALAR_TYPE; i++)
    {
        scalars.push_back(ScalarConstPtr(new Scalar(static_cast<ScalarType>(i)), Field::Deleter()));
        scalarArrays.push_back(ScalarArrayConstPtr(new ScalarArray(static_cast<ScalarType>(i)), Field::Deleter()));
    }
    variantUnion = UnionConstPtr(new Union(), Field::Deleter());
    variantUnionArray = UnionArrayConstPtr(new UnionArray(variantUnion), Field::Deleter());
}

FieldCreatePtr getFieldCreate() {
    return FieldCreate::getFieldCreate();
}

}}
