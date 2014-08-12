/* pvIntrospect.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk and Michael Davidsaver
 */
#ifndef PVINTROSPECT_H
#define PVINTROSPECT_H

#include <string>
#include <stdexcept>
#include <iostream>

#include <pv/noDefaultMethods.h>
#include <pv/pvType.h>
#include <pv/byteBuffer.h>
#include <pv/serialize.h>

#include <shareLib.h>

namespace epics { namespace pvData { 

namespace format {

struct indent_level
{
    long level;

    indent_level(long l) : level(l) {}
};

inline long& indent_value(std::ios_base& ios)
{
  static int indent_index = std::ios_base::xalloc();
  return ios.iword(indent_index);
}

epicsShareExtern std::ostream& operator<<(std::ostream& os, indent_level const& indent);

struct indent_scope
{
    long saved_level;
    std::ios_base& stream;

    indent_scope(std::ios_base& ios) :
        stream(ios)
    {
        long& l = indent_value(ios);
        saved_level = l;
        l = saved_level + 1;
    }

    ~indent_scope()
    {
        indent_value(stream) = saved_level;
    }
};

struct indent
{
};

epicsShareExtern std::ostream& operator<<(std::ostream& os, indent const&);

struct array_at
{
    std::size_t index;

    array_at(std::size_t ix) : index(ix) {}
};

struct array_at_internal
{
    std::size_t index;
    std::ostream& stream;

    array_at_internal(std::size_t ix, std::ostream& str) : index(ix), stream(str) {}
};

epicsShareExtern array_at_internal operator<<(std::ostream& str, array_at const& manip);

};

class Field;
class Scalar;
class Array;
class ScalarArray;
class Structure;
class StructureArray;
class Union;
class UnionArray;

/**
 * typedef for a shared pointer to an immutable Field.
 */
typedef std::tr1::shared_ptr<const Field> FieldConstPtr;
/**
 * typedef for an array of shared pointer to an immutable Field.
 */
typedef std::vector<FieldConstPtr> FieldConstPtrArray;
/**
 * typedef for a shared pointer to an immutable Scalar.
 */
typedef std::tr1::shared_ptr<const Scalar> ScalarConstPtr;
/**
 * typedef for a shared pointer to an immutable Array.
 */
typedef std::tr1::shared_ptr<const Array> ArrayConstPtr;
/**
 * typedef for a shared pointer to an immutable ScalarArray.
 */
typedef std::tr1::shared_ptr<const ScalarArray> ScalarArrayConstPtr;
/**
 * typedef for a shared pointer to an immutable Structure.
 */
typedef std::tr1::shared_ptr<const Structure> StructureConstPtr;
/**
 * typedef for a shared pointer to an immutable StructureArray.
 */
typedef std::tr1::shared_ptr<const StructureArray> StructureArrayConstPtr;
/**
 * typedef for a shared pointer to an immutable Union.
 */
typedef std::tr1::shared_ptr<const Union> UnionConstPtr;
/**
 * typedef for a shared pointer to an immutable UnionArray.
 */
typedef std::tr1::shared_ptr<const UnionArray> UnionArrayConstPtr;

/**
 * Definition of support field types.
 */
enum Type {
    /**
     * The type is scalar. It has a scalarType
     */
    scalar,
    /**
     * The type is scalarArray. Each element is a scalar of the same scalarType.
     */
    scalarArray,
    /**
     * The type is structure.
     */
    structure,
    /**
     * The type is structureArray. Each element is a structure.
     */
    structureArray,
    /**
     * The type is an union.
     */
    union_,
    /**
     * The type is an array of unions.
     */
    unionArray
};

/**
 * Convenience functions for Type.
 */
namespace TypeFunc {
    /**
     * Get a name for the type.
     * @param  type The type.
     * @return The name for the type.
     */
    epicsShareExtern const char* name(Type type);
};

epicsShareExtern std::ostream& operator<<(std::ostream& o, const Type& type);


/**
 * Definition of support scalar types.
 */
enum ScalarType {
    /**
     * The type is boolean, i. e. value can be {@code false} or {@code true}
     */
    pvBoolean,
    /**
     * The type is byte, i. e. a 8 bit signed integer.
     */
    pvByte,
    /**
     * The type is short, i. e. a 16 bit signed integer.
     */
    pvShort,
    /**
     * The type is int, i. e. a 32 bit signed integer.
     */
    pvInt,
    /**
     * The type is long, i. e. a 64 bit signed integer.
     */
    pvLong,
    /**
     * The type is unsigned byte, i. e. a 8 bit unsigned integer.
     */
    pvUByte,
    /**
     * The type is unsigned short, i. e. a 16 bit unsigned integer.
     */
    pvUShort,
    /**
     * The type is unsigned int, i. e. a 32 bit unsigned integer.
     */
    pvUInt,
    /**
     * The type is unsigned long, i. e. a 64 bit unsigned integer.
     */
    pvULong,
    /**
     * The type is float, i. e. 32 bit IEEE floating point,
     */
    pvFloat,
    /**
     * The type is float, i. e. 64 bit IEEE floating point,
     */
    pvDouble,
    /**
     * The type is string, i. e. a UTF8 character string.
     */
    pvString
};

#define MAX_SCALAR_TYPE pvString

/**
 * Convenience functions for ScalarType.
 */
namespace ScalarTypeFunc {
    /**
     * Is the type an integer, i. e. is it one of byte,...ulong
     * @param  scalarType The type.
     * @return (false,true) if the scalarType is an integer.
     */
    epicsShareExtern bool isInteger(ScalarType scalarType);
    /**
     * Is the type an unsigned integer, i. e. is it one of ubyte,...ulong
     * @param  scalarType The type.
     * @return (false,true) if the scalarType is an integer.
     */
    epicsShareExtern bool isUInteger(ScalarType scalarType);
    /**
     * Is the type numeric, i. e. is it one of byte,...,double
     * @param  scalarType The type.
     * @return (false,true) if the scalarType is a numeric
     */
    epicsShareExtern bool isNumeric(ScalarType scalarType);
    /**
     * Is the type primitive, i. e. not string
     * @param  scalarType The type.
     * @return (false,true) if the scalarType is primitive.
     */
    epicsShareExtern bool isPrimitive(ScalarType scalarType);
    /**
     * Get the scalarType for value.
     * @param  value The name of the scalar type.
     * @return The scalarType.
     * An exception is thrown if the name is not the name of a scalar type.
     */
    epicsShareExtern ScalarType getScalarType(std::string const &value);
    /**
     * Get a name for the scalarType.
     * @param  scalarType The type.
     * @return The name for the scalarType.
     */
    epicsShareExtern const char* name(ScalarType scalarType);

    //! gives sizeof(T) where T depends on the scalar type id.
    epicsShareExtern size_t elementSize(ScalarType id);
};

epicsShareExtern std::ostream& operator<<(std::ostream& o, const ScalarType& scalarType);


/**
 * This class implements introspection object for field.
 */
class epicsShareClass Field : 
    virtual public Serializable,
    public std::tr1::enable_shared_from_this<Field> {
public:
   POINTER_DEFINITIONS(Field);
    /**
     * Destructor.
     */
   virtual ~Field();
    /**
     * Get the field type.
     * @return The type.
     */
   Type getType() const{return m_fieldType;}
   /**
    * Get the identification string.
    * @return The identification string, can be empty.
    */
   virtual std::string getID() const = 0;
   
    /**
     * Puts the string representation to the stream.
     * @param o output stream.
     * @return The output stream.
     */
    virtual std::ostream& dump(std::ostream& o) const = 0;

protected:
    /**
     * Constructor
     * @param  fieldName The field type.
     */
   Field(Type type);
private:
   Type m_fieldType;

   friend class StructureArray;
   friend class Structure;
   friend class PVFieldPvt;
   friend class StandardField;
   friend class BasePVStructureArray;
   friend class FieldCreate;

   struct Deleter{void operator()(Field *p){delete p;}};
};

epicsShareExtern std::ostream& operator<<(std::ostream& o, const Field& field);


/**
 * This class implements introspection object for Scalar.
 */
class epicsShareClass Scalar : public Field{
public:
    POINTER_DEFINITIONS(Scalar);
    /**
     * Destructor.
     */
    virtual ~Scalar();
    typedef Scalar& reference;
    typedef const Scalar& const_reference;
    /**
     * Get the scalarType
     * @return the scalarType
     */
    ScalarType getScalarType() const {return scalarType;}
    
    virtual std::string getID() const;

    virtual std::ostream& dump(std::ostream& o) const;

    virtual void serialize(ByteBuffer *buffer, SerializableControl *control) const;
    virtual void deserialize(ByteBuffer *buffer, DeserializableControl *control);
    
protected:
    Scalar(ScalarType scalarType);
private:
    static int8 getTypeCodeLUT(ScalarType scalarType);
    ScalarType scalarType;
    friend class FieldCreate;
    friend class ScalarArray;
    friend class BoundedScalarArray;
    friend class FixedScalarArray;
};

/**
 * This class implements introspection object for Array.
 */
class epicsShareClass Array : public Field{
public:
    POINTER_DEFINITIONS(Array);
    /**
     * Destructor.
     */
    virtual ~Array();
    typedef Array& reference;
    typedef const Array& const_reference;

    enum ArraySizeType { variable, fixed, bounded };

    /**
     * Get array size type (i.e. variable/fixed/bounded size array).
     * @return array size type enum.
     */
    virtual ArraySizeType getArraySizeType() const = 0;

    /**
     * Get maximum capacity of the array.
     * @return maximum capacity of the array, 0 indicates variable size array.
     */
    virtual std::size_t getMaximumCapacity() const = 0;

protected:
    /**
     * Constructor
     * @param  fieldName The field type.
     */
   Array(Type type);

};







/**
 * This class implements introspection object for scalar array.
 */
class epicsShareClass ScalarArray : public Array{
public:
    POINTER_DEFINITIONS(ScalarArray);
    typedef ScalarArray& reference;
    typedef const ScalarArray& const_reference;

    /**
     * Constructor
     * @param scalarType The scalarType for the field.
     */
    ScalarArray(ScalarType scalarType);
    /**
     * Get the scalarType for the elements.
     * @return the scalarType
     */
    ScalarType getElementType() const {return elementType;}
    
    virtual ArraySizeType getArraySizeType() const {return Array::variable;}

    virtual std::size_t getMaximumCapacity() const {return 0;}

    virtual std::string getID() const;

    virtual std::ostream& dump(std::ostream& o) const;

    virtual void serialize(ByteBuffer *buffer, SerializableControl *control) const;
    virtual void deserialize(ByteBuffer *buffer, DeserializableControl *control);
    
protected:
    /**
     * Destructor.
     */
    virtual ~ScalarArray();
private:
    const std::string getIDScalarArrayLUT() const;
    ScalarType elementType;
    friend class FieldCreate;
};



/**
 * This class implements introspection object for bounded scalar array.
 */
class epicsShareClass BoundedScalarArray : public ScalarArray{
public:
    POINTER_DEFINITIONS(BoundedScalarArray);
    typedef BoundedScalarArray& reference;
    typedef const BoundedScalarArray& const_reference;

    /**
     * Constructor
     * @param scalarType The scalarType for the field.
     * @param size maximum (bound) capacity.
     */
    BoundedScalarArray(ScalarType scalarType, std::size_t size);

    virtual ArraySizeType getArraySizeType() const {return Array::bounded;}

    virtual std::size_t getMaximumCapacity() const {return size;}

    virtual std::string getID() const;

    virtual void serialize(ByteBuffer *buffer, SerializableControl *control) const;

protected:
    /**
     * Destructor.
     */
    virtual ~BoundedScalarArray();
private:
    std::size_t size;
    friend class FieldCreate;
};

/**
 * This class implements introspection object for bounded scalar array.
 */
class epicsShareClass FixedScalarArray : public ScalarArray{
public:
    POINTER_DEFINITIONS(FixedScalarArray);
    typedef FixedScalarArray& reference;
    typedef const FixedScalarArray& const_reference;

    /**
     * Constructor
     * @param scalarType The scalarType for the field.
     * @param size maximum (bound) capacity.
     */
    FixedScalarArray(ScalarType scalarType, std::size_t size);

    virtual ArraySizeType getArraySizeType() const {return Array::fixed;}

    virtual std::size_t getMaximumCapacity() const {return size;}

    virtual std::string getID() const;

    virtual void serialize(ByteBuffer *buffer, SerializableControl *control) const;

protected:
    /**
     * Destructor.
     */
    virtual ~FixedScalarArray();
private:
    std::size_t size;
    friend class FieldCreate;
};





/**
 * This class implements introspection object for a structureArray
 */
class epicsShareClass StructureArray : public Array{
public:
    POINTER_DEFINITIONS(StructureArray);
    typedef StructureArray& reference;
    typedef const StructureArray& const_reference;

    /**
     * Get the introspection interface for the array elements.
     * @return The introspection interface.
     */
    StructureConstPtr getStructure() const {return pstructure;}

    virtual ArraySizeType getArraySizeType() const {return Array::variable;}

    virtual std::size_t getMaximumCapacity() const {return 0;}

    virtual std::string getID() const;

    virtual std::ostream& dump(std::ostream& o) const;

    virtual void serialize(ByteBuffer *buffer, SerializableControl *control) const;
    virtual void deserialize(ByteBuffer *buffer, DeserializableControl *control);

protected:
    /**
     * Constructor.
     * @param structure The introspection interface for the elements.
     */
    StructureArray(StructureConstPtr const & structure);
    /**
     * Destructor.
     */
    virtual ~StructureArray();
private:
    StructureConstPtr pstructure;
    friend class FieldCreate;
};

/**
 * This class implements introspection object for a unionArray
 */
class epicsShareClass UnionArray : public Array{
public:
    POINTER_DEFINITIONS(UnionArray);
    typedef UnionArray& reference;
    typedef const UnionArray& const_reference;

    /**
     * Get the introspection interface for the array elements.
     * @return The introspection interface.
     */
    UnionConstPtr getUnion() const {return punion;}

    virtual ArraySizeType getArraySizeType() const {return Array::variable;}

    virtual std::size_t getMaximumCapacity() const {return 0;}

    virtual std::string getID() const;

    virtual std::ostream& dump(std::ostream& o) const;

    virtual void serialize(ByteBuffer *buffer, SerializableControl *control) const;
    virtual void deserialize(ByteBuffer *buffer, DeserializableControl *control);

protected:
    /**
     * Constructor.
     * @param union The introspection interface for the elements.
     */
    UnionArray(UnionConstPtr const & _punion);
    /**
     * Destructor.
     */
    virtual ~UnionArray();
private:
    UnionConstPtr punion;
    friend class FieldCreate;
};

/**
 * This class implements introspection object for a structure.
 */
class epicsShareClass Structure : public Field {
public:
    POINTER_DEFINITIONS(Structure);

    /**
     * Default structure ID.
     */
    static std::string DEFAULT_ID;

    /**
     * Destructor.
     */
    virtual ~Structure();
    typedef Structure& reference;
    typedef const Structure& const_reference;

    /**
     * Get the number of immediate subfields in the structure.
     * @return The number of fields.
     */
    std::size_t getNumberFields() const {return fieldNames.size();}
    /**
     * Get the field for the specified fieldName.
     * @param fieldName The name of the field to get;
     * @return The introspection interface.
     * This will hold a null pointer if the field is not in the structure.
     */
    FieldConstPtr getField(std::string const &fieldName) const;
    /**
     * Get the field for the specified fieldName.
     * @param fieldName The index of the field to get;
     * @return The introspection interface.
     * This will hold a null pointer if the field is not in the structure.
     */
    FieldConstPtr getField(std::size_t index) const {return fields[index];}
    /**
     * Get the field index for the specified fieldName.
     * @return The introspection interface.
     * This will be -1 if the field is not in the structure.
     */
    std::size_t getFieldIndex(std::string const &fieldName) const;
    /**
     * Get the fields in the structure.
     * @return The array of fields.
     */
    FieldConstPtrArray const & getFields() const {return fields;}
    /**
     * Get the names of the fields in the structure.
     * @return The array of fieldNames.
     */
    StringArray const & getFieldNames() const {return fieldNames;}
    /**
     * Get the name of the field with the specified index;
     * @param fieldIndex The index of the desired field.
     * @return The fieldName.
     */
    std::string getFieldName(std::size_t fieldIndex) const {return fieldNames[fieldIndex];}

    virtual std::string getID() const;

    virtual std::ostream& dump(std::ostream& o) const;

    virtual void serialize(ByteBuffer *buffer, SerializableControl *control) const;
    virtual void deserialize(ByteBuffer *buffer, DeserializableControl *control);
    
protected:
    Structure(StringArray const & fieldNames, FieldConstPtrArray const & fields, std::string const & id = DEFAULT_ID);
private:
    StringArray fieldNames;
    FieldConstPtrArray fields;
    std::string id;

    virtual void dumpFields(std::ostream& o) const;
    
    friend class FieldCreate;
    friend class Union;
};

/**
 * This class implements introspection object for a union.
 */
class epicsShareClass Union : public Field {
public:
    POINTER_DEFINITIONS(Union);

    /**
     * Default union ID.
     */
    static std::string DEFAULT_ID;

    /**
     * Default variant union ID.
     */
    static std::string ANY_ID;

    /**
     * Destructor.
     */
    virtual ~Union();
    typedef Union& reference;
    typedef const Union& const_reference;

    /**
     * Get the number of immediate subfields in the union.
     * @return The number of fields.
     */
    std::size_t getNumberFields() const {return fieldNames.size();}
    /**
     * Get the field for the specified fieldName.
     * @param fieldName The name of the field to get;
     * @return The introspection interface.
     * This will hold a null pointer if the field is not in the union.
     */
    FieldConstPtr getField(std::string const &fieldName) const;
    /**
     * Get the field for the specified fieldName.
     * @param fieldName The index of the field to get;
     * @return The introspection interface.
     * This will hold a null pointer if the field is not in the union.
     */
    FieldConstPtr getField(std::size_t index) const {return fields[index];}
    /**
     * Get the field index for the specified fieldName.
     * @return The introspection interface.
     * This will be -1 if the field is not in the union.
     */
    std::size_t getFieldIndex(std::string const &fieldName) const;
    /**
     * Get the fields in the union.
     * @return The array of fields.
     */
    FieldConstPtrArray const & getFields() const {return fields;}
    /**
     * Get the names of the fields in the union.
     * @return The array of fieldNames.
     */
    StringArray const & getFieldNames() const {return fieldNames;}
    /**
     * Get the name of the field with the specified index;
     * @param fieldIndex The index of the desired field.
     * @return The fieldName.
     */
    std::string getFieldName(std::size_t fieldIndex) const {return fieldNames[fieldIndex];}
    /**
     * Check if this union is variant union (aka any type).
     * @return <code>true</code> if this union is variant union, otherwise <code>false</code>.
     */
    bool isVariant() const {return (fieldNames.size() == 0);}

    virtual std::string getID() const;

    virtual std::ostream& dump(std::ostream& o) const;

    virtual void serialize(ByteBuffer *buffer, SerializableControl *control) const;
    virtual void deserialize(ByteBuffer *buffer, DeserializableControl *control);
    
protected:
   Union();
   Union(StringArray const & fieldNames, FieldConstPtrArray const & fields, std::string const & id = DEFAULT_ID);
private:
   StringArray fieldNames;
   FieldConstPtrArray fields;
   std::string id;
   
   virtual void dumpFields(std::ostream& o) const;

   friend class FieldCreate;
   friend class Structure;
};

class FieldCreate;
typedef std::tr1::shared_ptr<FieldCreate> FieldCreatePtr;

class FieldBuilder;
typedef std::tr1::shared_ptr<FieldBuilder> FieldBuilderPtr;

/**
 * Interface for in-line creating of introspection interfaces.
 * One instance can be used to create multiple {@code Field} instances.
 * An instance of this object must not be used concurrently (an object has a state).
 * @author mse
 */
class epicsShareClass FieldBuilder :
    public std::tr1::enable_shared_from_this<FieldBuilder>
{
public:
	/**
	 * Set ID of an object to be created.
	 * @param id id to be set.
     * @return this instance of a {@code FieldBuilder}.
	 */
	FieldBuilderPtr setId(std::string const & id);

    /**
     * Add a {@code Scalar}.
     * @param name name of the array.
     * @param scalarType type of a scalar to add.
     * @return this instance of a {@code FieldBuilder}.
     */
    FieldBuilderPtr add(std::string const & name, ScalarType scalarType);

    /**
     * Add a {@code Field} (e.g. {@code Structure}, {@code Union}).
     * @param name name of the array.
     * @param field a field to add.
     * @return this instance of a {@code FieldBuilder}.
     */
    FieldBuilderPtr add(std::string const & name, FieldConstPtr const & field);

    /**
     * Add variable size array of {@code Scalar} elements.
     * @param name name of the array.
     * @param scalarType type of a scalar element.
     * @return this instance of a {@code FieldBuilder}.
     */
    FieldBuilderPtr addArray(std::string const & name, ScalarType scalarType);
    
    /**
     * Add fixed-size array of {@code Scalar} elements.
     * @param name name of the array.
     * @param scalarType type of a scalar element.
     * @param size Array fixed size.
     * @return this instance of a {@code FieldBuilder}.
     */
    FieldBuilderPtr addFixedArray(std::string const & name, ScalarType scalarType, std::size_t size);

    /**
     * Add bounded-size array of {@code Scalar} elements.
     * @param name name of the array.
     * @param scalarType type of a scalar element.
     * @param bound Array maximum capacity (size).
     * @return this instance of a {@code FieldBuilder}.
     */
    FieldBuilderPtr addBoundedArray(std::string const & name, ScalarType scalarType, std::size_t bound);

    /**
     * Add array of {@code Field} elements.
     * @param name name of the array.
     * @param field a type of an array element.
     * @return this instance of a {@code FieldBuilder}.
     */
    FieldBuilderPtr addArray(std::string const & name, FieldConstPtr const & element);

    /**
     * Create a {@code Structure}.
     * This resets this instance state and allows new {@code Field} instance to be created.
     * @return a new instance of a {@code Structure}.
     */
    StructureConstPtr createStructure();
    
    /**
     * Create an {@code Union}.
     * This resets this instance state and allows new {@code Field} instance to be created.
     * @return a new instance of an {@code Union}.
     */
    UnionConstPtr createUnion();

    /**
     * Add new nested {@code Structure}.
     * {@code endNested()} method must be called
     * to complete creation of the nested {@code Structure}.
     * @param name nested structure name.
     * @return a new instance of a {@code FieldBuilder} is returned.
     * @see #endNested()
     */
    FieldBuilderPtr addNestedStructure(std::string const & name); 
    
    /**
     * Add new nested {@code Union}.
     * {@code endNested()} method must be called
     * to complete creation of the nested {@code Union}.
     * @param name nested union name.
     * @return a new instance of a {@code FieldBuilder} is returned.
     * @see #endNested()
     */
    FieldBuilderPtr addNestedUnion(std::string const & name);
    
    /**
     * Add new nested {@code Structure[]}.
     * {@code endNested()} method must be called
     * to complete creation of the nested {@code Structure}.
     * @param name nested structure name.
     * @return a new instance of a {@code FieldBuilder} is returned.
     * @see #endNested()
     */
    FieldBuilderPtr addNestedStructureArray(std::string const & name); 
    
    /**
     * Add new nested {@code Union[]}.
     * {@code endNested()} method must be called
     * to complete creation of the nested {@code Union}.
     * @param name nested union name.
     * @return a new instance of a {@code FieldBuilder} is returned.
     * @see #endNested()
     */
    FieldBuilderPtr addNestedUnionArray(std::string const & name);

    /**
     * Complete the creation of a nested object.
     * @see #addNestedStructure(std::string)
     * @see #addNestedUnion(std::string)
     * @return a previous (parent) {@code FieldBuilder}.
     */
    FieldBuilderPtr endNested();

private:
    FieldBuilder();
    FieldBuilder(FieldBuilderPtr const & parentBuilder,
			std::string const & nestedName,
			Type nestedClassToBuild, bool nestedArray);
			
	void reset();
	FieldConstPtr createFieldInternal(Type type);
			
    friend class FieldCreate;
    
    FieldCreatePtr fieldCreate;

	std::string id;
	bool idSet;
	
	// NOTE: this preserves order, however it does not handle duplicates
    StringArray fieldNames;
    FieldConstPtrArray fields;
    
	FieldBuilderPtr parentBuilder;
	Type nestedClassToBuild;
	std::string nestedName;
	bool nestedArray;
   
};

/**
 * This is a singleton class for creating introspection interfaces.
 */
class epicsShareClass FieldCreate {
public:
    static FieldCreatePtr getFieldCreate();
	/**
	 * Create a new instance of in-line {@code Field} builder.
	 * @return a new instance of a {@code FieldBuilder}.
	 */
	FieldBuilderPtr createFieldBuilder() const;
    /**
     * Create a {@code ScalarField}.
     * @param scalarType The scalar type.
     * @return a {@code Scalar} interface for the newly created object.
     * @throws An {@code IllegalArgumentException} if an illegal type is specified.
     */
    ScalarConstPtr createScalar(ScalarType scalarType) const;
    /**
     * Create an {@code Array} field, variable size array.
     * @param elementType The {@code scalarType} for array elements
     * @return An {@code Array} Interface for the newly created object.
     */
    ScalarArrayConstPtr createScalarArray(ScalarType elementType) const;
    /*
     * Create an {@code Array} field, fixed size array.
     * @param elementType The {@code scalarType} for array elements
     * @param size Fixed array size.
     * @return An {@code Array} Interface for the newly created object.
     */
    ScalarArrayConstPtr createFixedScalarArray(ScalarType elementType, std::size_t size) const;
    /**
     * Create an {@code Array} field, bounded size array.
     * @param elementType The {@code scalarType} for array elements
     * @param size Array maximum capacity (bound).
     * @return An {@code Array} Interface for the newly created object.
     */
     ScalarArrayConstPtr createBoundedScalarArray(ScalarType elementType, std::size_t bound) const;
     /**
      * Create an {@code Array} field that is has element type <i>Structure</i>
      * @param fieldName The field name
      * @param elementStructure The {@code Structure} for each array element.
      * @return An {@code Array} Interface for the newly created object.
      */
    StructureArrayConstPtr createStructureArray(StructureConstPtr const & structure) const;
    /**
     * Create a {@code Structure} field.
     * @return a {@code Structure} interface for the newly created object.
     */
    StructureConstPtr createStructure () const;
    /**
     * Create a {@code Structure} field.
     * @param fieldNames The array of {@code fieldNames} for the structure.
     * @param fields The array of {@code fields} for the structure.
     * @return a {@code Structure} interface for the newly created object.
     */
    StructureConstPtr createStructure (
        StringArray const & fieldNames,
        FieldConstPtrArray const & fields) const;
    /**
     * Create a {@code Structure} field with identification string.
     * @param id The identification string for the structure.
     * @param fieldNames The array of {@code fieldNames} for the structure.
     * @param fields The array of {@code fields} for the structure.
     * @return a {@code Structure} interface for the newly created object.
     */
    StructureConstPtr createStructure (
    	std::string const & id,
        StringArray const & fieldNames,
        FieldConstPtrArray const & fields) const;
     /**
      * Create an {@code Array} field that is has element type <i>Union</i>
      * @param fieldName The field name
      * @param elementUnion The {@code Union} for each array element.
      * @return An {@code Array} Interface for the newly created object.
      */
    UnionArrayConstPtr createUnionArray(UnionConstPtr const & punion) const;
    /**
     * Create a variant {@code UnionArray} (aka any type) field.
     * @return a {@code UnionArray} interface for the newly created object.
     */
    UnionArrayConstPtr createVariantUnionArray() const;
    /**
     * Create a variant {@code Union} (aka any type) field.
     * @return a {@code Union} interface for the newly created object.
     */
    UnionConstPtr createVariantUnion() const;
    /**
     * Create a {@code Union} field.
     * @param fieldNames The array of {@code fieldNames} for the union.
     * @param fields The array of {@code fields} for the union.
     * @return a {@code Union} interface for the newly created object.
     */
    UnionConstPtr createUnion (
        StringArray const & fieldNames,
        FieldConstPtrArray const & fields) const;
    /**
     * Create a {@code Union} field with identification string.
     * @param id The identification string for the union.
     * @param fieldNames The array of {@code fieldNames} for the union.
     * @param fields The array of {@code fields} for the union.
     * @return a {@code Union} interface for the newly created object.
     */
    UnionConstPtr createUnion (
    	std::string const & id,
        StringArray const & fieldNames,
        FieldConstPtrArray const & fields) const;
    /**
     * Append a field to a structure.
     * @param structure The structure to which the field is appended.
     * @param fieldName The name of the field.
     * @param field The field.
     * @return a {@code Structure} interface for the newly created object.
     */
    StructureConstPtr appendField(
        StructureConstPtr const & structure,
        std::string const & fieldName, FieldConstPtr const & field) const;
    /**
     * Append fields to a structure.
     * @param structure The structure to which the fields appended.
     * @param fieldName The names of the fields.
     * @param field The fields.
     * @return a {@code Structure} interface for the newly created object.
     */
    StructureConstPtr appendFields(
        StructureConstPtr const & structure,
        StringArray const & fieldNames,
        FieldConstPtrArray const & fields) const;
    /**
     * Deserialize {@code Field} instance from given byte buffer.
     * @param buffer Buffer containing serialized {@code Field} instance. 
     * @param control Deserialization control instance.
     * @return a deserialized {@code Field} instance.
     */
    FieldConstPtr deserialize(ByteBuffer* buffer, DeserializableControl* control) const;
        
private:
    FieldCreate();
    
    std::vector<ScalarConstPtr> scalars;
    std::vector<ScalarArrayConstPtr> scalarArrays;
    UnionConstPtr variantUnion;
    UnionArrayConstPtr variantUnionArray;
};

/**
 * Get the single class that implemnents FieldCreate,
 * @param The fieldCreate factory.
 */
epicsShareExtern FieldCreatePtr getFieldCreate();

/** Define a compile time mapping from
 * type to enum value.
 @code
  ScalarType code = (ScalarType)ScalarTypeID<int8>::value;
  assert(code==pvByte);
 @endcode
 *
 * For unspecified types this evaluates to an invalid ScalarType
 * value (eg -1).
 */
template<typename T>
struct ScalarTypeID { enum {value=-1}; };

/**
 * Static mapping from ScalarType enum to value type.
 @code
   typename ScalarTypeTraits<pvByte>::type value = 4;
 @endcode
 */
template<ScalarType ID>
struct ScalarTypeTraits {};

#define OP(ENUM, TYPE) \
template<> struct ScalarTypeTraits<ENUM> {typedef TYPE type;}; \
template<> struct ScalarTypeID<TYPE> { enum {value=ENUM}; }; \
template<> struct ScalarTypeID<const TYPE> { enum {value=ENUM}; };

OP(pvBoolean, boolean)
OP(pvByte, int8)
OP(pvShort, int16)
OP(pvInt, int32)
OP(pvLong, int64)
OP(pvUByte, uint8)
OP(pvUShort, uint16)
OP(pvUInt, uint32)
OP(pvULong, uint64)
OP(pvFloat, float)
OP(pvDouble, double)
OP(pvString, std::string)
#undef OP

struct ScalarHashFunction {
    size_t operator() (const Scalar& scalar) const { return scalar.getScalarType(); }
};

struct ScalarArrayHashFunction {
    size_t operator() (const ScalarArray& scalarArray) const { return 0x10 | scalarArray.getElementType(); }
};

struct StructureHashFunction {
    size_t operator() (const Structure& /*structure*/) const { return 0; }
    // TODO hash
//        final int PRIME = 31;
//        return PRIME * Arrays.hashCode(fieldNames) + Arrays.hashCode(fields);
};

struct StructureArrayHashFunction {
    size_t operator() (const StructureArray& structureArray) const { StructureHashFunction shf; return (0x10 | shf(*(structureArray.getStructure()))); }
};

}}
#endif  /* PVINTROSPECT_H */
