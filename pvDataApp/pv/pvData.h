/* pvData.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 * Author - Marty Kraimer
 */
#ifndef PVDATA_H
#define PVDATA_H
#include <string>
#include <stdexcept>
#include <pv/pvType.h>
#include <pv/pvIntrospect.h>
#include <pv/noDefaultMethods.h>
#include <pv/requester.h>
#include <pv/byteBuffer.h>
#include <pv/serialize.h>
namespace epics { namespace pvData { 

class PVAuxInfo;
class PostHandler;

class PVField;
class PVScalar;

class PVScalarArray;

class PVStructure;
class PVStructureArray;

/**
 * typedef for a pointer to a PVStructure.
 */
typedef PVStructure * PVStructurePtr;
/**
 * typedef for a pointer to a array of pointer to PVStructure.
 */
typedef PVStructurePtr* PVStructurePtrArray;
/**
 * typedef for a pointer to a PVField.
 */
typedef PVField* PVFieldPtr;
/**
 * typedef for a pointer to a array of pointer to PVField.
 */
typedef PVFieldPtr * PVFieldPtrArray;

/**
 * This class provides auxillary information about a PVField.
 * Each item is stored as a PVScalar.
 * A (key,value) is provided for accessing the items where the key is a String.
 */
class PVAuxInfo : private NoDefaultMethods {
public:
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
     * @param scalarType The scalrType for the new item being added/
     * @return The new PVScalar that has been added to the Auxinfo.
     */
    PVScalar * createInfo(String key,ScalarType scalarType);
    /**
     * Get the number of PVScalars in the Auxinfo.
     * @return The number.
     */
    int getNumberInfo();
    /**
     * Get the Auxinfo with the specified key.
     * @return The PVScalar or null if it does not exist.
     */
    PVScalar * getInfo(String key);
    /**
     * Get the Auxinfo with the specified index.
     * @return The PVScalar or null if it does not exist.
     */
    PVScalar * getInfo(int index);
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
    PVField *pvField;
    int lengthInfo;
    int numberInfo;
    PVScalar **pvInfos; // ptr to array of PVscalar *
    friend class PVDataCreate;
};

/**
 * This class is implemented by code that calls setPostHander
 */
class PostHandler {
public:
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
  private NoDefaultMethods
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
     * Register the message requester.
     * At most one requester can be registered.
     * @param prequester The requester.
     */
    virtual void setRequester(Requester *prequester);
    /**
     * Get offset of the PVField field within top level structure.
     * Every field within the PVStructure has a unique offset.
     * The top level structure has an offset of 0.
     * The first field within the structure has offset equal to 1.
     * The other offsets are determined by recursively traversing each structure of the tree.
     * @return The offset.
     */
    int getFieldOffset() ;
    /**
     * Get the next offset. If the field is a scalar or array field then this is just offset + 1.
     * If the field is a structure it is the offset of the next field after this structure.
     * Thus (nextOffset - offset) is always equal to the number of fields within the field.
     * @return The offset.
     */
    int getNextFieldOffset() ;
    /**
     * Get the total number of fields in this field.
     * This is equal to nextFieldOffset - fieldOffset.
     */
    int getNumberFields() ;
    /**
     * Get the PVAuxInfo interface for the PVField.
     * @return The PVAuxInfo interface.
     */
    PVAuxInfo * getPVAuxInfo();
    /**
     * Is the field immutable, i.e. does it not allow changes.
     * @return (false,true) if it (is not, is) immutable.
     */
    bool isImmutable() ;
    /**
     * Set the field to be immutable, i. e. it can no longer be modified.
     * This is permanent, i.e. once done the field can onot be made mutable.
     */
    virtual void setImmutable();
    /**
     * Get the <i>Field</i> that describes the field.
     * @return Field, which is the reflection interface.
     */
    FieldConstPtr getField() ;
    /**
     * Get the parent of this field.
     * @return The parent interface or null if this is PVRecord
     */
    PVStructure * getParent() ;
    /**
     * Rename the field name.
     * @param newName The new name.
     */
    bool renameField(String  newName);
    /**
     * postPut. Called when the field is updated by the implementation.
     */
    void postPut() ;
    /**
     * Set the handler for postPut.
     * At most one handler can be set.
     * @param postHandler The handler.
     */
    void setPostHandler(PostHandler *postHandler);
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
protected:
    PVField(PVStructure *parent,FieldConstPtr field);
    void setParent(PVStructure * parent);
private:
    void message(String fieldName,String message,MessageType messageType);
    class PVFieldPvt *pImpl;
    static void computeOffset(PVField *pvField);
    static void computeOffset(PVField *pvField,int offset);
    friend class PVDataCreate;
    friend class PVStructure;
};

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
    /**
     * Get the Scalar introspection interface for the PVScalar.
     * @return the interface.
     */
    ScalarConstPtr getScalar() ;
protected:
    PVScalar(PVStructure *parent,ScalarConstPtr scalar);
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
    /**
     * Destructor
     */
    virtual ~PVScalarValue() {}
    /**
     * Get the value.
     * @return The value.
     */
    virtual T get() = 0;
    /**
     * Put a new value into the PVScalar.
     * @param The value.
     */
    virtual void put(T value) = 0;
protected:
    PVScalarValue(PVStructure *parent,ScalarConstPtr scalar)
    : PVScalar(parent,scalar) {}
private:
};

/**
 * typedefs for the various possible scalar types.
 */
typedef PVScalarValue<bool> PVBoolean;
typedef PVScalarValue<int8> PVByte;
typedef PVScalarValue<int16> PVShort;
typedef PVScalarValue<int32> PVInt;
typedef PVScalarValue<int64> PVLong;
typedef PVScalarValue<float> PVFloat;
typedef PVScalarValue<double> PVDouble;

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
    PVString(PVStructure *parent,ScalarConstPtr scalar)
    : PVScalarValue<String>(parent,scalar) {}
};


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
     * Get the array length.
     * @return The length.
     */
    int getLength() const;
    /**
     * Set the array length.
     * @param The length.
     */
    void setLength(int length);
    /**
     * Get the array capacity.
     * @return The capacity.
     */
    int getCapacity() const;
    /**
     * Can the capacity be changed.
     * @return (false,true) if (can not, can) be changed.
     */
    bool isCapacityMutable();
    /**
     * Set the mutability of the array capacity.
     * @return false or true
     */
    void setCapacityMutable(bool isMutable);
    /**
     * Set the array capacity.
     * @param The capacity.
     */
    virtual void setCapacity(int capacity) = 0;
protected:
    PVArray(PVStructure *parent,FieldConstPtr field);
    void setCapacityLength(int capacity,int length);
private:
    class PVArrayPvt * pImpl;
};

/**
 * Class provided by caller of get
 */
template<typename T>
class PVArrayData {
public:
    POINTER_DEFINITIONS(PVArrayData);
    typedef T  value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    /**
     * The data array.
     */
    pointer data;
    /**
     * The offset. This is the offset into the actual array of the first element in data,
     */
    int offset;
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
    /**
     * Get the introspection interface
     * @return The interface.
     */
    ScalarArrayConstPtr getScalarArray() ;

protected:
    PVScalarArray(PVStructure *parent,ScalarArrayConstPtr scalarArray);
private:
};

/**
 * This is provided by code that calls get.
 */
typedef PVArrayData<PVStructurePtr> StructureArrayData;

/**
 * Data class for a structureArray
 */
class PVStructureArray : public PVArray {
public:
    POINTER_DEFINITIONS(PVStructureArray);
    /**
     * Destructor
     */
    virtual ~PVStructureArray() {}
    /**
     * Get the introspection interface
     * @return The interface.
     */
    virtual StructureArrayConstPtr getStructureArray() = 0;
    /**
     * Append new elements to the end of the array.
     * @param number The number of elements to add.
     * @return the new length of the array.
     */
    virtual int append(int number) = 0;
    /**
     * Remove elements from the array.
     * @param offset The offset of the first element to remove.
     * @param number The number of elements to remove.
     * @return (false,true) if the elements were removed.
     */
    virtual bool remove(int offset,int number) = 0;
    /**
     * Compress. This removes all null elements from the array.
     */
    virtual void compress() = 0;
    /**
     * Get array elements
     * @param offset The offset of the first element,
     * @param length The number of elements to get.
     * @param data The place where the data is placed.
     */
    virtual int get(int offset, int length,
        StructureArrayData *data) = 0;
    /**
     * Put data into the array.
     * @param offset The offset of the first element,
     * @param length The number of elements to get.
     * @param from The new values to put into the array.
     * @param fromOffset The offset in from.
     * @return The number of elements put into the array.
     */
    virtual int put(int offset,int length,
        PVStructurePtrArray from, int fromOffset) = 0;
    /**
     * Share data from another source.
     * @param value The data to share.
     * @param capacity The capacity of the array.
     * @param length The length of the array.
     */
    virtual void shareData( PVStructurePtrArray value,int capacity,int length) = 0;

protected:
    PVStructureArray(PVStructure *parent, StructureArrayConstPtr structureArray)
    : PVArray(parent,structureArray) {}
private:
};


class PVStructure : public PVField,public BitSetSerializable {
public:
    POINTER_DEFINITIONS(PVStructure);
    /**
     * Destructor
     */
    virtual ~PVStructure();
    /**
     * Get the introspection interface
     * @return The interface.
     */
    StructureConstPtr getStructure();
    /**
     * Get the array of pointers to the subfields in the structure.
     * @return The array.
     */
    PVFieldPtrArray getPVFields();
    /**
     * Get the subfield with the specified name.
     * @param fieldName The name of the field.
     * @return Pointer to the field or null if field does not exist.
     */
    PVField *getSubField(String fieldName);
    /**
     * Get the subfield with the specified offset.
     * @param fieldOffset The offset.
     * @return Pointer to the field or null if field does not exist.
     */
    PVField *getSubField(int fieldOffset);
    /**
     * Append a field to the structure.
     * @param pvField The field to append.
     */
    void appendPVField(PVField *pvField);
    /**
     * Append fields to the structure.
     * @param numberFields The number of fields.
     * @param pvFields The fields to append.
     * @return Pointer to the field or null if field does not exist.
     */
    void appendPVFields(int numberFields,PVFieldPtrArray pvFields);
    /**
     * Remove a field from the structure.
     * @param fieldName The name of the field to remove.
     */
    void removePVField(String fieldName);
    /**
     * Get a boolean field with the specified name.
     * @param fieldName The name of the field to get.
     * @return Pointer to the field of null if a field with that name and type does not exist.
     */
    PVBoolean *getBooleanField(String fieldName);
    /**
     * Get a byte field with the specified name.
     * @param fieldName The name of the field to get.
     * @return Pointer to the field of null if a field with that name and type does not exist.
     */
    PVByte *getByteField(String fieldName);
    /**
     * Get a short field with the specified name.
     * @param fieldName The name of the field to get.
     * @return Pointer to the field of null if a field with that name and type does not exist.
     */
    PVShort *getShortField(String fieldName);
    /**
     * Get a int field with the specified name.
     * @param fieldName The name of the field to get.
     * @return Pointer to the field of null if a field with that name and type does not exist.
     */
    PVInt *getIntField(String fieldName);
    /**
     * Get a long field with the specified name.
     * @param fieldName The name of the field to get.
     * @return Pointer to the field of null if a field with that name and type does not exist.
     */
    PVLong *getLongField(String fieldName);
    /**
     * Get a float field with the specified name.
     * @param fieldName The name of the field to get.
     * @return Pointer to the field of null if a field with that name and type does not exist.
     */
    PVFloat *getFloatField(String fieldName);
    /**
     * Get a double field with the specified name.
     * @param fieldName The name of the field to get.
     * @return Pointer to the field of null if a field with that name and type does not exist.
     */
    PVDouble *getDoubleField(String fieldName);
    /**
     * Get a string field with the specified name.
     * @param fieldName The name of the field to get.
     * @return Pointer to the field of null if a field with that name and type does not exist.
     */
    PVString *getStringField(String fieldName);
    /**
     * Get a structure field with the specified name.
     * @param fieldName The name of the field to get.
     * @return Pointer to the field of null if a field with that name and type does not exist.
     */
    PVStructure *getStructureField(String fieldName);
    /**
     * Get a scalarArray field with the specified name.
     * @param fieldName The name of the field to get.
     * @param elementType The element type.
     * @return Pointer to the field of null if a field with that name and type does not exist.
     */
    PVScalarArray *getScalarArrayField(
        String fieldName,ScalarType elementType);
    /**
     * Get a structureArray field with the specified name.
     * @param fieldName The name of the field to get.
     * @return Pointer to the field of null if a field with that name and type does not exist.
     */
    PVStructureArray *getStructureArrayField(String fieldName);
    /**
     * Get the name if this structure extends another structure.
     * @return The string which may be null.
     */
    String getExtendsStructureName();
    /**
     * Put the extends name.
     * @param extendsStructureName The name.
     */
    bool putExtendsStructureName(
        String extendsStructureName);
    /**
     * Serialize.
     * @param pbuffer The byte buffer.
     * @param pflusher Interface to call when buffer is full.
     */
    virtual void serialize(
        ByteBuffer *pbuffer,SerializableControl *pflusher) const;
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
     * @param parent The parent structure.
     * @param structure The introspection interface.
     */
    PVStructure(PVStructure *parent,StructureConstPtr structure);
    /**
     * Constructor
     * @param parent The parent structure.
     * @param structure The introspection interface.
     * @param pvFields The array of fields for the structure.
     */
    PVStructure(
        PVStructure *parent,
        StructureConstPtr structure,
        PVFieldPtrArray pvFields);
private:
    void setParentPvt(PVField *pvField,PVStructure *parent);
    class PVStructurePvt * pImpl;
};

template<typename T>
class PVValueArray : public PVScalarArray {
public:
    POINTER_DEFINITIONS(PVValueArray);
    typedef T  value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef PVArrayData<T> ArrayDataType;

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
    virtual int get(int offset, int length, ArrayDataType *data) = 0;
    /**
     * Put data into the array.
     * @param offset The offset of the first element,
     * @param length The number of elements to get.
     * @param from The new values to put into the array.
     * @param fromOffset The offset in from.
     * @return The number of elements put into the array.
     */
    virtual int put(int offset,int length, pointer from, int fromOffset) = 0;
    /**
     * Share data from another source.
     * @param value The data to share.
     * @param capacity The capacity of the array.
     * @param length The length of the array.
     */
    virtual void shareData(pointer value,int capacity,int length) = 0;
protected:
    PVValueArray(PVStructure *parent,ScalarArrayConstPtr scalar)
    : PVScalarArray(parent,scalar) {}
private:
};

/**
 * Definitions for the various scalarArray types.
 */
typedef PVArrayData<bool> BooleanArrayData;
typedef PVValueArray<bool> PVBooleanArray;

typedef PVArrayData<int8> ByteArrayData;
typedef PVValueArray<int8> PVByteArray;

typedef PVArrayData<int16> ShortArrayData;
typedef PVValueArray<int16> PVShortArray;

typedef PVArrayData<int32> IntArrayData;
typedef PVValueArray<int32> PVIntArray;

typedef PVArrayData<int64> LongArrayData;
typedef PVValueArray<int64> PVLongArray;


typedef PVArrayData<float> FloatArrayData;
typedef PVValueArray<float> PVFloatArray;

typedef PVArrayData<double> DoubleArrayData;
typedef PVValueArray<double> PVDoubleArray;

typedef PVArrayData<String> StringArrayData;
typedef PVValueArray<String> PVStringArray;

/**
 * This is a singlton class for creating data instances.
 */
class PVDataCreate {
public:
    /**
     * Create a PVField using given Field introspection data.
     * @param parent The parent interface.
     * @param field The introspection data to be used to create PVField.
     * @return The PVField implementation.
     */
    PVField *createPVField(PVStructure *parent,
        FieldConstPtr field);
    /**
     * Create a PVField using given a PVField to clone.
     * This method calls the appropriate createPVScalar, createPVArray, or createPVStructure.
     * @param parent The parent interface.
     * @param fieldToClone The field to clone.
     * @return The PVField implementation
     */
    PVField *createPVField(PVStructure *parent,
        String fieldName,PVField * fieldToClone);
    /**
     * Create an implementation of a scalar field reusing the Scalar introspection interface.
     * @param parent The parent.
     * @param scalar The introspection interface.
     * @return The PVScalar implementation.
     */
    PVScalar *createPVScalar(PVStructure *parent,ScalarConstPtr scalar);
    /**
     * Create an implementation of a scalar field. A Scalar introspection interface is created.
     * @param parent The parent interface.
     * @param fieldName The field name.
     * @param fieldType The field type.
     * @return The PVScalar implementation.
     */
    PVScalar *createPVScalar(PVStructure *parent,
        String fieldName,ScalarType scalarType);
    /**
     * Create an implementation of a scalar field by cloning an existing PVScalar.
     * The new PVScalar will have the same value and auxInfo as the original.
     * @param parent The parent interface.
     * @param fieldName The field name.
     * @param scalarToClone The PVScalar to clone.
     * @return The PVScalar implementation.
     */
    PVScalar *createPVScalar(PVStructure *parent,
        String fieldName,PVScalar * scalarToClone);
    /**
     * Create an implementation of an array field reusing the Array introspection interface.
     * @param parent The parent interface.
     * @param array The introspection interface.
     * @return The PVScalarArray implementation.
     */
    PVScalarArray *createPVScalarArray(PVStructure *parent,
        ScalarArrayConstPtr scalarArray);
    /**
     * Create an implementation for an array field. An Array introspection interface is created.
     * @param parent The parent interface.
     * @param fieldName The field name.
     * @param elementType The element type.
     * @return The PVScalarArray implementation.
     */
    PVScalarArray *createPVScalarArray(PVStructure *parent,
        String fieldName,ScalarType elementType);
    /**
     * Create an implementation of an array field by cloning an existing PVArray.
     * The new PVArray will have the same value and auxInfo as the original.
     * @param parent The parent interface.
     * @param fieldName The field name.
     * @param arrayToClone The PVScalarArray to clone.
     * @return The PVScalarArray implementation.
     */
    PVScalarArray *createPVScalarArray(PVStructure *parent,
        String fieldName,PVScalarArray * scalarArrayToClone);
    /**
     * Create an implementation of an array with structure elements.
     * @param parent The parent interface.
     * @param structureArray The introspection interface.
     * All elements share the same introspection interface.
     * @return The PVStructureArray implementation.
     */
    PVStructureArray *createPVStructureArray(PVStructure *parent,
        StructureArrayConstPtr structureArray);
    /**
     * Create implementation for PVStructure.
     * @param parent The parent interface.
     * @param structure The introspection interface.
     * @return The PVStructure implementation
     */
    PVStructure *createPVStructure(PVStructure *parent,
        StructureConstPtr structure);
    /**
     * Create implementation for PVStructure.
     * @param parent The parent interface.
     * @param fieldName The field name.
     * @param fields Array of reflection interfaces for the subFields.
     * @return The PVStructure implementation
     */
    PVStructure *createPVStructure(PVStructure *parent,
        String fieldName,int numberFields,FieldConstPtrArray fields);
    /**
     * Create implementation for PVStructure.
     * @param parent The parent interface.
     * @param fieldName The field name.
     * @param pvFields Array of PVFields
     * @return The PVStructure implementation
     */
    PVStructure *createPVStructure(PVStructure *parent,
        String fieldName,int numberFields,PVFieldPtrArray pvFields);
     /**
      * Create implementation for PVStructure.
      * @param parent The parent interface.
      * @param fieldName The field name.
      * @param structToClone A structure. Each subfield and any auxInfo is cloned and added to the newly created structure.
      * @return The PVStructure implementation.
      */
   PVStructure *createPVStructure(PVStructure *parent,
       String fieldName,PVStructure *structToClone);
protected:
   PVDataCreate();
   friend PVDataCreate * getPVDataCreate();
};

/**
 * Get the single class that implemnents PVDataCreate
 * @param The PVDataCreate factory.
 */

extern PVDataCreate * getPVDataCreate();
    
}}
#endif  /* PVDATA_H */
