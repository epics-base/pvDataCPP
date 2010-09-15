
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <boost/smart_ptr.hpp>
#include "pvData.h"

namespace epics { namespace pvData {

    static void newLine(StringPtr buffer,int indentLevel) {
        *buffer += "\n";
        for(int i=0; i<indentLevel; i++) *buffer += "    ";
    }

    Field::~Field(){}

    class BaseField : public Field {
    public:
       BaseField(StringConstPtr fieldName,Type type);
       virtual ~BaseField();
       virtual StringConstPtr getFieldName() const;
       virtual Type getType() const;
       virtual void toString(StringPtr buf) const;
       virtual void toString(StringPtr buf,int indentLevel) const;
    private:
       std::string const fieldName;
       Type type;
    };

    BaseField::~BaseField() {
        delete &fieldName;
    }

    BaseField::BaseField(StringConstPtr fieldName,Type type)
       :fieldName(*fieldName),type(type){}
 
    StringConstPtr BaseField::getFieldName() const {return &fieldName;}
    Type BaseField::getType() const {return type;}
    void BaseField::toString(StringPtr buf) const{toString(buf,0);}
    void BaseField::toString(StringPtr buffer,int indentLevel) const{
        newLine(buffer,indentLevel);
        *buffer += "field ";
        *buffer += fieldName.c_str();
        *buffer += " type ";
        TypeFunc::toString(buffer,type);
    }

    Scalar::~Scalar(){}

    class BaseScalar: private BaseField,public Scalar {
    public:
        BaseScalar(StringConstPtr fieldName,ScalarType scalarType);
        virtual ~BaseScalar();
        virtual StringConstPtr getFieldName() const{
            return BaseField::getFieldName();
        }
        virtual Type getType() const{return BaseField::getType();}
        virtual ScalarType getScalarType() const { return scalarType;}
        virtual void toString(StringPtr buf) const {toString(buf,0);}
        virtual void toString(StringPtr buf,int indentLevel) const;
    private:
        ScalarType scalarType;
    };

    BaseScalar::BaseScalar(StringConstPtr fieldName,ScalarType scalarType)
           : BaseField(fieldName,scalar),scalarType(scalarType){}
    BaseScalar::~BaseScalar() {}


    void BaseScalar::toString(StringPtr buffer,int indentLevel) const{
        BaseField::toString(buffer,indentLevel);
        *buffer +=  " scalarType ";
        ScalarTypeFunc::toString(buffer,scalarType);
    }

    ScalarArray::~ScalarArray(){}

    class BaseScalarArray: private BaseField,public ScalarArray {
    public:
        BaseScalarArray(StringConstPtr fieldName,ScalarType elementType);
        virtual ~BaseScalarArray();
        virtual StringConstPtr getFieldName() const{
            return BaseField::getFieldName();
        }
        virtual Type getType() const{return BaseField::getType();}
        virtual ScalarType getElementType() const { return elementType;}
        virtual void toString(StringPtr buf) const {toString(buf,0);}
        virtual void toString(StringPtr buf,int indentLevel) const;
    private:
        ScalarType elementType;
    };

    BaseScalarArray::BaseScalarArray
           (StringConstPtr fieldName,ScalarType elementType)
           : BaseField(fieldName,scalar),elementType(elementType){}
    BaseScalarArray::~BaseScalarArray() {}


    void BaseScalarArray::toString(StringPtr buffer,int indentLevel) const{
        BaseField::toString(buffer,indentLevel);
        *buffer +=  " elementType ";
        ScalarTypeFunc::toString(buffer,elementType);
    }

    Structure::~Structure(){}

    class BaseStructure: private BaseField,public Structure {
    public:
        BaseStructure(StringConstPtr fieldName,
            int numberFields,FieldConstPtrArray fields);
        virtual ~BaseStructure();
        virtual StringConstPtr getFieldName() const{
            return BaseField::getFieldName();
         }
        virtual Type getType() const{return BaseField::getType();}
        virtual int const getNumberFields() const {return numberFields;}
        virtual StringConstPtrArray getFieldNames() const { return fieldNames;}
        virtual FieldConstPtr getField(StringConstPtr fieldName) const;
        virtual int getFieldIndex(StringConstPtr fieldName) const;
        virtual FieldConstPtrArray getFields() const { return fields;}
        virtual void toString(StringPtr buf) const {toString(buf,0);}
        virtual void toString(StringPtr buf,int indentLevel) const;
    private:
        int numberFields;
        FieldConstPtrArray  fields;
        StringConstPtr* fieldNames;
    };

    BaseStructure::BaseStructure (StringConstPtr fieldName,
        int numberFields, FieldConstPtrArray fields)
        : BaseField(fieldName,structure),
          numberFields(numberFields),
          fields(fields),
          fieldNames(new StringConstPtr[numberFields])
    {
        for(int i=0; i<numberFields; i++) {
            fieldNames[i] = fields[i]->getFieldName();
        }
    }
    BaseStructure::~BaseStructure() {
        for(int i=0; i<numberFields; i++) {
            delete fieldNames[i];
        }
	delete[] fieldNames;
        delete[] fields;
    }

    FieldConstPtr  BaseStructure::getField(StringConstPtr fieldName) const {
        for(int i=0; i<numberFields; i++) {
            FieldConstPtr pfield = fields[i];
            int result = fieldName->compare(*pfield->getFieldName());
            if(result==0) return pfield;
        }
        return 0;
    }

    int BaseStructure::getFieldIndex(StringConstPtr fieldName) const {
        for(int i=0; i<numberFields; i++) {
            FieldConstPtr pfield = fields[i];
            int result = fieldName->compare(*pfield->getFieldName());
            if(result==0) return i;
        }
        return -1;
    }

    void BaseStructure::toString(StringPtr buffer,int indentLevel) const{
        BaseField::toString(buffer,indentLevel);
        *buffer += " {";
        for(int i=0; i<numberFields; i++) {
            FieldConstPtr pfield = fields[i];
            pfield->toString(buffer,indentLevel+1);
        }
        newLine(buffer,indentLevel);
        *buffer +=  "}";
    }

    StructureArray::~StructureArray(){}

    class BaseStructureArray: private BaseField,public StructureArray {
    public:
        BaseStructureArray(StringConstPtr fieldName,StructureConstPtr structure);
        virtual ~BaseStructureArray();
        virtual StringConstPtr getFieldName() const{
            return BaseField::getFieldName();
         }
        virtual Type getType() const{return BaseField::getType();}
        virtual StructureConstPtr  getStructure() const { return pstructure; }
        virtual void toString(StringPtr buf) const {toString(buf,0);}
        virtual void toString(StringPtr buf,int indentLevel) const;
    private:
        StructureConstPtr pstructure;
    };

    BaseStructureArray::BaseStructureArray(StringConstPtr fieldName,StructureConstPtr structure)
    : BaseField(fieldName,structureArray),pstructure(structure) {}

    BaseStructureArray::~BaseStructureArray() {}


    void BaseStructureArray::toString(StringPtr buffer,int indentLevel) const {
        BaseField::toString(buffer,indentLevel);
        *buffer +=  " structure ";
        pstructure->toString(buffer,indentLevel + 1);
    }

static std::string notImplemented = "not implemented";
static std::string logicError = "Logic Error. Should never get here";

  FieldCreate::FieldCreate(){};

   ScalarConstPtr  FieldCreate::createScalar(StringConstPtr fieldName,
       ScalarType scalarType) const
   {
         BaseScalar *baseScalar = new BaseScalar(fieldName,scalarType);
         return baseScalar;
   }
 
   ScalarArrayConstPtr FieldCreate::createScalarArray(
       StringConstPtr fieldName,ScalarType elementType) const
   {
         BaseScalarArray *baseScalarArray = new BaseScalarArray(fieldName,elementType);
         return baseScalarArray;
   }
   StructureConstPtr FieldCreate::createStructure (
       StringConstPtr fieldName,int numberFields,
       FieldConstPtr fields[]) const
   {
         BaseStructure *baseStructure = new BaseStructure(
             fieldName,numberFields,fields);
         return baseStructure;
   }
   StructureArrayConstPtr FieldCreate::createStructureArray(
       StringConstPtr fieldName,StructureConstPtr structure) const
   {
        throw std::invalid_argument(notImplemented);
   }

   FieldConstPtr FieldCreate::create(StringConstPtr fieldName,
       FieldConstPtr pfield) const
   {
       Type type = pfield->getType();
       switch(type) {
       case scalar: {
           ScalarConstPtr pscalar = dynamic_cast<ScalarConstPtr>(pfield);
           return createScalar(fieldName,pscalar->getScalarType());
       }
       case scalarArray: {
           ScalarArrayConstPtr pscalarArray = dynamic_cast<ScalarArrayConstPtr>(pfield);
           return createScalarArray(fieldName,pscalarArray->getElementType());
       }
       case structure: {
           StructureConstPtr pstructure = dynamic_cast<StructureConstPtr>(pfield);
           return createStructure(fieldName,pstructure->getNumberFields(),pstructure->getFields());
       }
       case structureArray: {
           StructureArrayConstPtr pstructureArray = dynamic_cast<StructureArrayConstPtr>(pfield);
           return createStructureArray(fieldName,pstructureArray->getStructure());
       }
       }
       throw std::logic_error(logicError);
   }

   static FieldCreate* instance = 0;

   class FieldCreateExt : public FieldCreate {
   public:
       FieldCreateExt(): FieldCreate(){};
   };

    FieldCreate * getFieldCreate() {
           if(instance==0) instance = new FieldCreateExt();
            return instance;
    }

}}
