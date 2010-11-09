/* pvIntrospect.h */
#include <string>
#include <stdexcept>
#ifndef PVINTROSPECT_H
#define PVINTROSPECT_H
#include "noDefaultMethods.h"
#include "pvType.h"
namespace epics { namespace pvData { 

    class Field;
    class Scalar;
    class ScalarArray;
    class Structure;
    class StructureArray;

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
        static ScalarType getScalarType(String value);
        static void toString(StringBuilder buf,ScalarType scalarType);
    };

    class Field : private NoDefaultMethods {
    public:
       virtual ~Field();
       Field(String fieldName,Type type);
       static int getTotalReferenceCount();
       static int64 getTotalConstruct();
       static int64 getTotalDestruct();
       int getReferenceCount() const;
       String getFieldName() const;
       Type getType() const;
       virtual void toString(StringBuilder buf) const{toString(buf,0);}
       virtual void toString(StringBuilder buf,int indentLevel) const;
    private:
       class FieldPvt *pImpl;
       void incReferenceCount() const;
       void decReferenceCount() const;
       friend class StructureArray;
       friend class Structure;
       friend class PVFieldPvt;
       friend class StandardField;
       friend class BasePVStructureArray;
    };


    class Scalar : public Field{
    public:
       Scalar(String fieldName,ScalarType scalarType);
       virtual ~Scalar();
       ScalarType getScalarType() const {return scalarType;}
       virtual void toString(StringBuilder buf) const{toString(buf,0);}
       virtual void toString(StringBuilder buf,int indentLevel) const;
    private:
       ScalarType scalarType;
    };

    class ScalarArray : public Field{
    public:
       ScalarArray(String fieldName,ScalarType scalarType);
       virtual ~ScalarArray();
       ScalarType  getElementType() const {return elementType;}
       virtual void toString(StringBuilder buf) const{toString(buf,0);}
       virtual void toString(StringBuilder buf,int indentLevel) const;
    private:
       ScalarType elementType;
    };

    class StructureArray : public Field{
    public:
       StructureArray(String fieldName,StructureConstPtr structure);
       virtual ~StructureArray();
       StructureConstPtr  getStructure() const {return pstructure;}
       virtual void toString(StringBuilder buf) const{toString(buf,0);}
       virtual void toString(StringBuilder buf,int indentLevel) const;
    private:
        StructureConstPtr pstructure;
    };

    class Structure : public Field {
    public:
       Structure(String fieldName, int numberFields,FieldConstPtrArray fields);
       virtual ~Structure();
       int getNumberFields() const {return numberFields;}
       FieldConstPtr getField(String fieldName) const;
       int getFieldIndex(String fieldName) const;
       FieldConstPtrArray getFields() const {return fields;}
       virtual void toString(StringBuilder buf) const{toString(buf,0);}
       virtual void toString(StringBuilder buf,int indentLevel) const;
    private:
        int numberFields;
        FieldConstPtrArray  fields;
    };

    class FieldCreate {
    public:
       FieldConstPtr  create(String fieldName,FieldConstPtr  field) const;
       ScalarConstPtr  createScalar(String fieldName,ScalarType scalarType) const;
       ScalarArrayConstPtr createScalarArray(String fieldName,
           ScalarType elementType) const;
       StructureConstPtr createStructure (String fieldName,
           int numberFields,FieldConstPtrArray fields) const;
       StructureArrayConstPtr createStructureArray(String fieldName,
           StructureConstPtr structure) const;
    private:
       FieldCreate();
       friend FieldCreate * getFieldCreate();
    };

    extern FieldCreate * getFieldCreate();

}}
#endif  /* PVINTROSPECT_H */
