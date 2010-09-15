/* pvData.h */
#include <string>
#include <stdexcept>
#ifndef PVINTROSPECT_H
#define PVINTROSPECT_H
namespace epics { namespace pvData { 
    class Field;
    class Scalar;
    class ScalarArray;
    class Structure;
    class StructureArray;
    typedef std::string * StringPtr;
    typedef std::string const * StringConstPtr;  //pointer to constant string
    typedef StringConstPtr * StringConstPtrArray;//array of pointers to constant string
    typedef Field const * FieldConstPtr;         //pointer to constant field
    typedef FieldConstPtr * FieldConstPtrArray;  //array of pointers to const field
    typedef Scalar const * ScalarConstPtr;         //pointer to constant field
    typedef ScalarArray const * ScalarArrayConstPtr;
    typedef Structure const * StructureConstPtr;
    typedef StructureArray const * StructureArrayConstPtr;

    enum Type {
        scalar,
        scalarArray,
        structure,
        structureArray
    };

    class TypeFunc {
    public:
        static void toString(StringPtr buf,const Type type);
    };

    enum ScalarType {
        pvBoolean,
        pvByte,
        pvShort,
        pvInt,
        pvLong,
        pvFloat,
        pvDouble,
        pvString
    };

    class ScalarTypeFunc {
    public:
        static bool isInteger(ScalarType type);
        static bool isNumeric(ScalarType type);
        static bool isPrimitive(ScalarType type);
        static ScalarType getScalarType(StringConstPtr value);
        static void toString(StringPtr buf,const ScalarType scalarType);
    };

    class Field {
    public:
       virtual ~Field();
       virtual StringConstPtr getFieldName() const = 0;
       virtual Type getType() const = 0;
       virtual void toString(StringPtr buf) const = 0;
       virtual void toString(StringPtr buf,int indentLevel) const = 0;
    };


    class Scalar : public Field{
    public:
       virtual ~Scalar();
       virtual ScalarType getScalarType() const = 0;
    };

    class ScalarArray : public Field{
    public:
       virtual ~ScalarArray();
       virtual ScalarType  getElementType() const = 0;
    };


    class Structure : public Field {
    public:
       virtual ~Structure();
       virtual int const getNumberFields() const = 0;
       virtual StringConstPtrArray getFieldNames() const = 0;
       virtual FieldConstPtr getField(StringConstPtr fieldName) const = 0;
       virtual int getFieldIndex(StringConstPtr fieldName) const = 0;
       virtual FieldConstPtrArray getFields() const = 0;
    };

    class StructureArray : public Field{
    public:
       virtual ~StructureArray();
       virtual StructureConstPtr  getStructure() const = 0;
    };


    class FieldCreate {
    public:
       FieldConstPtr  create(StringConstPtr fieldName,FieldConstPtr  field) const;
       ScalarConstPtr  createScalar(StringConstPtr fieldName,ScalarType scalarType) const;
       ScalarArrayConstPtr createScalarArray(StringConstPtr fieldName,
           ScalarType elementType) const;
       StructureConstPtr createStructure (StringConstPtr fieldName,
           int numberFields,FieldConstPtrArray fields) const;
       StructureArrayConstPtr createStructureArray(StringConstPtr fieldName,
           StructureConstPtr structure) const;
    protected:
       FieldCreate();
    };

    extern FieldCreate * getFieldCreate();

}}
#endif  /* PVINTROSPECT_H */
