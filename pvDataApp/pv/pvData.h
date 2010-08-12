/* pvData.h */
#include <string>
#include <stdexcept>
#ifndef TYPE_H
#define TYPE_H
namespace epics { namespace pvData { 

    enum Type {
        scalar,
        scalarArray,
        structure,
        structureArray
    };

    class TypeFunc {
    public:
        static void toString(std::string &buf,const Type type);
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
        static ScalarType getScalarType(std::string const& value);
        static void toString(std::string &buf,const ScalarType scalarType);
    };

    class Field {
    public:
       virtual ~Field();
       virtual std::string const& getFieldName() const = 0;
       virtual Type getType() const = 0;
       virtual void toString(std::string &buf) const = 0;
       virtual void toString(std::string &buf,int indentLevel) const = 0;
    };

    class Field;
    class Scalar;
    class ScalarArray;
    class Structure;
    class StructureArray;

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

    typedef Field const * FieldPtrConst;
    typedef FieldPtrConst * FieldPtrConstArray;
    typedef std::string const * StringPtrConst;
    typedef StringPtrConst * StringPtrConstArray;

    class Structure : public Field {
    public:
       virtual ~Structure();
       virtual int const getNumberFields() const = 0;
       virtual StringPtrConstArray getFieldNames() const = 0;
       virtual FieldPtrConst getField(std::string const& fieldName) const = 0;
       virtual int getFieldIndex(std::string const& fieldName) const = 0;
       virtual FieldPtrConstArray getFields() const = 0;
    };

    class StructureArray : public Field{
    public:
       virtual ~StructureArray();
       virtual Structure const &  getStructure() const = 0;
    };


    class FieldCreate {
    public:
       Field const &  create(std::string const& fieldName,Field const &  field) const;
       Scalar const &  createScalar(std::string const& fieldName,ScalarType scalarType) const;
       ScalarArray const & createScalarArray(std::string const& fieldName,ScalarType elementType) const;
       Structure const & createStructure (std::string const& fieldName,int numberFields,FieldPtrConstArray fields) const;
       StructureArray const & createStructureArray(std::string const& fieldName,Structure const & structure) const;
    protected:
       FieldCreate();
    };

    extern FieldCreate & getFieldCreate();

}}
#endif  /* TYPE_H */
