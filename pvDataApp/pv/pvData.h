/* pvData.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#ifndef PVDATA_H
#define PVDATA_H
#include <string>
#include <map>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <iomanip>
#include <pv/pvIntrospect.h>
#include <pv/requester.h>
#include <pv/typeCast.h>
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

std::ostream& operator<<(std::ostream& os, indent_level const& indent);
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

std::ostream& operator<<(std::ostream& os, indent const&);

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

array_at_internal operator<<(std::ostream& str, array_at const& manip);

};


class PVAuxInfo;
class PostHandler;

class PVField;
class PVScalar;

class PVScalarArray;

class PVStructure;
class PVStructureArray;

/**
 * typedef for a pointer to a PVAuxInfo.
 */
typedef std::tr1::shared_ptr<PVAuxInfo> PVAuxInfoPtr;

/**
 * typedef for a pointer to a PostHandler.
 */
typedef std::tr1::shared_ptr<PostHandler> PostHandlerPtr;

/**
 * typedef for a pointer to a PVField.
 */
typedef std::tr1::shared_ptr<PVField> PVFieldPtr;
/**
 * typedef for a pointer to a array of pointer to PVField.
 */
typedef std::vector<PVFieldPtr> PVFieldPtrArray;
typedef std::vector<PVFieldPtr>::iterator PVFieldPtrArray_iterator;
typedef std::vector<PVFieldPtr>::const_iterator PVFieldPtrArray_const__iterator;

/**
 * typedef for a pointer to a PVScalar.
 */
typedef std::tr1::shared_ptr<PVScalar> PVScalarPtr;

/**
 * typedef for a pointer to a PVScalarArray.
 */
typedef std::tr1::shared_ptr<PVScalarArray> PVScalarArrayPtr;

/**
 * typedef for a pointer to a PVStructure.
 */
typedef std::tr1::shared_ptr<PVStructure> PVStructurePtr;
/**
 * typedef for a pointer to a array of pointer to PVStructure.
 */
typedef std::vector<PVStructurePtr> PVStructurePtrArray;
typedef std::vector<PVStructurePtr>::iterator PVStructurePtrArray_iterator;
typedef std::vector<PVStructurePtr>::const_iterator PVStructurePtrArray_const__iterator;

/**
 * typedef for a pointer to a PVStructureArray.
 */
typedef std::tr1::shared_ptr<PVStructureArray> PVStructureArrayPtr;
typedef std::vector<PVStructureArrayPtr> PVStructureArrayPtrArray;
typedef std::tr1::shared_ptr<PVStructureArrayPtrArray> PVStructureArrayPtrArrayPtr;

/**
 * This class provides auxillary information about a PVField.
 * Each item is stored as a PVScalar.
 * A (key,value) is provided for accessing the items where the key is a String.
 */
class PVAuxInfo : private NoDefaultMethods {
public:
    typedef std::map<String,PVScalarPtr> PVInfoMap;
    typedef std::map<String,PVScalarPtr>::iterator PVInfoIter;
    typedef std::pair<String,PVScalarPtr> PVInfoPair;

    /**
     * Constructor
     * @param The fields to which the Auxinfo is attached.
     */
    PVAuxInfo(PVField *pvField);
    /**
     * Destructor
     */
    ~PVAuxInfo();
    /**
     * Get the PVField to which the Auxinfo is attached.
     * @return The fields to which the Auxinfo is attached.
     */
    PVField * getPVField();
    /**
     * Add a new auxiliary item or retrieve the interface to an existing item. 
     *
     * @param key The key.
     * @param scalarType The scalarType for the new item being added/
     * @return The new PVScalar that has been added to the Auxinfo.
     */
    PVScalarPtr createInfo(String const & key,ScalarType scalarType);
    /**
     * Get the Auxinfo with the specified key.
     * @return The PVScalar or null if it does not exist.
     */
    PVScalarPtr getInfo(String const & key);
    /**
     * Get the map for the info.
     * @return The map;
     */
    PVInfoMap & getInfoMap();
    /**
     * Convert the Auxinfo to a string and add it to builder.
     * @param  builder The string builder.
     */
    void toString(StringBuilder buf);
    /**
     * Convert the Auxinfo to a string and add it to builder.
     * @param  builder The string builder.
     * @param  indentLevel The number of blanks at the beginning of new lines.
     */
    void toString(StringBuilder buf,int indentLevel);
private:
    PVScalarPtr nullPVScalar;
    PVField * pvField;
    PVInfoMap pvInfos;
    friend class PVDataCreate;
};

/**
 * This class is implemented by code that calls setPostHander
 */
class PostHandler 
{
public:
    POINTER_DEFINITIONS(PostHandler);
    /**
     * Destructor
     */
    virtual ~PostHandler(){}
    /**
     * This is called evertime postPut is called for this field.
     */
    virtual void postPut() = 0;
};

/**
 * PVField is the base class for each PVData field.
 * Each PVData field has an interface that extends PVField.
 */
class PVField
: virtual public Serializable,
  public std::tr1::enable_shared_from_this<PVField>
{
public:
    POINTER_DEFINITIONS(PVField);
    /**
     * Destructor
     */
    virtual ~PVField();
    /**
     * Called to report errors associated with the field.
     * @param message The message.
     * @param messageType The message type.
     */
    virtual void message(String message,MessageType messageType) ;
    /**
     * Get the fieldName for this field.
     * @return The name or empty string if top level field.
     */
    inline const String& getFieldName() const {return fieldName;}
    /**
     * Fully expand the name of this field using the
     * names of its parent fields with a dot '.' seperating
     * each name.
     */
    String getFullName() const;
    /**
     * Register the message requester.
     * At most one requester can be registered.
     * @param prequester The requester.
     */
    virtual void setRequester(RequesterPtr const &prequester);
    /**
     * Get offset of the PVField field within top level structure.
     * Every field within the PVStructure has a unique offset.
     * The top level structure has an offset of 0.
     * The first field within the structure has offset equal to 1.
     * The other offsets are determined by recursively traversing each structure of the tree.
     * @return The offset.
     */
    std::size_t getFieldOffset() const;
    /**
     * Get the next offset. If the field is a scalar or array field then this is just offset + 1.
     * If the field is a structure it is the offset of the next field after this structure.
     * Thus (nextOffset - offset) is always equal to the number of fields within the field.
     * @return The offset.
     */
    std::size_t getNextFieldOffset() const;
    /**
     * Get the total number of fields in this field.
     * This is equal to nextFieldOffset - fieldOffset.
     */
    std::size_t getNumberFields() const;
    /**
     * Get the PVAuxInfo interface for the PVField.
     * @return The PVAuxInfo interface.
     */
    PVAuxInfoPtr & getPVAuxInfo();
    /**
     * Is the field immutable, i.e. does it not allow changes.
     * @return (false,true) if it (is not, is) immutable.
     */
    bool isImmutable() const;
    /**
     * Set the field to be immutable, i. e. it can no longer be modified.
     * This is permanent, i.e. once done the field can onot be made mutable.
     */
    virtual void setImmutable();
    /**
     * Get the <i>Field</i> that describes the field.
     * @return Field, which is the reflection interface.
     */
    const FieldConstPtr & getField() const ;
    /**
     * Get the parent of this field.
     * @return The parent interface or null if this is PVRecord
     */
    PVStructure * getParent() const ;
    /**
     * Replace the data implementation for the field.
     * @param newPVField The new implementation
     */
    void replacePVField(const PVFieldPtr&  newPVField);
    /**
     * Rename the field name.
     * @param newName The new name.
     */
    void renameField(String const & newName);
    /**
     * postPut. Called when the field is updated by the implementation.
     */
    void postPut() ;
    /**
     * Set the handler for postPut.
     * At most one handler can be set.
     * @param postHandler The handler.
     */
    void setPostHandler(PostHandlerPtr const &postHandler);
    /**
     * Is this field equal to another field.
     * @param pv other field
     * @return (false,true) if (is not,is) equal.
     */
    virtual bool equals(PVField &pv);
    /**
     * Convert the PVField to a string.
     *  @param buf buffer for the result
     */
    virtual void toString(StringBuilder buf) ;
    /**
     * Convert the PVField to a string.
     * Each line is indented.
     * @param buf buffer for the result
     * @param indentLevel The indentation level.
     */
    virtual void toString(StringBuilder buf,int indentLevel) ;

    /**
     * Puts the PVField raw value to the stream.
     * @param o output stream.
     * @return The output stream.
     */
    virtual std::ostream& dumpValue(std::ostream& o) const = 0;


protected:
    PVField::shared_pointer getPtrSelf()
    {
        return shared_from_this();
    }
    PVField(FieldConstPtr field);
    void setParentAndName(PVStructure *parent, String const & fieldName);
    void replaceField(FieldConstPtr &field);
private:
    void message(String  message,MessageType messageType,String fullFieldName);
    static void computeOffset(const PVField *pvField);
    static void computeOffset(const PVField *pvField,std::size_t offset);
    String notImplemented;
    PVAuxInfoPtr pvAuxInfo;
    String fieldName;
    PVStructure *parent;
    FieldConstPtr field;
    size_t fieldOffset;
    size_t nextFieldOffset;
    bool immutable;
    RequesterPtr requester;
    PostHandlerPtr postHandler;
    std::tr1::shared_ptr<class Convert> convert;
    friend class PVDataCreate;
    friend class PVStructure;
};

std::ostream& operator<<(std::ostream& o, const PVField& f);

/**
 * PVScalar is the base class for each scalar field.
 */
class PVScalar : public PVField {
public:
    POINTER_DEFINITIONS(PVScalar);
    /**
     * Destructor
     */
    virtual ~PVScalar();
    typedef PVScalar &reference;
    typedef const PVScalar& const_reference;
    /**
     * Get the Scalar introspection interface for the PVScalar.
     * @return the interface.
     */
    const ScalarConstPtr getScalar() const ;

    /**
     * Convert and return the scalar value in the requested type.
     * Result type is determined from the function template argument
     * which must be one of the ScalarType enums.
     * Uses castUnsafe<TO>() for value conversion.
     @code
      PVScalar* pv = ...;
      uint32 val = pv->getAs<pvInt>();
     @endcode
     */
    template<ScalarType ID>
    inline typename ScalarTypeTraits<ID>::type getAs() const {
        typename ScalarTypeTraits<ID>::type result;
        this->getAs((void*)&result, ID);
        return result;
    }
    virtual void getAs(void *, ScalarType) const = 0;

    /**
     * Convert and assign the provided value.
     * The value type is determined from the function template argument
     * which must be one of the ScalarType enums.
     * Uses castUnsafe<TO>() for value conversion.
     @code
      PVScalar* pv = ...;
      pv->putFrom<pvInt>((int32)42);
     @endcode
     */
    template<ScalarType ID>
    inline void putFrom(typename ScalarTypeTraits<ID>::type val) {
        this->putFrom((const void*)&val, ID);
    }
    virtual void putFrom(const void *, ScalarType) = 0;

    virtual void assign(const PVScalar&) = 0;

protected:
    PVScalar(ScalarConstPtr const & scalar);
};

/**
 * Class that holds the data for each posssible scalar type.
 */
template<typename T>
class PVScalarValue : public PVScalar {
public:
    POINTER_DEFINITIONS(PVScalarValue);
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;

    static const ScalarType typeCode;

    /**
     * Destructor
     */
    virtual ~PVScalarValue() {}
    /**
     * Get the value.
     * @return The value.
     */
    virtual T get() const = 0;
    /**
     * Put a new value into the PVScalar.
     * @param The value.
     */
    virtual void put(T value) = 0;

    std::ostream& dumpValue(std::ostream& o) const
    {
    	return o << get();
    }

    // get operator
    // double value; doubleField >>= value;
    void operator>>=(T& value) const
	{
    	value = get();
	}

    // put operator
    // double value = 12.8; doubleField <<= value;
    void operator<<=(T value)
	{
    	put(value);
	}

protected:
    PVScalarValue(ScalarConstPtr const & scalar)
    : PVScalar(scalar) {}
    virtual void getAs(void * result, ScalarType rtype) const
    {
        const T src = get();
        castUnsafeV(1, rtype, result, typeCode, (const void*)&src);
    }
    virtual void putFrom(const void *src, ScalarType stype)
    {
        T result;
        castUnsafeV(1, typeCode, (void*)&result, stype, src);
        put(result);
    }
    virtual void assign(const PVScalar& scalar)
    {
        T result;
        scalar.getAs((void*)&result, typeCode);
        put(result);
    }

private:
    friend class PVDataCreate;
};

/**
 * typedefs for the various possible scalar types.
 */
typedef PVScalarValue<uint8> PVBoolean;
typedef PVScalarValue<int8> PVByte;
typedef PVScalarValue<int16> PVShort;
typedef PVScalarValue<int32> PVInt;
typedef PVScalarValue<int64> PVLong;
typedef PVScalarValue<uint8> PVUByte;
typedef PVScalarValue<uint16> PVUShort;
typedef PVScalarValue<uint32> PVUInt;
typedef PVScalarValue<uint64> PVULong;
typedef PVScalarValue<float> PVFloat;
typedef PVScalarValue<double> PVDouble;
typedef std::tr1::shared_ptr<PVBoolean> PVBooleanPtr;
typedef std::tr1::shared_ptr<PVByte> PVBytePtr;
typedef std::tr1::shared_ptr<PVShort> PVShortPtr;
typedef std::tr1::shared_ptr<PVInt> PVIntPtr;
typedef std::tr1::shared_ptr<PVLong> PVLongPtr;
typedef std::tr1::shared_ptr<PVUByte> PVUBytePtr;
typedef std::tr1::shared_ptr<PVUShort> PVUShortPtr;
typedef std::tr1::shared_ptr<PVUInt> PVUIntPtr;
typedef std::tr1::shared_ptr<PVULong> PVULongPtr;
typedef std::tr1::shared_ptr<PVFloat> PVFloatPtr;
typedef std::tr1::shared_ptr<PVDouble> PVDoublePtr;

/**
 * PVString is special case, since it implements SerializableArray
 */
class PVString : public PVScalarValue<String>, SerializableArray {
public:
    /**
     * Destructor
     */
    virtual ~PVString() {}
protected:
    PVString(ScalarConstPtr const & scalar)
    : PVScalarValue<String>(scalar) {}
};
typedef std::tr1::shared_ptr<PVString> PVStringPtr;


/**
 * PVArray is the base class for all array types, i.e. the scalarArray types and structureArray.
 */
class PVArray : public PVField, public SerializableArray {
public:
    POINTER_DEFINITIONS(PVArray);
    /**
     * Destructor
     */
    virtual ~PVArray();
    /**
     * Set the field to be immutable, i. e. it can no longer be modified.
     * This is permanent, i.e. once done the field can onot be made mutable.
     */
    virtual void setImmutable();
    /**
     * Get the array length.
     * @return The length.
     */
    std::size_t getLength() const;
    /**
     * Set the array length.
     * @param The length.
     */
    virtual void setLength(std::size_t length);
    /**
     * Get the array capacity.
     * @return The capacity.
     */
    std::size_t getCapacity() const;
    /**
     * Can the capacity be changed.
     * @return (false,true) if (can not, can) be changed.
     */
    bool isCapacityMutable() const;
    /**
     * Set the mutability of the array capacity.
     * @return false or true
     */
    void setCapacityMutable(bool isMutable);
    /**
     * Set the array capacity.
     * @param The capacity.
     */
    virtual void setCapacity(std::size_t capacity) = 0;

    virtual std::ostream& dumpValue(std::ostream& o, std::size_t index) const = 0;

protected:
    PVArray(FieldConstPtr const & field);
    void setCapacityLength(std::size_t capacity,std::size_t length);
private:
    class PVArrayPvt * pImpl;
    friend class PVDataCreate;
};

std::ostream& operator<<(format::array_at_internal const& manip, const PVArray& array);

/**
 * Class provided by caller of get
 */
template<typename T>
class PVArrayData {
private:
    std::vector<T> init;
public:
    POINTER_DEFINITIONS(PVArrayData);
    typedef T  value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    /**
     * The data array.
     */
    std::vector<T> & data;
    /**
     * The offset. This is the offset into the actual array of the first element in data,
     */
    std::size_t offset;
    PVArrayData()
    : data(init)
    {}
};


/**
 * Base class for a scalarArray.
 */
class PVScalarArray : public PVArray {
public:
    POINTER_DEFINITIONS(PVScalarArray);
    /**
     * Destructor
     */
    virtual ~PVScalarArray();
    typedef PVScalarArray &reference;
    typedef const PVScalarArray& const_reference;
    /**
     * Get the introspection interface
     * @return The interface.
     */
    const ScalarArrayConstPtr getScalarArray() const ;

    template<ScalarType ID>
    inline void getAs(typename ScalarTypeTraits<ID>::type* ptr,
                      size_t count, size_t offset = 0) const
    {
        getAs(ID, (void*)ptr, count, offset);
    }
    virtual void getAs(ScalarType, void*, size_t, size_t) const = 0;

    template<ScalarType ID>
    inline void putFrom(const typename ScalarTypeTraits<ID>::type* ptr,
                 size_t count, size_t offset = 0)
    {
        putFrom(ID, (const void*)ptr, count, offset);
    }
    virtual void putFrom(ScalarType, const void*, size_t ,size_t) = 0;

    virtual void assign(PVScalarArray& pv) = 0;

protected:
    PVScalarArray(ScalarArrayConstPtr const & scalarArray);
private:
    friend class PVDataCreate;
};

/**
 * This is provided by code that calls get.
 */
typedef PVArrayData<PVStructurePtr> StructureArrayData;

/**
 * Data class for a structureArray
 */
class PVStructureArray : public PVArray
{
public:
    POINTER_DEFINITIONS(PVStructureArray);
    typedef PVStructurePtr  value_type;
    typedef PVStructurePtr* pointer;
    typedef const PVStructurePtr* const_pointer;
    typedef PVArrayData<PVStructurePtr> ArrayDataType;
    typedef std::vector<PVStructurePtr> vector;
    typedef const std::vector<PVStructurePtr> const_vector;
    typedef std::tr1::shared_ptr<vector> shared_vector;
    typedef PVStructureArray &reference;
    typedef const PVStructureArray& const_reference;
    /**
     * Destructor
     */
    virtual ~PVStructureArray() {}
    /**
     * Set the array capacity.
     * @param capacity The length.
     */
    virtual void setCapacity(size_t capacity);
    /**
     * Set the array length.
     * @param length The length.
     */
    virtual void setLength(std::size_t length);
    /**
     * Get the introspection interface
     * @return The interface.
     */
    virtual StructureArrayConstPtr getStructureArray() const ;
    /**
     * Append new elements to the end of the array.
     * @param number The number of elements to add.
     * @return the new length of the array.
     */
    virtual std::size_t append(std::size_t number);
    /**
     * Remove elements from the array.
     * @param offset The offset of the first element to remove.
     * @param number The number of elements to remove.
     * @return (false,true) if the elements were removed.
     */
    virtual bool remove(std::size_t offset,std::size_t number);
    /**
     * Compress. This removes all null elements from the array.
     */
    virtual void compress();
    /**
     * Get array elements
     * @param offset The offset of the first element,
     * @param length The number of elements to get.
     * @param data The place where the data is placed.
     */
    virtual std::size_t get(std::size_t offset, std::size_t length,
        StructureArrayData &data);
    /**
     * Put data into the array.
     * @param offset The offset of the first element,
     * @param length The number of elements to get.
     * @param from The new values to put into the array.
     * @param fromOffset The offset in from.
     * @return The number of elements put into the array.
     */
    virtual std::size_t put(std::size_t offset,std::size_t length,
        const_vector const & from, std::size_t fromOffset);
    /**
     * Share data from another source.
     * @param value The data to share.
     * @param capacity The capacity of the array.
     * @param length The length of the array.
     */
    virtual void shareData(
         shared_vector const & value,
         std::size_t capacity,
         std::size_t length);
    virtual void serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher) const;
    virtual void deserialize(ByteBuffer *buffer,
        DeserializableControl *pflusher);
    virtual void serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher, std::size_t offset, std::size_t count) const ;
    virtual pointer get() { return &((*value.get())[0]); }
    virtual pointer get() const { return &((*value.get())[0]); }
    virtual vector const & getVector() {return *value;}
    virtual shared_vector const & getSharedVector() {return value;}

    virtual std::ostream& dumpValue(std::ostream& o) const;
    virtual std::ostream& dumpValue(std::ostream& o, std::size_t index) const;

protected:
    PVStructureArray(StructureArrayConstPtr const & structureArray);
private:
    StructureArrayConstPtr structureArray;
    shared_vector value;
    friend class PVDataCreate;
};


class PVStructure : public PVField, public BitSetSerializable
{
public:
    POINTER_DEFINITIONS(PVStructure);
    /**
     * Destructor
     */
    virtual ~PVStructure();
    typedef PVStructure & reference;
    typedef const PVStructure & const_reference;
    /**
     * Set the field to be immutable, i. e. it can no longer be modified.
     * This is permanent, i.e. once done the field can onot be made mutable.
     */
    virtual void setImmutable();
    /**
     * Get the introspection interface
     * @return The interface.
     */
    StructureConstPtr getStructure() const;
    /**
     * Get the array of pointers to the subfields in the structure.
     * @return The array.
     */
    const PVFieldPtrArray & getPVFields() const;
    /**
     * Get the subfield with the specified name.
     * @param fieldName The name of the field.
     * @return Pointer to the field or null if field does not exist.
     */
    PVFieldPtr getSubField(String const &fieldName) const;
    /**
     * Get the subfield with the specified offset.
     * @param fieldOffset The offset.
     * @return Pointer to the field or null if field does not exist.
     */
    PVFieldPtr getSubField(std::size_t fieldOffset) const;
    /**
     * Append a field to the structure.
     * @param fieldName The name of the field to append.
     * @param pvField The field to append.
     */
    void appendPVField(String const &fieldName,PVFieldPtr const & pvField);
    /**
     * Append fields to the structure.
     * @param fieldNames The names of the fields to add.
     * @param pvFields The fields to append.
     * @return Pointer to the field or null if field does not exist.
     */
    void appendPVFields(StringArray const & fieldNames,PVFieldPtrArray const & pvFields);
    /**
     * Remove a field from the structure.
     * @param fieldName The name of the field to remove.
     */
    void removePVField(String const &fieldName);
    /**
     * Get a boolean field with the specified name.
     * @param fieldName The name of the field to get.
     * @return Pointer to the field of null if a field with that name and type does not exist.
     */
    PVBooleanPtr getBooleanField(String const &fieldName) ;
    /**
     * Get a byte field with the specified name.
     * @param fieldName The name of the field to get.
     * @return Pointer to the field of null if a field with that name and type does not exist.
     */
    PVBytePtr getByteField(String const &fieldName) ;
    /**
     * Get a short field with the specified name.
     * @param fieldName The name of the field to get.
     * @return Pointer to the field of null if a field with that name and type does not exist.
     */
    PVShortPtr getShortField(String const &fieldName) ;
    /**
     * Get a int field with the specified name.
     * @param fieldName The name of the field to get.
     * @return Pointer to the field of null if a field with that name and type does not exist.
     */
    PVIntPtr getIntField(String const &fieldName) ;
    /**
     * Get a long field with the specified name.
     * @param fieldName The name of the field to get.
     * @return Pointer to the field of null if a field with that name and type does not exist.
     */
    PVLongPtr getLongField(String const &fieldName) ;
    /**
     * Get an unsigned byte field with the specified name.
     * @param fieldName The name of the field to get.
     * @return Pointer to the field of null if a field with that name and type does not exist.
     */
    PVUBytePtr getUByteField(String const &fieldName) ;
    /**
     * Get an unsigned short field with the specified name.
     * @param fieldName The name of the field to get.
     * @return Pointer to the field of null if a field with that name and type does not exist.
     */
    PVUShortPtr getUShortField(String const &fieldName) ;
    /**
     * Get an unsigned int field with the specified name.
     * @param fieldName The name of the field to get.
     * @return Pointer to the field of null if a field with that name and type does not exist.
     */
    PVUIntPtr getUIntField(String const &fieldName) ;
    /**
     * Get an unsigned long field with the specified name.
     * @param fieldName The name of the field to get.
     * @return Pointer to the field of null if a field with that name and type does not exist.
     */
    PVULongPtr getULongField(String const &fieldName) ;
    /**
     * Get a float field with the specified name.
     * @param fieldName The name of the field to get.
     * @return Pointer to the field of null if a field with that name and type does not exist.
     */
    PVFloatPtr getFloatField(String const &fieldName) ;
    /**
     * Get a double field with the specified name.
     * @param fieldName The name of the field to get.
     * @return Pointer to the field of null if a field with that name and type does not exist.
     */
    PVDoublePtr getDoubleField(String const &fieldName) ;
    /**
     * Get a string field with the specified name.
     * @param fieldName The name of the field to get.
     * @return Pointer to the field of null if a field with that name and type does not exist.
     */
    PVStringPtr getStringField(String const &fieldName) ;
    /**
     * Get a structure field with the specified name.
     * @param fieldName The name of the field to get.
     * @return Pointer to the field of null if a field with that name and type does not exist.
     */
    PVStructurePtr getStructureField(String const &fieldName) ;
    /**
     * Get a scalarArray field with the specified name.
     * @param fieldName The name of the field to get.
     * @param elementType The element type.
     * @return Pointer to the field of null if a field with that name and type does not exist.
     */
    PVScalarArrayPtr getScalarArrayField(
        String const &fieldName,ScalarType elementType) ;
    /**
     * Get a structureArray field with the specified name.
     * @param fieldName The name of the field to get.
     * @return Pointer to the field of null if a field with that name and type does not exist.
     */
    PVStructureArrayPtr getStructureArrayField(String const &fieldName) ;
    /**
     * Get the name if this structure extends another structure.
     * @return The string which may be null.
     */
    String getExtendsStructureName() const;
    /**
     * Put the extends name.
     * @param extendsStructureName The name.
     */
    bool putExtendsStructureName(
        String const &extendsStructureName);
    /**
     * Serialize.
     * @param pbuffer The byte buffer.
     * @param pflusher Interface to call when buffer is full.
     */
    virtual void serialize(
        ByteBuffer *pbuffer,SerializableControl *pflusher) const ;
    /**
     * Deserialize
     * @param pbuffer The byte buffer.
     * @param pflusher Interface to call when buffer is empty.
     */
    virtual void deserialize(
        ByteBuffer *pbuffer,DeserializableControl *pflusher);
    /**
     * Serialize.
     * @param pbuffer The byte buffer.
     * @param pflusher Interface to call when buffer is full.
     * @param pbitSet A bitset the specifies which fields to serialize.
    */
    virtual void serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher,BitSet *pbitSet) const;
    /**
     * Deserialize
     * @param pbuffer The byte buffer.
     * @param pflusher Interface to call when buffer is empty.
     * @param pbitSet A bitset the specifies which fields to deserialize.
     */
    virtual void deserialize(ByteBuffer *pbuffer,
        DeserializableControl*pflusher,BitSet *pbitSet);
    /**
     * Constructor
     * @param structure The introspection interface.
     */
    PVStructure(StructureConstPtr const & structure);
    /**
     * Constructor
     * @param structure The introspection interface.
     * @param pvFields The array of fields for the structure.
     */
    PVStructure(StructureConstPtr const & structure,PVFieldPtrArray const & pvFields);

    virtual std::ostream& dumpValue(std::ostream& o) const;

private:
    void   fixParentStructure();
    static PVFieldPtr nullPVField;
    static PVBooleanPtr nullPVBoolean;
    static PVBytePtr nullPVByte;
    static PVShortPtr nullPVShort;
    static PVIntPtr nullPVInt;
    static PVLongPtr nullPVLong;
    static PVUBytePtr nullPVUByte;
    static PVUShortPtr nullPVUShort;
    static PVUIntPtr nullPVUInt;
    static PVULongPtr nullPVULong;
    static PVFloatPtr nullPVFloat;
    static PVDoublePtr nullPVDouble;
    static PVStringPtr nullPVString;
    static PVStructurePtr nullPVStructure;
    static PVStructureArrayPtr nullPVStructureArray;
    static PVScalarArrayPtr nullPVScalarArray;

    PVFieldPtrArray pvFields;
    StructureConstPtr structurePtr;
    String extendsStructureName;
    friend class PVDataCreate;
};

template<typename T>
class PVValueArray : public PVScalarArray {
public:
    POINTER_DEFINITIONS(PVValueArray);
    typedef T  value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef PVArrayData<T> ArrayDataType;
    typedef std::vector<T> vector;
    typedef const std::vector<T> const_vector;
    typedef std::tr1::shared_ptr<vector> shared_vector;
    typedef PVValueArray & reference;
    typedef const PVValueArray & const_reference;

    static const ScalarType typeCode;

    /**
     * Destructor
     */
    virtual ~PVValueArray() {}
    /**
     * Get array elements
     * @param offset The offset of the first element,
     * @param length The number of elements to get.
     * @param data The place where the data is placed.
     */
    virtual std::size_t get(
         std::size_t offset, std::size_t length, ArrayDataType &data) = 0;
    /**
     * Put data into the array.
     * @param offset The offset of the first element,
     * @param length The number of elements to get.
     * @param from The new values to put into the array.
     * @param fromOffset The offset in from.
     * @return The number of elements put into the array.
     */
    virtual std::size_t put(std::size_t offset,
        std::size_t length, const_pointer from, std::size_t fromOffset) = 0;
    virtual std::size_t put(std::size_t offset,
        std::size_t length, const_vector &from, std::size_t fromOffset);
    /**
     * Share data from another source.
     * @param value The data to share.
     * @param capacity The capacity of the array.
     * @param length The length of the array.
     */
    virtual void shareData(
         shared_vector const & value,
         std::size_t capacity,
         std::size_t length) = 0;
    virtual pointer get() = 0;
    virtual pointer get() const = 0;
    virtual vector const & getVector() = 0;
    virtual shared_vector const & getSharedVector() = 0;

    std::ostream& dumpValue(std::ostream& o) const
    {
    	o << '[';
    	std::size_t len = getLength();
    	bool first = true;
    	for (std::size_t i = 0; i < len; i++)
    	{
    		if (first)
    			first = false;
    		else
    			o << ',';
    		dumpValue(o, i);
    	}
    	return o << ']';
    }

    std::ostream& dumpValue(std::ostream& o, size_t index) const
    {
    	return o << *(get() + index);
    }

    virtual void getAs(ScalarType dtype, void* ptr, size_t count, size_t offset) const
    {
        castUnsafeV(count, dtype, ptr, typeCode, (const void*)(get()+offset));
    }
    virtual void putFrom(ScalarType dtype, const void*ptr, size_t count, size_t offset)
    {
        castUnsafeV(count, typeCode, (void*)(get()+offset), dtype, ptr);
    }

    virtual void assign(PVScalarArray& pv)
    {
        if(this==&pv)
            return;
        if(isImmutable())
            throw std::invalid_argument("Destination is immutable");
        if(pv.isImmutable() && typeCode==pv.getScalarArray()->getElementType()) {
            PVValueArray& pvr = static_cast<PVValueArray&>(pv);
            shareData(pvr.getSharedVector(), pvr.getCapacity(), pvr.getLength());
        } else {
            setLength(pv.getLength());
            pv.getAs(typeCode, (void*)get(), std::min(getLength(),pv.getLength()), 0);
        }
    }

protected:
    PVValueArray(ScalarArrayConstPtr const & scalar)
    : PVScalarArray(scalar) {}
    friend class PVDataCreate;
};

template<typename T>
std::size_t PVValueArray<T>::put(
    std::size_t offset,
    std::size_t length,
    const_vector &from,
    std::size_t fromOffset)
{ return put(offset,length, &from[0], fromOffset); }

/**
 * Definitions for the various scalarArray types.
 */
typedef PVArrayData<uint8> BooleanArrayData;
typedef PVValueArray<uint8> PVBooleanArray;
typedef std::tr1::shared_ptr<PVBooleanArray> PVBooleanArrayPtr;

typedef PVArrayData<int8> ByteArrayData;
typedef PVValueArray<int8> PVByteArray;
typedef std::tr1::shared_ptr<PVByteArray> PVByteArrayPtr;

typedef PVArrayData<int16> ShortArrayData;
typedef PVValueArray<int16> PVShortArray;
typedef std::tr1::shared_ptr<PVShortArray> PVShortArrayPtr;

typedef PVArrayData<int32> IntArrayData;
typedef PVValueArray<int32> PVIntArray;
typedef std::tr1::shared_ptr<PVIntArray> PVIntArrayPtr;

typedef PVArrayData<int64> LongArrayData;
typedef PVValueArray<int64> PVLongArray;
typedef std::tr1::shared_ptr<PVLongArray> PVLongArrayPtr;

typedef PVArrayData<uint8> UByteArrayData;
typedef PVValueArray<uint8> PVUByteArray;
typedef std::tr1::shared_ptr<PVUByteArray> PVUByteArrayPtr;

typedef PVArrayData<uint16> UShortArrayData;
typedef PVValueArray<uint16> PVUShortArray;
typedef std::tr1::shared_ptr<PVUShortArray> PVUShortArrayPtr;

typedef PVArrayData<uint32> UIntArrayData;
typedef PVValueArray<uint32> PVUIntArray;
typedef std::tr1::shared_ptr<PVUIntArray> PVUIntArrayPtr;

typedef PVArrayData<uint64> ULongArrayData;
typedef PVValueArray<uint64> PVULongArray;
typedef std::tr1::shared_ptr<PVULongArray> PVULongArrayPtr;

typedef PVArrayData<float> FloatArrayData;
typedef PVValueArray<float> PVFloatArray;
typedef std::tr1::shared_ptr<PVFloatArray> PVFloatArrayPtr;

typedef PVArrayData<double> DoubleArrayData;
typedef PVValueArray<double> PVDoubleArray;
typedef std::tr1::shared_ptr<PVDoubleArray> PVDoubleArrayPtr;

typedef PVArrayData<String> StringArrayData;
typedef PVValueArray<String> PVStringArray;
typedef std::tr1::shared_ptr<PVStringArray> PVStringArrayPtr;

class PVDataCreate;
typedef std::tr1::shared_ptr<PVDataCreate> PVDataCreatePtr;
/**
 * This is a singlton class for creating data instances.
 */
class PVDataCreate {
public:
    static PVDataCreatePtr getPVDataCreate();
    /**
     * Create a PVField using given Field introspection data.
     * @param field The introspection data to be used to create PVField.
     * @return The PVField implementation.
     */
    PVFieldPtr createPVField(FieldConstPtr const & field);
    /**
     * Create a PVField using given a PVField to clone.
     * This method calls the appropriate createPVScalar, createPVArray, or createPVStructure.
     * @param fieldToClone The field to clone.
     * @return The PVField implementation
     */
    PVFieldPtr createPVField(PVFieldPtr const & fieldToClone);
    /**
     * Create an implementation of a scalar field reusing the Scalar introspection interface.
     * @param scalar The introspection interface.
     * @return The PVScalar implementation.
     */
    PVScalarPtr createPVScalar(ScalarConstPtr const & scalar);
    /**
     * Create an implementation of a scalar field. A Scalar introspection interface is created.
     * @param fieldType The field type.
     * @return The PVScalar implementation.
     */
    PVScalarPtr createPVScalar(ScalarType scalarType);
    /**
     * Create an implementation of a scalar field by cloning an existing PVScalar.
     * The new PVScalar will have the same value and auxInfo as the original.
     * @param scalarToClone The PVScalar to clone.
     * @return The PVScalar implementation.
     */
    PVScalarPtr createPVScalar(PVScalarPtr const & scalarToClone);
    /**
     * Create an implementation of an array field reusing the Array introspection interface.
     * @param array The introspection interface.
     * @return The PVScalarArray implementation.
     */
    PVScalarArrayPtr createPVScalarArray(ScalarArrayConstPtr const & scalarArray);
    /**
     * Create an implementation for an array field. An Array introspection interface is created.
     * @param parent The parent interface.
     * @param elementType The element type.
     * @return The PVScalarArray implementation.
     */
    PVScalarArrayPtr createPVScalarArray(ScalarType elementType);
    /**
     * Create an implementation of an array field by cloning an existing PVArray.
     * The new PVArray will have the same value and auxInfo as the original.
     * @param arrayToClone The PVScalarArray to clone.
     * @return The PVScalarArray implementation.
     */
    PVScalarArrayPtr createPVScalarArray(PVScalarArrayPtr const  & scalarArrayToClone);
    /**
     * Create an implementation of an array with structure elements.
     * @param structureArray The introspection interface.
     * All elements share the same introspection interface.
     * @return The PVStructureArray implementation.
     */
    PVStructureArrayPtr createPVStructureArray(StructureArrayConstPtr const & structureArray);
    /**
     * Create implementation for PVStructure.
     * @param structure The introspection interface.
     * @return The PVStructure implementation
     */
    PVStructurePtr createPVStructure(StructureConstPtr const & structure);
    /**
     * Create implementation for PVStructure.
     * @param fieldNames The field names.
     * @param pvFields Array of PVFields
     * @return The PVStructure implementation
     */
    PVStructurePtr createPVStructure(
        StringArray const & fieldNames,PVFieldPtrArray const & pvFields);
     /**
      * Create implementation for PVStructure.
      * @param structToClone A structure. Each subfield and any auxInfo is cloned and added to the newly created structure.
      * @return The PVStructure implementation.
      */
   PVStructurePtr createPVStructure(PVStructurePtr const & structToClone);
private:
   PVDataCreate();
   FieldCreatePtr fieldCreate;
};

/**
 * Get the single class that implemnents PVDataCreate
 * @param The PVDataCreate factory.
 */

extern PVDataCreatePtr getPVDataCreate();
    
}}
#endif  /* PVDATA_H */
