/*FieldCreateFactory.cpp*/
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <lock.h>
#include "pvIntrospect.h"
#include "factory.h"

namespace epics { namespace pvData {

    static DebugLevel debugLevel = highDebug;
    static void newLine(StringBuilder buffer, int indentLevel)
    {
        *buffer += "\n";
        for(int i=0; i<indentLevel; i++) *buffer += "    ";
    }

    Field::~Field(){}

    class BaseField : public Field {
    public:
       BaseField(String fieldName,Type type);
       virtual ~BaseField();
       virtual void decReferenceCount() const;
       virtual void incReferenceCount() const {referenceCount++;}
       virtual int getReferenceCount() const {return referenceCount;}
       virtual String getFieldName() const {return fieldName;}
       virtual Type getType() const {return type;}
       virtual void toString(StringBuilder buf) const {return toString(buf,0);}
       virtual void toString(StringBuilder buf,int indentLevel) const;
    private:
       String fieldName;
       Type type;
       mutable volatile int referenceCount;
    };

    BaseField::BaseField(String fieldName,Type type)
       :fieldName(fieldName),type(type), referenceCount(0){}

    BaseField::~BaseField() {
        // note that c++ automatically calls destructor for fieldName
        if(debugLevel==highDebug) printf("~BaseField %s\n",fieldName.c_str());
    }

    void BaseField::decReferenceCount() const {
         if(referenceCount<=0) {
              String message("logicError field ");
              message += fieldName;
              throw std::logic_error(message);
         }
         referenceCount--;
         if(referenceCount==0) delete this;
    }
 
    void BaseField::toString(StringBuilder buffer,int indentLevel) const{
        newLine(buffer,indentLevel);
        *buffer += "field ";
        *buffer += fieldName.c_str();
        *buffer += " type ";
        TypeFunc::toString(buffer,type);
    }

    Scalar::~Scalar(){}

    class BaseScalar: private BaseField,public Scalar {
    public:
        BaseScalar(String fieldName,ScalarType scalarType);
        virtual ~BaseScalar();
        virtual void incReferenceCount() const {BaseField::incReferenceCount();}
        virtual void decReferenceCount() const {BaseField::decReferenceCount();}
        virtual int getReferenceCount() const {return BaseField::getReferenceCount();}
        virtual String getFieldName() const{ return BaseField::getFieldName(); }
        virtual Type getType() const{return BaseField::getType();}
        virtual ScalarType getScalarType() const { return scalarType;}
        virtual void toString(StringBuilder buf) const {toString(buf,0);}
        virtual void toString(StringBuilder buf,int indentLevel) const;
    private:
        ScalarType scalarType;
    };

    BaseScalar::BaseScalar(String fieldName,ScalarType scalarType)
           : BaseField(fieldName,scalar),scalarType(scalarType){}
    BaseScalar::~BaseScalar() {}


    void BaseScalar::toString(StringBuilder buffer,int indentLevel) const{
        BaseField::toString(buffer,indentLevel);
        *buffer +=  " scalarType ";
        ScalarTypeFunc::toString(buffer,scalarType);
    }

    ScalarArray::~ScalarArray(){}

    class BaseScalarArray: private BaseField,public ScalarArray {
    public:
        BaseScalarArray(String fieldName,ScalarType elementType);
        virtual ~BaseScalarArray();
        virtual void incReferenceCount() const {BaseField::incReferenceCount();}
        virtual void decReferenceCount() const {BaseField::decReferenceCount();}
        virtual int getReferenceCount() const {return BaseField::getReferenceCount();}
        virtual String getFieldName() const{ return BaseField::getFieldName(); }
        virtual Type getType() const{return BaseField::getType();}
        virtual ScalarType getElementType() const { return elementType;}
        virtual void toString(StringBuilder buf) const {toString(buf,0);}
        virtual void toString(StringBuilder buf,int indentLevel) const;
    private:
        ScalarType elementType;
    };

    BaseScalarArray::BaseScalarArray
           (String fieldName,ScalarType elementType)
           : BaseField(fieldName,scalar),elementType(elementType){}
    BaseScalarArray::~BaseScalarArray() {}


    void BaseScalarArray::toString(StringBuilder buffer,int indentLevel) const{
        BaseField::toString(buffer,indentLevel);
        *buffer +=  " elementType ";
        ScalarTypeFunc::toString(buffer,elementType);
    }

    Structure::~Structure(){}

    class BaseStructure: private BaseField,public Structure {
    public:
        BaseStructure(String fieldName, int numberFields,FieldConstPtrArray fields);
        virtual ~BaseStructure();
        virtual void incReferenceCount() const {BaseField::incReferenceCount();}
        virtual void decReferenceCount() const {BaseField::decReferenceCount();}
        virtual int getReferenceCount() const {return BaseField::getReferenceCount();}
        virtual String getFieldName() const{ return BaseField::getFieldName(); }
        virtual Type getType() const{return BaseField::getType();}
        virtual int const getNumberFields() const {return numberFields;}
        virtual FieldConstPtr getField(String fieldName) const;
        virtual int getFieldIndex(String fieldName) const;
        virtual FieldConstPtrArray getFields() const { return fields;}
        virtual void toString(StringBuilder buf) const {toString(buf,0);}
        virtual void toString(StringBuilder buf,int indentLevel) const;
    private:
        int numberFields;
        FieldConstPtrArray  fields;
    };

    BaseStructure::BaseStructure (String fieldName,
        int numberFields, FieldConstPtrArray fields)
    : BaseField(fieldName,structure),
          numberFields(numberFields),
          fields(fields)
    {
        for(int i=0; i<numberFields; i++) {
            String name = fields[i]->getFieldName();
            // look for duplicates
            for(int j=i+1; j<numberFields; j++) {
                String otherName = fields[j]->getFieldName();
                int result = name.compare(otherName);
                if(result==0) {
                    String  message("duplicate fieldName ");
                    message += name;
                    throw std::invalid_argument(message);
                }
            }
            // inc reference counter
            fields[i]->incReferenceCount();
        }
    }
    BaseStructure::~BaseStructure() {
        if(debugLevel==highDebug) printf("~BaseStructure %s\n",BaseField::getFieldName().c_str());
        for(int i=0; i<numberFields; i++) {
            FieldConstPtr pfield = fields[i];
            pfield->decReferenceCount();
        }
    }

    FieldConstPtr  BaseStructure::getField(String fieldName) const {
        for(int i=0; i<numberFields; i++) {
            FieldConstPtr pfield = fields[i];
            int result = fieldName.compare(pfield->getFieldName());
            if(result==0) return pfield;
        }
        return 0;
    }

    int BaseStructure::getFieldIndex(String fieldName) const {
        for(int i=0; i<numberFields; i++) {
            FieldConstPtr pfield = fields[i];
            int result = fieldName.compare(pfield->getFieldName());
            if(result==0) return i;
        }
        return -1;
    }

    void BaseStructure::toString(StringBuilder buffer,int indentLevel) const{
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
        BaseStructureArray(String fieldName,StructureConstPtr structure);
        virtual ~BaseStructureArray();
        virtual void incReferenceCount() const {BaseField::incReferenceCount();}
        virtual void decReferenceCount() const {BaseField::decReferenceCount();}
        virtual int getReferenceCount() const {return BaseField::getReferenceCount();}
        virtual String getFieldName() const{
            return BaseField::getFieldName();
         }
        virtual Type getType() const{return BaseField::getType();}
        virtual StructureConstPtr  getStructure() const { return pstructure; }
        virtual void toString(StringBuilder buf) const {toString(buf,0);}
        virtual void toString(StringBuilder buf,int indentLevel) const;
    private:
        StructureConstPtr pstructure;
    };

    BaseStructureArray::BaseStructureArray(String fieldName,StructureConstPtr structure)
    : BaseField(fieldName,structureArray),pstructure(structure)
    {
        pstructure->incReferenceCount();
    }

    BaseStructureArray::~BaseStructureArray() {
        if(debugLevel==highDebug) printf("~BaseStructureArray\n");
        pstructure->decReferenceCount();
    }


    void BaseStructureArray::toString(StringBuilder buffer,int indentLevel) const {
        BaseField::toString(buffer,indentLevel);
        *buffer +=  " structure ";
        pstructure->toString(buffer,indentLevel + 1);
    }

  FieldCreate::FieldCreate(){};

   ScalarConstPtr  FieldCreate::createScalar(String fieldName,
       ScalarType scalarType) const
   {
         BaseScalar *baseScalar = new BaseScalar(fieldName,scalarType);
         return baseScalar;
   }
 
   ScalarArrayConstPtr FieldCreate::createScalarArray(
       String fieldName,ScalarType elementType) const
   {
         BaseScalarArray *baseScalarArray = new BaseScalarArray(fieldName,elementType);
         return baseScalarArray;
   }
   StructureConstPtr FieldCreate::createStructure (
       String fieldName,int numberFields,
       FieldConstPtr fields[]) const
   {
         BaseStructure *baseStructure = new BaseStructure(
             fieldName,numberFields,fields);
         return baseStructure;
   }
   StructureArrayConstPtr FieldCreate::createStructureArray(
       String fieldName,StructureConstPtr structure) const
   {
        BaseStructureArray *baseStructureArray = new BaseStructureArray(fieldName,structure);
        return baseStructureArray;
   }

   FieldConstPtr FieldCreate::create(String fieldName,
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
       String  message("field ");
       message += fieldName;
       throw std::logic_error(message);
   }

   static FieldCreate* instance = 0;


   class FieldCreateExt : public FieldCreate {
   public:
       FieldCreateExt(): FieldCreate(){};
   };

   FieldCreate * getFieldCreate() {
       static Mutex *mutex = new Mutex();
       Lock xx(mutex);

       if(instance==0) instance = new FieldCreateExt();
       return instance;
   }

}}
