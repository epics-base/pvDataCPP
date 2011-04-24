/* pvIntrospect.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef PVINTROSPECT_H
#define PVINTROSPECT_H
#include <string>
#include <stdexcept>

#include "noDefaultMethods.h"
#include "sharedPtr.h"
#include "pvType.h"
namespace epics { namespace pvData { 

class Field;
class Scalar;
class ScalarArray;
class Structure;
class StructureArray;

typedef std::tr1::shared_ptr<const Field> FieldConstPtr;
typedef FieldConstPtr * FieldConstPtrArray;
typedef std::tr1::shared_ptr<const Scalar> ScalarConstPtr;
typedef std::tr1::shared_ptr<const ScalarArray> ScalarArrayConstPtr;
typedef std::tr1::shared_ptr<const Structure> StructureConstPtr;
typedef std::tr1::shared_ptr<const StructureArray> StructureArrayConstPtr;

enum Type {
    scalar,
    scalarArray,
    structure,
    structureArray
};

namespace TypeFunc {
    const char* name(Type);
    void toString(StringBuilder buf,const Type type);
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

namespace ScalarTypeFunc {
    bool isInteger(ScalarType type);
    bool isNumeric(ScalarType type);
    bool isPrimitive(ScalarType type);
    ScalarType getScalarType(String value);
    const char* name(ScalarType);
    void toString(StringBuilder buf,ScalarType scalarType);
};

class Field :  public std::tr1::enable_shared_from_this<Field> {
public:
   typedef std::tr1::shared_ptr<Field> shared_pointer;
   typedef std::tr1::shared_ptr<const Field> const_shared_pointer;

   String getFieldName() const{return m_fieldName;}
   Type getType() const{return m_type;}
   virtual void toString(StringBuilder buf) const{toString(buf,0);}
   virtual void toString(StringBuilder buf,int indentLevel) const;
   void renameField(String  newName);
protected:
   Field(String fieldName,Type type);
   virtual ~Field();
private:
   String m_fieldName;
   Type m_type;

   friend class StructureArray;
   friend class Structure;
   friend class PVFieldPvt;
   friend class StandardField;
   friend class BasePVStructureArray;
   friend class FieldCreate;

   struct Deleter{void operator()(Field *p){delete p;}};
};


class Scalar : public Field{
public:
   typedef std::tr1::shared_ptr<Scalar> shared_pointer;
   typedef std::tr1::shared_ptr<const Scalar> const_shared_pointer;
   typedef Scalar& reference;
   typedef const Scalar& const_reference;

   ScalarType getScalarType() const {return scalarType;}
   virtual void toString(StringBuilder buf) const{toString(buf,0);}
   virtual void toString(StringBuilder buf,int indentLevel) const;
protected:
   Scalar(String fieldName,ScalarType scalarType);
   virtual ~Scalar();
private:
   ScalarType scalarType;
   friend class FieldCreate;
};

class ScalarArray : public Field{
public:
   typedef std::tr1::shared_ptr<ScalarArray> shared_pointer;
   typedef std::tr1::shared_ptr<const ScalarArray> const_shared_pointer;
   typedef ScalarArray& reference;
   typedef const ScalarArray& const_reference;

   ScalarType  getElementType() const {return elementType;}
   virtual void toString(StringBuilder buf) const{toString(buf,0);}
   virtual void toString(StringBuilder buf,int indentLevel) const;
protected:
   ScalarArray(String fieldName,ScalarType scalarType);
   virtual ~ScalarArray();
private:
   ScalarType elementType;
   friend class FieldCreate;
};

class StructureArray : public Field{
public:
   typedef std::tr1::shared_ptr<StructureArray> shared_pointer;
   typedef std::tr1::shared_ptr<const StructureArray> const_shared_pointer;
   typedef StructureArray& reference;
   typedef const StructureArray& const_reference;

   const Structure& structure() const {return *pstructure;}
   StructureConstPtr  getStructure() const {return pstructure;}

   virtual void toString(StringBuilder buf,int indentLevel=0) const;
protected:
   StructureArray(String fieldName,StructureConstPtr structure);
   virtual ~StructureArray();
private:
    StructureConstPtr pstructure;
   friend class FieldCreate;
};

class Structure : public Field {
public:
   typedef std::tr1::shared_ptr<Structure> shared_pointer;
   typedef std::tr1::shared_ptr<const Structure> const_shared_pointer;
   typedef Structure& reference;
   typedef const Structure& const_reference;

   int getNumberFields() const {return numberFields;}
   FieldConstPtr getField(String fieldName) const;
   int getFieldIndex(String fieldName) const;
   FieldConstPtrArray getFields() const {return fields;}
   void appendField(FieldConstPtr field);
   void appendFields(int numberFields,FieldConstPtrArray fields);
   void removeField(int index);
   virtual void toString(StringBuilder buf) const{toString(buf,0);}
   virtual void toString(StringBuilder buf,int indentLevel) const;
protected:
   Structure(String fieldName, int numberFields,FieldConstPtrArray fields);
   virtual ~Structure();
private:
    int numberFields;
    FieldConstPtrArray  fields;
   friend class FieldCreate;
};

class FieldCreate : NoDefaultMethods {
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
