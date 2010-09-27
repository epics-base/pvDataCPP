/* pvIntrospect.h */
#include <string>
#include <stdexcept>
#ifndef PVINTROSPECT_H
#define PVINTROSPECT_H
#include "noDefaultMethods.h"
namespace epics { namespace pvData { 
    class Field;
    class Scalar;
    class ScalarArray;
    class Structure;
    class StructureArray;

    typedef std::string String;
    typedef std::string * StringBuilder;
    typedef std::string const StringConst;
    typedef StringConst * StringConstArray;

    typedef Field const * FieldConstPtr;
    typedef FieldConstPtr * FieldConstPtrArray;
    typedef Scalar const * ScalarConstPtr;
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
        static void toString(StringBuilder buf,const Type type);
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
        static ScalarType getScalarType(StringConst value);
        static void toString(StringBuilder buf,const ScalarType scalarType);
    };

    class Field : private NoDefaultMethods {
    public:
       virtual ~Field();
       virtual int getReferenceCount() const = 0;
       virtual StringConst getFieldName() const = 0;
       virtual Type getType() const = 0;
       virtual void toString(StringBuilder buf) const = 0;
       virtual void toString(StringBuilder buf,int indentLevel) const = 0;
    private:
       virtual void incReferenceCount() const = 0;
       virtual void decReferenceCount() const = 0;
       friend class BaseStructure;
       friend class BaseStructureArray;
       friend class PVFieldPvt;
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
       virtual FieldConstPtr getField(StringConst fieldName) const = 0;
       virtual int getFieldIndex(StringConst fieldName) const = 0;
       virtual FieldConstPtrArray getFields() const = 0;
    };

    class StructureArray : public Field{
    public:
       virtual ~StructureArray();
       virtual StructureConstPtr  getStructure() const = 0;
    };


    class FieldCreate {
    public:
       FieldConstPtr  create(StringConst fieldName,FieldConstPtr  field) const;
       ScalarConstPtr  createScalar(StringConst fieldName,ScalarType scalarType) const;
       ScalarArrayConstPtr createScalarArray(StringConst fieldName,
           ScalarType elementType) const;
       StructureConstPtr createStructure (StringConst fieldName,
           int numberFields,FieldConstPtrArray fields) const;
       StructureArrayConstPtr createStructureArray(StringConst fieldName,
           StructureConstPtr structure) const;
    protected:
       FieldCreate();
    };

    extern FieldCreate * getFieldCreate();

}}
#endif  /* PVINTROSPECT_H */
