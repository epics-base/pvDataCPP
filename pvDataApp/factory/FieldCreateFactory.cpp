
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>

#include <boost/smart_ptr.hpp>

#include "pvData.h"

namespace epics { namespace pvData {

    static void newLine(std::string & buffer,int indentLevel) {
        buffer += "\n";
        for(int i=0; i<indentLevel; i++) buffer += "    ";
    }

    Field::~Field(){}

    class BaseField : public Field {
    public:
       BaseField(std::string const& fieldName,Type type);
       ~BaseField();
       virtual std::string const& getFieldName() const;
       virtual Type getType() const;
       virtual void toString(std::string &buf) const;
       virtual void toString(std::string &buf,int indentLevel) const;
    private:
       std::string const fieldName;
       Type type;
    };

    BaseField::~BaseField() {
        delete &fieldName;
    }

    BaseField::BaseField(std::string const& fieldName,Type type)
       :fieldName(fieldName),type(type){}
 
    std::string const& BaseField::getFieldName() const {return fieldName;}
    Type BaseField::getType() const {return type;}
    void BaseField::toString(std::string &buf) const{toString(buf,0);}
    void BaseField::toString(std::string &buffer,int indentLevel) const{
        newLine(buffer,indentLevel);
        buffer += "field ";
        buffer += fieldName.c_str();
        buffer += " type ";
        TypeFunc::toString(buffer,type);
    }

    Scalar::~Scalar(){}

    class BaseScalar: private BaseField,public Scalar {
    public:
        BaseScalar(std::string const& fieldName,ScalarType scalarType);
        ~BaseScalar();
        virtual std::string const& getFieldName() const{
            return BaseField::getFieldName();
        }
        virtual Type getType() const{return BaseField::getType();}
        virtual ScalarType getScalarType() const { return scalarType;}
        virtual void toString(std::string &buf) const {toString(buf,0);}
        virtual void toString(std::string &buf,int indentLevel) const;
    private:
        ScalarType scalarType;
    };

    BaseScalar::BaseScalar(std::string const& fieldName,ScalarType scalarType)
           : BaseField(fieldName,scalar),scalarType(scalarType){}
    BaseScalar::~BaseScalar() {}


    void BaseScalar::toString(std::string &buffer,int indentLevel) const{
        BaseField::toString(buffer,indentLevel);
        buffer +=  " scalarType ";
        ScalarTypeFunc::toString(buffer,scalarType);
    }

    ScalarArray::~ScalarArray(){}

    class BaseScalarArray: private BaseField,public ScalarArray {
    public:
        BaseScalarArray(std::string const& fieldName,ScalarType elementType);
        ~BaseScalarArray();
        virtual std::string const& getFieldName() const{
            return BaseField::getFieldName();
        }
        virtual Type getType() const{return BaseField::getType();}
        virtual ScalarType getElementType() const { return elementType;}
        virtual void toString(std::string &buf) const {toString(buf,0);}
        virtual void toString(std::string &buf,int indentLevel) const;
    private:
        ScalarType elementType;
    };

    BaseScalarArray::BaseScalarArray
           (std::string const& fieldName,ScalarType elementType)
           : BaseField(fieldName,scalar),elementType(elementType){}
    BaseScalarArray::~BaseScalarArray() {}


    void BaseScalarArray::toString(std::string &buffer,int indentLevel) const{
        BaseField::toString(buffer,indentLevel);
        buffer +=  " elementType ";
        ScalarTypeFunc::toString(buffer,elementType);
    }

    Structure::~Structure(){}

    class BaseStructure: private BaseField,public Structure {
    public:
        BaseStructure(std::string const& fieldName,
            int numberFields,FieldPtrConstArray fields);
        ~BaseStructure();
        virtual std::string const & getFieldName() const{
            return BaseField::getFieldName();
         }
        virtual Type getType() const{return BaseField::getType();}
        virtual int const getNumberFields() const {return numberFields;}
        virtual StringPtrConstArray getFieldNames() const { return fieldNames;}
        virtual FieldPtrConst getField(std::string const& fieldName) const;
        virtual int getFieldIndex(std::string const& fieldName) const;
        virtual FieldPtrConstArray getFields() const { return fields;}
        virtual void toString(std::string &buf) const {toString(buf,0);}
        virtual void toString(std::string &buf,int indentLevel) const;
    private:
        int numberFields;
        FieldPtrConstArray  fields;
        StringPtrConst* fieldNames;
    };

    BaseStructure::BaseStructure (std::string const& fieldName,
        int numberFields, FieldPtrConstArray fields)
        : BaseField(fieldName,structure),
          numberFields(numberFields),
          fields(fields),
          fieldNames(new StringPtrConst[numberFields])
    {
        for(int i=0; i<numberFields; i++) {
            fieldNames[i] = &fields[i]->getFieldName();
        }
    }
    BaseStructure::~BaseStructure() {
        for(int i=0; i<numberFields; i++) {
            delete &fieldNames[i];
        }
	delete[] fieldNames;
        delete[] fields;
    }

    FieldPtrConst  BaseStructure::getField(std::string const& fieldName) const {
        for(int i=0; i<numberFields; i++) {
            Field const &field =  *fields[i];
            int result = fieldName.compare(field.getFieldName());
            if(result==0) return &field;
        }
        return 0;
    }

    int BaseStructure::getFieldIndex(std::string const& fieldName) const {
        for(int i=0; i<numberFields; i++) {
            Field const &field =  *fields[i];
            int result = fieldName.compare(field.getFieldName());
            if(result==0) return i;
        }
        return -1;
    }

    void BaseStructure::toString(std::string &buffer,int indentLevel) const{
        BaseField::toString(buffer,indentLevel);
        buffer += " {";
        for(int i=0; i<numberFields; i++) {
            Field const &field =  *fields[i];
            field.toString(buffer,indentLevel+1);
        }
        newLine(buffer,indentLevel);
        buffer +=  "}";
    }

static std::string notImplemented = "not implemented";

  FieldCreate::FieldCreate(){};

   Field const & FieldCreate::create(std::string const& fieldName,
       Field const & field) const
   {
        throw std::invalid_argument(notImplemented);
   }
   Scalar const &  FieldCreate::createScalar(std::string const& fieldName,
       ScalarType scalarType) const
   {
         BaseScalar *baseScalar = new BaseScalar(fieldName,scalarType);
         return *baseScalar;
   }
 
   ScalarArray const & FieldCreate::createScalarArray(
       std::string const& fieldName,ScalarType elementType) const
   {
         BaseScalarArray *baseScalarArray = new BaseScalarArray(fieldName,elementType);
         return *baseScalarArray;
   }
   Structure const & FieldCreate::createStructure (
       std::string const& fieldName,int numberFields,
       FieldPtrConst fields[]) const
   {
         BaseStructure *baseStructure = new BaseStructure(
             fieldName,numberFields,fields);
         return *baseStructure;
   }
   StructureArray const & FieldCreate::createStructureArray(
       std::string const& fieldName,Structure const & structure) const
   {
        throw std::invalid_argument(notImplemented);
   }

   static FieldCreate* instance = 0;

   class FieldCreateExt : public FieldCreate {
   public:
       FieldCreateExt(): FieldCreate(){};
   };

    FieldCreate & getFieldCreate() {
           if(instance==0) instance = new FieldCreateExt();
            return *instance;
    }

}}
