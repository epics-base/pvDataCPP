
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>

#include <boost/smart_ptr.hpp>

#include "pvData.h"

namespace epics { namespace pvData {

    void TypeFunc::toString(std::string &buf,const Type type) {
        static const std::string unknownString = "logic error unknown Type";
        switch(type) {
        case scalar : buf += "scalar"; break;
        case scalarArray : buf += "scalarArray"; break;
        case structure : buf += "structure"; break;
        case structureArray : buf += "structureArray"; break;
        default:
        throw std::invalid_argument(unknownString);
        }
    }


    bool ScalarTypeFunc::isInteger(ScalarType type) {
        if(type>=pvByte && type<=pvLong) return true;
        return false;
    }

    bool ScalarTypeFunc::isNumeric(ScalarType type) {
        if(type>=pvByte && type<=pvDouble) return true;
        return false;
    }

    bool ScalarTypeFunc::isPrimitive(ScalarType type) {
        if(type>=pvBoolean && type<=pvDouble) return true;
        return false;
    }

    ScalarType ScalarTypeFunc::getScalarType(std::string const& pvalue) {
        static const std::string unknownString = "error unknown ScalarType";
        if(pvalue.compare("boolean")==0) return pvBoolean;
        if(pvalue.compare("byte")==0) return pvByte;
        if(pvalue.compare("short")==0) return pvShort;
        if(pvalue.compare("int")==0) return pvInt;
        if(pvalue.compare("long")==0) return pvLong;
        if(pvalue.compare("float")==0) return pvFloat;
        if(pvalue.compare("double")==0) return pvDouble;
        if(pvalue.compare("string")==0) return pvString;
        throw std::invalid_argument(unknownString);
    }
    void ScalarTypeFunc::toString(std::string &buf,const ScalarType scalarType) {
        static const std::string unknownString = "logic error unknown ScalarType";
        switch(scalarType) {
        case pvBoolean : buf += "pvBoolean"; return;
        case pvByte : buf += "pvByte"; return;;
        case pvShort : buf += "pvShort"; return;
        case pvInt : buf += "pvInt"; return;
        case pvLong : buf += "pvLong"; return;
        case pvFloat : buf += "pvFloat"; return;
        case pvDouble : buf += "pvDouble"; return;
        case pvString : buf += "pvString"; return;
        }
        throw std::invalid_argument(unknownString);
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
        delete(&fieldName);
    }

    BaseField::BaseField(std::string const& fieldName,Type type)
       :fieldName(fieldName),type(type){}
 
    std::string const& BaseField::getFieldName() const {return fieldName;}
    Type BaseField::getType() const {return type;}
    void BaseField::toString(std::string &buf) const{toString(buf,0);}
    void BaseField::toString(std::string &buffer,int indentLevel) const{
        for(int i=0; i<indentLevel; i++) buffer += "    ";
        buffer += "field ";
        buffer += fieldName.c_str();
        buffer += " type ";
        TypeFunc::toString(buffer,type);
    }

   Scalar::~Scalar(){}

    class BaseScalar: public BaseField,public Scalar {
    public:
        BaseScalar(std::string const& fieldName,ScalarType scalarType);
        ~BaseScalar();
// WHY DO I HAVE TO DESCRIBE AND DEFINE THESE??
virtual std::string const& getFieldName() const;
virtual Type getType() const;
       virtual ScalarType getScalarType() const { return scalarType;}
       virtual void toString(std::string &buf) const;
       virtual void toString(std::string &buf,int indentLevel) const;
    private:
        ScalarType scalarType;
    };

    BaseScalar::BaseScalar(std::string const& fieldName,ScalarType scalarType)
           : BaseField(fieldName,scalar),scalarType(scalarType){}
    BaseScalar::~BaseScalar() {}


std::string const& BaseScalar::getFieldName() const
{
    return BaseField::getFieldName();
}
Type BaseScalar::getType() const
{
   return BaseField::getType();
}
    void BaseScalar::toString(std::string &buf) const{toString(buf,0);}
    void BaseScalar::toString(std::string &buffer,int indentLevel) const{
        BaseField::toString(buffer,indentLevel);
        buffer +=  " scalarType ";
        ScalarTypeFunc::toString(buffer,scalarType);
    }

static std::string notImplemented = "not implemented";

  FieldCreate::FieldCreate(){};

   Field const & FieldCreate::create(std::string const& fieldName, Field const & field) const
   {
        throw std::invalid_argument(notImplemented);
   }
   Scalar const &  FieldCreate::createScalar(std::string const& fieldName,ScalarType scalarType) const
   {
         BaseScalar *baseScalar = new BaseScalar(fieldName,scalarType);
         return *baseScalar;
   }
 
   ScalarArray const & FieldCreate::createScalarArray(std::string const& fieldName,ScalarType elementType) const
   {
        throw std::invalid_argument(notImplemented);
   }
   Structure const & FieldCreate::createStructure (std::string const& fieldName,Field const * const fields) const
   {
        throw std::invalid_argument(notImplemented);
   }
   StructureArray const & FieldCreate::createStructureArray(std::string const& fieldName,Structure const & structure) const
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
