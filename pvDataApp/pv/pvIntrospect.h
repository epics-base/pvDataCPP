/* pvIntrospect.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
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

class Field :  private NoDefaultMethods {
public:
   int getReferenceCount() const;
   String getFieldName() const;
   Type getType() const;
   virtual void toString(StringBuilder buf) const{toString(buf,0);}
   virtual void toString(StringBuilder buf,int indentLevel) const;
   void incReferenceCount() const;
   void decReferenceCount() const;
   void dumpReferenceCount(StringBuilder buf,int indentLevel) const;
   virtual bool operator==(const Field& field) const;
   virtual bool operator!=(const Field& field) const;
protected:
   Field(String fieldName,Type type);
   virtual ~Field();
private:
   class FieldPvt *pImpl;
   friend class StructureArray;
   friend class Structure;
   friend class PVFieldPvt;
   friend class StandardField;
   friend class BasePVStructureArray;
   friend class FieldCreate;
};


class Scalar : public Field{
public:
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
   StructureConstPtr  getStructure() const {return pstructure;}
   virtual void toString(StringBuilder buf) const{toString(buf,0);}
   virtual void toString(StringBuilder buf,int indentLevel) const;
protected:
   StructureArray(String fieldName,StructureConstPtr structure);
   virtual ~StructureArray();
private:
    StructureConstPtr pstructure;
   friend class FieldCreate;
};

class Structure : public Field {
public:
   int getNumberFields() const {return numberFields;}
   FieldConstPtr getField(String fieldName) const;
   int getFieldIndex(String fieldName) const;
   FieldConstPtrArray getFields() const {return fields;}
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
