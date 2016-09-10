/* pvData.h */
/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */
/**
 *  @author mrk
 */
#ifndef PVDATA_H
#define PVDATA_H

#if defined(_WIN32) && !defined(NOMINMAX)
#define NOMINMAX
#endif

#include <string>
#include <map>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <iomanip>

#include <pv/pvIntrospect.h>
#include <pv/typeCast.h>
#include <pv/sharedVector.h>

#include <shareLib.h>
#include <compilerDependencies.h>

#if defined(vxWorks) && !defined(_WRS_VXWORKS_MAJOR)
typedef class std::ios std::ios_base;
#endif

#if defined(_WIN32) && !defined(_MINGW)
#pragma warning(disable: 4344) // A call to a function using explicit template
// arguments calls a different function than it would if explicit arguments had not been specified
// Removes warnings for template getSubField.
#endif

#define USAGE_DEPRECATED EPICS_DEPRECATED

#if defined(__GNUC__) && !(defined(vxWorks) && !defined(_WRS_VXWORKS_MAJOR))
#define USAGE_ERROR(MSG) __attribute__((error(MSG)))
#else
#define USAGE_ERROR(MSG) { throw std::runtime_error(MSG); }
#endif

namespace epics { namespace pvData { 

class PostHandler;

class PVField;
class PVScalar;

class PVScalarArray;

class PVStructure;

class PVUnion;



template<typename T> class PVScalarValue;
template<typename T> class PVValueArray;


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

typedef PVValueArray<PVStructurePtr> PVStructureArray;
typedef std::tr1::shared_ptr<PVStructureArray> PVStructureArrayPtr;
typedef std::vector<PVStructureArrayPtr> PVStructureArrayPtrArray;
typedef std::tr1::shared_ptr<PVStructureArrayPtrArray> PVStructureArrayPtrArrayPtr;

/**
 * typedef for a pointer to a PVUnion.
 */
typedef std::tr1::shared_ptr<PVUnion> PVUnionPtr;
/**
 * typedef for a pointer to a array of pointer to PVUnion.
 */
typedef std::vector<PVUnionPtr> PVUnionPtrArray;
typedef std::vector<PVUnionPtr>::iterator PVUnionPtrArray_iterator;
typedef std::vector<PVUnionPtr>::const_iterator PVUnionPtrArray_const__iterator;

/**
 * typedef for a pointer to a PVUnionArray.
 */

typedef PVValueArray<PVUnionPtr> PVUnionArray;
typedef std::tr1::shared_ptr<PVUnionArray> PVUnionArrayPtr;
typedef std::vector<PVUnionArrayPtr> PVUnionArrayPtrArray;
typedef std::tr1::shared_ptr<PVUnionArrayPtrArray> PVUnionArrayPtrArrayPtr;

class PVDataCreate;
typedef std::tr1::shared_ptr<PVDataCreate> PVDataCreatePtr;

/**
 * @brief This class is implemented by code that calls setPostHander
 *
 */
class epicsShareClass PostHandler 
{
public:
    POINTER_DEFINITIONS(PostHandler);
    /**
     * Destructor
     */
    virtual ~PostHandler(){}
    /**
     * This is called every time postPut is called for this field.
     */
    virtual void postPut() = 0;
};

/**
 * @brief PVField is the base class for each PVData field.
 *
 * Each PVData field has an interface that extends PVField.
 */
class epicsShareClass PVField
: virtual public Serializable,
  public std::tr1::enable_shared_from_this<PVField>
{
public:
    POINTER_DEFINITIONS(PVField);
    /**
     * Constructor
     */
    PVField() {};
    /**
     * Destructor
     */
    virtual ~PVField();
    /**
     * Get the fieldName for this field.
     * @return The name or empty string if top-level field.
     */
    inline const std::string& getFieldName() const {return fieldName;}
    /**
     * Fully expand the name of this field using the
     * names of its parent fields with a dot '.' separating
     * each name.
     */
    std::string getFullName() const;
    /**
     * Get offset of the PVField field within top-level structure.
     * Every field within the PVStructure has a unique offset.
     * The top-level structure has an offset of 0.
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
     * Is the field immutable, i.e. does it not allow changes.
     * @return (false,true) if it (is not, is) immutable.
     */
    bool isImmutable() const;
    /**
     * Set the field to be immutable, i.e. it can no longer be modified.
     * This is permanent, i.e. once done the field cannot be made mutable.
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
     * Puts the PVField raw value to the stream.
     * @param o output stream.
     * @return The output stream.
     */
    virtual std::ostream& dumpValue(std::ostream& o) const = 0;

    void copy(const PVField& from);
    void copyUnchecked(const PVField& from);

protected:
    PVField::shared_pointer getPtrSelf()
    {
        return shared_from_this();
    }
    explicit PVField(FieldConstPtr field);
    void setParentAndName(PVStructure *parent, std::string const & fieldName);
private:
    static void computeOffset(const PVField *pvField);
    static void computeOffset(const PVField *pvField,std::size_t offset);
    std::string fieldName;
    PVStructure *parent;
    FieldConstPtr field;
    size_t fieldOffset;
    size_t nextFieldOffset;
    bool immutable;
    PostHandlerPtr postHandler;
    friend class PVDataCreate;
    friend class PVStructure;
};

epicsShareExtern std::ostream& operator<<(std::ostream& o, const PVField& f);

/**
 * @brief PVScalar is the base class for each scalar field.
 *
 */
class epicsShareClass PVScalar : public PVField {
    // friend our child class(s) so that it
    // can call protected methods of other
    // PVScalar instances.
    template<typename E> friend class PVScalarValue;
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
    template<typename T>
    inline T getAs() const {
        T result;
        this->getAs((void*)&result, (ScalarType)ScalarTypeID<T>::value);
        return result;
    }
protected:
    virtual void getAs(void *, ScalarType) const = 0;
public:

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
    template<typename T>
    inline void putFrom(T val) {
        this->putFrom((const void*)&val, (ScalarType)ScalarTypeID<T>::value);
    }
protected:
    virtual void putFrom(const void *, ScalarType) = 0;
public:

    virtual void assign(const PVScalar&) = 0;

    virtual void copy(const PVScalar& from) = 0;
    virtual void copyUnchecked(const PVScalar& from) = 0;

protected:
    explicit PVScalar(ScalarConstPtr const & scalar);
};

/**
 * @brief Class that holds the data for each possible scalar type.
 *
 */
template<typename T>
class epicsShareClass PVScalarValue : public PVScalar {
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
     * @param value The value.
     */
    virtual void put(T value) = 0;

    std::ostream& dumpValue(std::ostream& o) const
    {
    	return o << get();
    }

    // get operator
    // double value; doubleField >>= value;
    // NOTE: virtual is needed for MS C++ compiler to get this operator exported
    virtual void operator>>=(T& value) const
	{
    	value = get();
	}

    // put operator
    // double value = 12.8; doubleField <<= value;
    // NOTE: virtual is needed for MS C++ compiler to get this operator exported
    virtual void operator<<=(T value)
	{
    	put(value);
	}

    template<typename T1>
    inline T1 getAs() const {
        T1 result(castUnsafe<T1,T>(get()));
        return result;
    }

    template<typename T1>
    inline void putFrom(T1 val) {
        put(castUnsafe<T,T1>(val));
    }

    virtual void assign(const PVScalar& scalar)
    {
        if(isImmutable())
            throw std::invalid_argument("destination is immutable");
        copyUnchecked(scalar);
    }
    virtual void copy(const PVScalar& from)
    {
        assign(from);
    }
    virtual void copyUnchecked(const PVScalar& from)
    {
        if(this==&from)
            return;
        T result;
        from.getAs((void*)&result, typeCode);
        put(result);
    }

protected:
    explicit PVScalarValue(ScalarConstPtr const & scalar)
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

private:
    friend class PVDataCreate;
};

/**
 * @brief Some explicit specializations exist
 */
template<> inline
std::ostream& PVScalarValue<int8>::dumpValue(std::ostream& o) const
{
    return o << static_cast<int>(get());
}

template<> inline
std::ostream& PVScalarValue<uint8>::dumpValue(std::ostream& o) const
{
    return o << static_cast<unsigned int>(get());
}

template<> inline
std::ostream& PVScalarValue<boolean>::dumpValue(std::ostream& o) const
{
    return o << std::boolalpha << static_cast<bool>(get());
}

/**
 * typedefs for the various possible scalar types.
 */
typedef PVScalarValue<boolean> PVBoolean;
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
 * @brief PVString is special case, since it implements SerializableArray
 *
 */
class epicsShareClass PVString : public PVScalarValue<std::string>, SerializableArray {
public:
    /**
     * Destructor
     */
    virtual ~PVString() {}
protected:
    explicit PVString(ScalarConstPtr const & scalar)
    : PVScalarValue<std::string>(scalar) {}
};
typedef std::tr1::shared_ptr<PVString> PVStringPtr;


/**
 * @brief PVArray is the base class for all array types.
 *
 * The array types are unionArray, strucrtureArray and scalarArray.
 * There is a scalarArray type for each scalarType.
 *
 */
class epicsShareClass PVArray : public PVField, public SerializableArray {
public:
    POINTER_DEFINITIONS(PVArray);
    /**
     * Constructor
     */
    PVArray(){};
    /**
     * Destructor
     */
    virtual ~PVArray(){};
    /**
     * Get the introspection interface
     * @return The interface.
     */
    virtual ArrayConstPtr getArray() const = 0;
    /**
     * Set the field to be immutable, i.e. it can no longer be modified.
     * This is permanent, i.e. once done the field cannot be made mutable.
     */
    virtual void setImmutable();
    /**
     * Get the array length.
     * @return The length.
     */
    virtual std::size_t getLength() const = 0;
    /**
     * Set the array length.
     * @param length The length.
     */
    virtual void setLength(std::size_t length) = 0;
    /**
     * Get the array capacity.
     * @return The capacity.
     */
    virtual std::size_t getCapacity() const = 0;
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
     * @param capacity The capacity.
     */
    virtual void setCapacity(std::size_t capacity) = 0;

    using PVField::dumpValue;
    virtual std::ostream& dumpValue(std::ostream& o, std::size_t index) const = 0;

protected:
    explicit PVArray(FieldConstPtr const & field);
    void checkLength(size_t length);
private:
    bool capacityMutable;
    friend class PVDataCreate;
};

epicsShareExtern std::ostream& operator<<(format::array_at_internal const& manip, const PVArray& array);

/**
 * @brief Base class for a scalarArray.
 *
 */
class epicsShareClass PVScalarArray : public PVArray {
public:
    POINTER_DEFINITIONS(PVScalarArray);
    /**
     * Constructor
     */
    PVScalarArray() {};
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

protected:
    virtual void _getAsVoid(shared_vector<const void>&) const = 0;
    virtual void _putFromVoid(const shared_vector<const void>&) = 0;
public:

    /**
     * Fetch the current value and convert to the requested type.
     *
     * A copy is made if the requested type does not match
     * the element type.  If the types do match then
     * no copy is made.
     */
    template<typename T>
    void
    getAs(shared_vector<const T>& out) const
    {
        shared_vector<const void> temp;
        _getAsVoid(temp);
        out = shared_vector_convert<const T>(temp);
    }

    /**
     * Assign the given value after conversion.
     *
     * A copy and element-wise conversion is performed unless
     * the element type of the PVScalarArray matches the
     * type of the provided data.
     * If the types do match then a new reference to the provided
     * data is kept.
     *
     * Calls postPut()
     */
    template<typename T>
    inline void putFrom(const shared_vector<const T>& inp)
    {
        shared_vector<const void> temp(static_shared_vector_cast<const void>(inp));
        _putFromVoid(temp);
    }

    /**
     * Assign the given PVScalarArray's value.
     *
     * A copy and element-wise conversion is performed unless
     * the element type of the PVScalarArray matches the
     * type of the provided data.
     * If the types do match then a new reference to the provided
     * data is kept.
     */
    void assign(const PVScalarArray& pv) {
        if (isImmutable())
            throw std::invalid_argument("destination is immutable");
        copyUnchecked(pv);
    }

    void copy(const PVScalarArray& from) {
        assign(from);
    }

    void copyUnchecked(const PVScalarArray& from) {
        if (this==&from)
            return;
        shared_vector<const void> temp;
        from._getAsVoid(temp);
        _putFromVoid(temp);
    }

protected:
    explicit PVScalarArray(ScalarArrayConstPtr const & scalarArray);
private:
    friend class PVDataCreate;
};


/**
 * @brief Data interface for a structure,
 *
 */
class epicsShareClass PVStructure : public PVField, public BitSetSerializable
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
     * Set the field to be immutable, i.e. it can no longer be modified.
     * This is permanent, i.e. once done the field cannot be made mutable.
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
    FORCE_INLINE PVFieldPtr getSubField(std::string const &fieldName) const
    {
        return getSubField(fieldName.c_str());
    }

    PVFieldPtr getSubField(const char *fieldName) const;

    /**
     * Get a subfield with the specified name.
     * @param fieldName a '.' separated list of child field names (no whitespace allowed)
     * @returns A pointer to the sub-field or null if field does not exist or has a different type
     * @code
     *   PVIntPtr ptr = pvStruct->getSubField<PVInt>("substruct.leaffield");
     * @endcode
     */
    template<typename PVT>
    FORCE_INLINE std::tr1::shared_ptr<PVT> getSubField(std::string const &fieldName) const
    {
        return this->getSubField<PVT>(fieldName.c_str());
    }

    template<typename PVT>
    std::tr1::shared_ptr<PVT> getSubField(const char *name) const
    {
        PVField *raw = getSubFieldImpl(name, false);
        if (raw)
            return std::tr1::dynamic_pointer_cast<PVT>(raw->shared_from_this());
        else
            return std::tr1::shared_ptr<PVT>();
    }

    /**
     * Get the subfield with the specified offset.
     * @param fieldOffset The offset.
     * @return Pointer to the field or null if field does not exist.
     */
    PVFieldPtr getSubField(std::size_t fieldOffset) const;

    /**
     * Get the subfield with the specified offset.
     * @param fieldOffset The offset.
     * @return Pointer to the field or null if field does not exist.
     */
    template<typename PVT>
    std::tr1::shared_ptr<PVT> getSubField(std::size_t fieldOffset) const
    {
        PVFieldPtr pvField = getSubField(fieldOffset);
        if (pvField.get())
            return std::tr1::dynamic_pointer_cast<PVT>(pvField);
        else
            return std::tr1::shared_ptr<PVT>();
    }

    /**
     * Get a subfield with the specified name.
     * @param fieldName a '.' separated list of child field names (no whitespace allowed)
     * @returns A reference to the sub-field (never NULL)
     * @throws std::runtime_error if the requested sub-field doesn't exist, or has a different type
     */
    FORCE_INLINE PVFieldPtr getSubFieldT(std::string const &fieldName) const
    {
        return getSubFieldImpl(fieldName.c_str())->shared_from_this();
    }

    /**
     * Get a subfield with the specified name.
     * @param fieldName a '.' separated list of child field names (no whitespace allowed)
     * @returns A reference to the sub-field (never NULL)
     * @throws std::runtime_error if the requested sub-field doesn't exist, or has a different type
     * @code
     *   PVIntPtr ptr = pvStruct->getSubFieldT<PVInt>("substruct.leaffield");
     * @endcode
     */
    template<typename PVT>
    FORCE_INLINE std::tr1::shared_ptr<PVT> getSubFieldT(std::string const &fieldName) const
    {
        return this->getSubFieldT<PVT>(fieldName.c_str());
    }

    template<typename PVT>
    std::tr1::shared_ptr<PVT> getSubFieldT(const char *name) const;

    /**
     * Get the subfield with the specified offset.
     * @param fieldOffset The offset.
     * @returns A reference to the sub-field (never NULL)
     * @throws std::runtime_error if the requested sub-field doesn't exist
     */
    PVFieldPtr getSubFieldT(std::size_t fieldOffset) const;

    /**
     * Get the subfield with the specified offset.
     * @param fieldOffset The offset.
     * @returns A reference to the sub-field (never NULL)
     * @throws std::runtime_error if the requested sub-field doesn't exist, or has a different type 
     */
    template<typename PVT>
    std::tr1::shared_ptr<PVT> getSubFieldT(std::size_t fieldOffset) const;

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
    explicit PVStructure(StructureConstPtr const & structure);
    /**
     * Constructor
     * @param structure The introspection interface.
     * @param pvFields The array of fields for the structure.
     */
    PVStructure(StructureConstPtr const & structure,PVFieldPtrArray const & pvFields);

    virtual std::ostream& dumpValue(std::ostream& o) const;

    void copy(const PVStructure& from);

    void copyUnchecked(const PVStructure& from);
    void copyUnchecked(const PVStructure& from, const BitSet& maskBitSet, bool inverse = false);

private:
    PVField *getSubFieldImpl(const char *name, bool throws = true) const;

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
    static PVUnionPtr nullPVUnion;
    static PVUnionArrayPtr nullPVUnionArray;
    static PVScalarArrayPtr nullPVScalarArray;

    PVFieldPtrArray pvFields;
    StructureConstPtr structurePtr;
    std::string extendsStructureName;
    friend class PVDataCreate;
};


template<typename PVT>
std::tr1::shared_ptr<PVT> PVStructure::getSubFieldT(const char *name) const
{
    std::tr1::shared_ptr<PVT> pvField = std::tr1::dynamic_pointer_cast<PVT>(
        getSubFieldImpl(name)->shared_from_this());

    if (pvField.get())
        return pvField;
    else
    {
        std::stringstream ss;
        ss << "Failed to get field: " << name << " (Field has wrong type)";
        throw std::runtime_error(ss.str());
    }
}

template<typename PVT>
std::tr1::shared_ptr<PVT> PVStructure::getSubFieldT(std::size_t fieldOffset) const
{
    std::tr1::shared_ptr<PVT> pvField = std::tr1::dynamic_pointer_cast<PVT>(
        getSubFieldT(fieldOffset));
    if (pvField.get())
        return pvField;
    else
    {
        std::stringstream ss;
        ss << "Failed to get field with offset "
           << fieldOffset << " (Field has wrong type)";
        throw std::runtime_error(ss.str());
    }
}

/**
 * @brief PVUnion has a single subfield.
 *
 * The type for the subfield is specified by a union introspection interface.
 *
 */
class epicsShareClass PVUnion : public PVField
{
public:
    POINTER_DEFINITIONS(PVUnion);
    /**
     * Destructor
     */
    virtual ~PVUnion();
    typedef PVUnion & reference;
    typedef const PVUnion & const_reference;

	/**
	 * Undefined index.
	 * Default value upon PVUnion construction. Can be set by the user.
	 * Corresponds to @c null value.
	 */
	static int32 UNDEFINED_INDEX;

    /**
     * Get the introspection interface
     * @return The interface.
     */
    UnionConstPtr getUnion() const;

    /**
     * Get the @c PVField value stored in the field.
     * @return @c PVField value of field, @c null if {@code getSelectedIndex() == UNDEFINED_INDEX}.
     */
    PVFieldPtr get() const;
   
    template<typename PVT>
    std::tr1::shared_ptr<PVT> get() const {
        return std::tr1::dynamic_pointer_cast<PVT>(get());
    }
        
    /**
     * Select field (set index) and get the field at the index.
     * @param index index of the field to select.
     * @return corresponding PVField (of undetermined value), @c null if {@code index == UNDEFINED_INDEX}.
     * @throws std::invalid_argument if index is invalid (out of range).
     */
    PVFieldPtr select(int32 index);

    template<typename PVT>
    std::tr1::shared_ptr<PVT> select(int32 index) {
        return std::tr1::dynamic_pointer_cast<PVT>(select(index));
    }

    /**
     * Select field (set index) and get the field by given name.
     * @param fieldName the name of the field to select.
     * @return corresponding PVField (of undetermined value).
     * @throws std::invalid_argument if field does not exist.
     */
    PVFieldPtr select(std::string const & fieldName);

    template<typename PVT>
    std::tr1::shared_ptr<PVT> select(std::string const & fieldName) {
        return std::tr1::dynamic_pointer_cast<PVT>(select(fieldName));
    }

    /**
     * Get selected field index.
     * @return selected field index.
     */
    int32 getSelectedIndex() const;
    
    /**
     * Get selected field name.
     * @return selected field name, empty string if field does not exist.
     */
    std::string getSelectedFieldName() const;
    
    /**
     * Set the @c PVField (by reference!) as selected field.
     * If a value is not a valid union field an @c std::invalid_argument
     * exception is thrown.
     * @param value the field to set.
     */
    void set(PVFieldPtr const & value);
    /**
     * Set the @c PVField (by reference!) as field at given index.
     * If a value is not a valid union field an @c std::invalid_argument
     * exception is thrown.
     * Use @c select(int32) to put by value.
     * @param index index of a field to put.
     * @param value the field to set.
     * @see #select(int32)
     */
    void set(int32 index, PVFieldPtr const & value);
    /**
     * Set the @c PVField (by reference!) as field by given name.
     * If a value is not a valid union field an @c std::invalid_argument
     * exception is thrown.
     * Use @c select(std::string const &) to put by value.
     * @param fieldName Name of the field to put.
     * @param value the field to set.
     * @see #select(std::string const &)
     */
    void set(std::string const & fieldName, PVFieldPtr const & value);

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
     * Constructor
     * @param punion The introspection interface.
     */
    explicit PVUnion(UnionConstPtr const & punion);

    virtual std::ostream& dumpValue(std::ostream& o) const;

    void copy(const PVUnion& from);
    void copyUnchecked(const PVUnion& from);

private:
    static PVDataCreatePtr pvDataCreate;

    friend class PVDataCreate;
    UnionConstPtr unionPtr;

	int32 selector;
	PVFieldPtr value;
	bool variant;  
};


namespace detail {
    // adaptor to allow epics::pvData::shared_vector to hold a reference
    // to a shared_ptr<std::vector<> >
    template<typename T>
    struct shared_ptr_vector_deletor {
        typedef std::tr1::shared_ptr<std::vector<T> > shared_vector;
        shared_vector vec;
        shared_ptr_vector_deletor(const shared_vector& v)
            :vec(v) {}
        void operator()(T*){vec.reset();}
    };

    template<typename T, class Base>
    class PVVectorStorage : public Base
    {
    public:
        typedef T  value_type;
        typedef T* pointer;
        typedef const T* const_pointer;

        //TODO: full namespace can be removed along with local typedef 'shared_vector'
        typedef ::epics::pvData::shared_vector<T> svector;
        typedef ::epics::pvData::shared_vector<const T> const_svector;


    protected:
        PVVectorStorage() : Base() {}

        template<typename A>
        explicit PVVectorStorage(A a) : Base(a) {}
    public:
        virtual ~PVVectorStorage(){};

        // Primitive array manipulations

        //! Fetch a read-only view of the current array data
        virtual const_svector view() const = 0;

        /** Exchange our contents for the provided.
         *
         @throws std::logic_error for Immutable arrays.
         *
         *  Callers must ensure that postPut() is called
         *  after the last swap() operation.
         *
         *  Before you call this directly, consider using
         *  the reuse(), or replace() methods.
         */
        virtual void swap(const_svector& other) = 0;

        //! Discard current contents and replaced with the provided.
        //! Fails for Immutable arrays
        //! calls postPut()
        virtual void replace(const const_svector& next) = 0;

        // Derived operations

        /** Remove and return the current array data
         * or an unique copy if shared.
         *
         * Does @b not (and should not) call postPut()
         *
         * The returned shared_vector<T> will
         * have unique()==true.
         */
        inline svector reuse()
        {
            const_svector result;
            this->swap(result);
            return thaw(result);
        }

    };
} // namespace detail

/**
 * @brief template class for all extensions of PVArray.
 *
 * The direct extensions are pvBooleanArray, pvByteArray, ..., pvStringArray.
 * There are specializations for PVStringArray, PVStructureArray, and PVUnionArray.
 *
 */
template<typename T>
class epicsShareClass PVValueArray : public detail::PVVectorStorage<T,PVScalarArray> {
    typedef detail::PVVectorStorage<T,PVScalarArray> base_t;
public:
    POINTER_DEFINITIONS(PVValueArray);
    typedef T  value_type;
    typedef T* pointer;
    typedef const T* const_pointer;

    //TODO: full namespace can be removed along with local typedef 'shared_vector'
    typedef ::epics::pvData::shared_vector<T> svector;
    typedef ::epics::pvData::shared_vector<const T> const_svector;


    static const ScalarType typeCode;

    /**
     * Destructor
     */
    virtual ~PVValueArray() {}

    /**
     * Get introspection interface.
     */
    virtual ArrayConstPtr getArray() const
    {
        return std::tr1::static_pointer_cast<const Array>(this->getField());
    }

    std::ostream& dumpValue(std::ostream& o) const
    {
        const_svector v(this->view());
        typename const_svector::const_iterator it(v.begin()),
                                      end(v.end());
    	o << '[';
        if(it!=end) {
            o << print_cast(*it++);
            for(; it!=end; ++it)
                o << ',' << print_cast(*it);

        }
    	return o << ']';
    }

    std::ostream& dumpValue(std::ostream& o, size_t index) const
    {
        return o << print_cast(this->view().at(index));
    }

protected:
    virtual void _getAsVoid(epics::pvData::shared_vector<const void>& out) const
    {
        out = static_shared_vector_cast<const void>(this->view());
    }

    virtual void _putFromVoid(const epics::pvData::shared_vector<const void>& in)
    {
        // TODO: try to re-use storage
        this->replace(shared_vector_convert<const T>(in));
    }

    explicit PVValueArray(ScalarArrayConstPtr const & scalar)
    : base_t(scalar) {}
    friend class PVDataCreate;
};


/**
 * @brief Data class for a structureArray
 *
 */
template<>
class epicsShareClass PVValueArray<PVStructurePtr> : public detail::PVVectorStorage<PVStructurePtr,PVArray>
{
    typedef detail::PVVectorStorage<PVStructurePtr,PVArray> base_t;
public:
    POINTER_DEFINITIONS(PVStructureArray);
    typedef PVStructurePtr  value_type;
    typedef PVStructurePtr* pointer;
    typedef const PVStructurePtr* const_pointer;
    typedef PVStructureArray &reference;
    typedef const PVStructureArray& const_reference;

    //TODO: full namespace can be removed along with local typedef 'shared_vector'
    typedef ::epics::pvData::shared_vector<PVStructurePtr> svector;
    typedef ::epics::pvData::shared_vector<const PVStructurePtr> const_svector;
    /**
     * Destructor
     */
    virtual ~PVValueArray() {}

    virtual ArrayConstPtr getArray() const
    {
        return std::tr1::static_pointer_cast<const Array>(structureArray);
    }

    virtual size_t getLength() const {return value.size();}
    virtual size_t getCapacity() const {return value.capacity();}

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
    StructureArrayConstPtr getStructureArray() const {return structureArray;}
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

    virtual const_svector view() const { return value; }
    virtual void swap(const_svector &other);
    virtual void replace(const const_svector &other) {
        checkLength(other.size());
        value = other;
        PVField::postPut();
    }

    virtual void serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher) const;
    virtual void deserialize(ByteBuffer *buffer,
        DeserializableControl *pflusher);
    virtual void serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher, std::size_t offset, std::size_t count) const ;

    virtual std::ostream& dumpValue(std::ostream& o) const;
    virtual std::ostream& dumpValue(std::ostream& o, std::size_t index) const;

    void copy(const PVStructureArray& from);
    void copyUnchecked(const PVStructureArray& from);

protected:
    explicit PVValueArray(StructureArrayConstPtr const & structureArray)
        :base_t(structureArray)
        ,structureArray(structureArray)
    {}
private:
    StructureArrayConstPtr structureArray;
    const_svector value;
    friend class PVDataCreate;
};



/**
 * @brief Data class for a unionArray
 *
 */
template<>
class epicsShareClass PVValueArray<PVUnionPtr> : public detail::PVVectorStorage<PVUnionPtr,PVArray>
{
    typedef detail::PVVectorStorage<PVUnionPtr,PVArray> base_t;
public:
    POINTER_DEFINITIONS(PVUnionArray);
    typedef PVUnionPtr  value_type;
    typedef PVUnionPtr* pointer;
    typedef const PVUnionPtr* const_pointer;
    typedef PVUnionArray &reference;
    typedef const PVUnionArray& const_reference;

    //TODO: full namespace can be removed along with local typedef 'shared_vector'
    typedef ::epics::pvData::shared_vector<PVUnionPtr> svector;
    typedef ::epics::pvData::shared_vector<const PVUnionPtr> const_svector;
    /**
     * Destructor
     */
    virtual ~PVValueArray() {}

    virtual ArrayConstPtr getArray() const
    {
        return std::tr1::static_pointer_cast<const Array>(unionArray);
    }

    virtual size_t getLength() const {return value.size();}
    virtual size_t getCapacity() const {return value.capacity();}

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
    UnionArrayConstPtr getUnionArray() const {return unionArray;}
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

    virtual const_svector view() const { return value; }
    virtual void swap(const_svector &other);
    virtual void replace(const const_svector &other) {
        checkLength(other.size());
        value = other;
        PVField::postPut();
    }

    virtual void serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher) const;
    virtual void deserialize(ByteBuffer *buffer,
        DeserializableControl *pflusher);
    virtual void serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher, std::size_t offset, std::size_t count) const ;

    virtual std::ostream& dumpValue(std::ostream& o) const;
    virtual std::ostream& dumpValue(std::ostream& o, std::size_t index) const;

    void copy(const PVUnionArray& from);
    void copyUnchecked(const PVUnionArray& from);

protected:
    explicit PVValueArray(UnionArrayConstPtr const & unionArray)
        :base_t(unionArray)
        ,unionArray(unionArray)
    {}
private:
    UnionArrayConstPtr unionArray;
    const_svector value;
    friend class PVDataCreate;
};


/**
 * Definitions for the various scalarArray types.
 */
typedef PVValueArray<boolean> PVBooleanArray;
typedef std::tr1::shared_ptr<PVBooleanArray> PVBooleanArrayPtr;

typedef PVValueArray<int8> PVByteArray;
typedef std::tr1::shared_ptr<PVByteArray> PVByteArrayPtr;

typedef PVValueArray<int16> PVShortArray;
typedef std::tr1::shared_ptr<PVShortArray> PVShortArrayPtr;

typedef PVValueArray<int32> PVIntArray;
typedef std::tr1::shared_ptr<PVIntArray> PVIntArrayPtr;

typedef PVValueArray<int64> PVLongArray;
typedef std::tr1::shared_ptr<PVLongArray> PVLongArrayPtr;

typedef PVValueArray<uint8> PVUByteArray;
typedef std::tr1::shared_ptr<PVUByteArray> PVUByteArrayPtr;

typedef PVValueArray<uint16> PVUShortArray;
typedef std::tr1::shared_ptr<PVUShortArray> PVUShortArrayPtr;

typedef PVValueArray<uint32> PVUIntArray;
typedef std::tr1::shared_ptr<PVUIntArray> PVUIntArrayPtr;

typedef PVValueArray<uint64> PVULongArray;
typedef std::tr1::shared_ptr<PVULongArray> PVULongArrayPtr;

typedef PVValueArray<float> PVFloatArray;
typedef std::tr1::shared_ptr<PVFloatArray> PVFloatArrayPtr;

typedef PVValueArray<double> PVDoubleArray;
typedef std::tr1::shared_ptr<PVDoubleArray> PVDoubleArrayPtr;

typedef PVValueArray<std::string> PVStringArray;
typedef std::tr1::shared_ptr<PVStringArray> PVStringArrayPtr;

/**
 * @brief This is a singleton class for creating data instances.
 *
 */
class epicsShareClass PVDataCreate {
public:
    /**
     * get the singleton
     * @return The PVDataCreate implementation
     */
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
     * @param scalarType The scalar type.
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
     * template version
     * @tparam PVT must be a valid PVType
     * @return The PVScalar implementation.
     */
    template<typename PVT>
    std::tr1::shared_ptr<PVT> createPVScalar()
    {
        return std::tr1::static_pointer_cast<PVT>(createPVScalar(PVT::typeCode));   
    }

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

    /**
     * Create implementation for PVUnion.
     * @param punion The introspection interface.
     * @return The PVUnion implementation
     */
    PVUnionPtr createPVUnion(UnionConstPtr const & punion);
     /**
      * Create implementation for PVUnion.
      * @param unionToClone A union. Each subfield is cloned and added to the newly created union.
      * @return The PVUnion implementation.
      */
   PVUnionPtr createPVUnion(PVUnionPtr const & unionToClone);
    /**
     * Create variant union implementation.
     * @return The variant PVUnion implementation. 
     */
    PVUnionPtr createPVVariantUnion();

    /**
     * Create an implementation of an array field reusing the Array introspection interface.
     * @param scalarArray The introspection interface.
     * @return The PVScalarArray implementation.
     */
    PVScalarArrayPtr createPVScalarArray(ScalarArrayConstPtr const & scalarArray);
    /**
     * Create an implementation for an array field. An Array introspection interface is created.
     * @param elementType The element type.
     * @return The PVScalarArray implementation.
     */
    PVScalarArrayPtr createPVScalarArray(ScalarType elementType);
    /**
     * Create an implementation of an array field by cloning an existing PVArray.
     * The new PVArray will have the same value and auxInfo as the original.
     * @param scalarArrayToClone The PVScalarArray to clone.
     * @return The PVScalarArray implementation.
     */
    PVScalarArrayPtr createPVScalarArray(PVScalarArrayPtr const  & scalarArrayToClone);
    /**
     * template version
     * @tparam PVT must be a valid pvType
     * @return The PVScalarArray implementation.
     */
    template<typename PVAT>
    std::tr1::shared_ptr<PVAT> createPVScalarArray()
    {
        return std::tr1::static_pointer_cast<PVAT>(createPVScalarArray(PVAT::typeCode));   
    }

    /**
     * Create an implementation of an array with structure elements.
     * @param structureArray The introspection interface.
     * All elements share the same introspection interface.
     * @return The PVStructureArray implementation.
     */
    PVStructureArrayPtr createPVStructureArray(StructureArrayConstPtr const & structureArray);
    /**
     * Create an implementation of an array with structure elements.
     * @param structure The introspection interface that is used to create StructureArrayConstPtr.
     * All elements share the same introspection interface.
     * @return The PVStructureArray implementation.
     */
    PVStructureArrayPtr createPVStructureArray(StructureConstPtr const & structure)
    {
        return createPVStructureArray(fieldCreate->createStructureArray(structure));
    }

    /**
     * Create an implementation of an array with union elements.
     * @param unionArray The introspection interface.
     * All elements share the same introspection interface.
     * @return The PVUnionArray implementation.
     */
    PVUnionArrayPtr createPVUnionArray(UnionArrayConstPtr const & unionArray);
    /**
     * Create an implementation of an array with union elements.
     * @param punion The introspection interface that is used to create UnionArrayConstPtr.
     * All elements share the same introspection interface.
     * @return The PVUnionArray implementation.
     */
    PVUnionArrayPtr createPVUnionArray(UnionConstPtr const & punion)
    {
        return createPVUnionArray(fieldCreate->createUnionArray(punion));
    }
    /**
     * Create variant union array implementation.
     * @return The variant PVUnionArray implementation. 
     */
    PVUnionArrayPtr createPVVariantUnionArray();
    
private:
   PVDataCreate();
   FieldCreatePtr fieldCreate;
};

/**
 * Get the single class that implements PVDataCreate
 * @return The PVDataCreate factory.
 */
epicsShareExtern PVDataCreatePtr getPVDataCreate();

bool epicsShareExtern operator==(const PVField&, const PVField&);

static inline bool operator!=(const PVField& a, const PVField& b)
{return !(a==b);}

}}

/**
 * stream support for pvField
 */
namespace std{
    epicsShareExtern std::ostream& operator<<(std::ostream& o, const epics::pvData::PVField *ptr);
}

#undef USAGE_DEPRECATED
#undef USAGE_ERROR

#endif  /* PVDATA_H */

/** @page Overview Documentation
 *
 * <a href = "pvDataCPP.html">pvData.html</a>
 *
 */
