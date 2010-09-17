/* pvData.h */
#include <string>
#include <stdexcept>
#include <map>
#ifndef PVDATA_H
#define PVDATA_H
#include "pvIntrospect.h"
#include "requester.h"
#include "byteBuffer.h"
#include "serialize.h"
namespace epics { namespace pvData { 

    class PVField;
    class PVScalar;
    class PVBoolean;
    class PVByte;
    class PVShort;
    class PVInt;
    class PVLong;
    class PVFloat;
    class PVDouble;
    class PVString;

    class PVScalarArray;
    class PVBooleanArray;
    class PVByteArray;
    class PVShortArray;
    class PVIntArray;
    class PVLongArray;
    class PVFloatArray;
    class PVDoubleArray;
    class PVStringArray;

    class PVStructure;
    class PVStructureArray;

    class PVAuxInfo {
    public:
        virtual ~PVAuxInfo();
        virtual PVField * getPVField() const = 0;
        virtual PVScalar * createInfo(StringConstPtr key,ScalarType scalarType) = 0;
        /* following caused compiler error
        virtual map<std::string, ScalarType> getInfos() = 0;
        */
        virtual PVScalar * getInto(StringConstPtr key) = 0;
        virtual void toString(StringPtr buf) const = 0;
        virtual void toString(StringPtr buf,int indentLevel) const = 0;
    };

    class PostHandler {
        virtual void postPut() = 0;
    };

    class PVField : public Requester, public Serializable {
    public:
        virtual ~PVField();
        virtual void setRequester(Requester *prequester) = 0;
        virtual int getFieldOffset() const = 0;
        virtual int getNextFieldOffset() const = 0;
        virtual int getNumberFields() const = 0;
        virtual PVAuxInfo * getPVAuxInfo() const = 0;
        virtual epicsBoolean isImmutable() const = 0;
        virtual void setImmutable() = 0;
        virtual FieldConstPtr getField() const = 0;
        virtual PVStructure * getParent() const = 0;
        virtual void replacePVField(PVField * newPVField) = 0;
        virtual void renameField(StringConstPtr  newName) = 0;
        virtual void postPut() const = 0;
        virtual void setPostHandler(PostHandler *ppostHandler) = 0;
        virtual void toString(StringPtr buf) const = 0;
        virtual void toString(StringPtr buf,int indentLevel) const = 0;
    }; 

    class PVScalar : public PVField {
    public:
        virtual ~PVScalar();
        virtual ScalarConstPtr getScalar() const = 0;
    };

    class PVBoolean : public PVScalar {
    public:
        virtual ~PVBoolean();
        virtual epicsBoolean get() const = 0;
        virtual void put(epicsBoolean value) = 0;
    };

    class PVByte : public PVScalar {
    public:
        virtual ~PVByte();
        virtual epicsInt8 get() const = 0;
        virtual void put(epicsInt8 value) = 0;
    };

    class PVShort : public PVScalar {
    public:
        virtual ~PVShort();
        virtual epicsInt16 get() const = 0;
        virtual void put(epicsInt16 value) = 0;
    };

    class PVInt : public PVScalar {
    public:
        virtual ~PVInt();
        virtual epicsInt32 get() const = 0;
        virtual void put(epicsInt32 value) = 0;
    };

    class PVLong : public PVScalar {
    public:
        virtual ~PVLong();
        virtual epicsInt64 get() const = 0;
        virtual void put(epicsInt64 value) = 0;
    };

    class PVFloat : public PVScalar {
    public:
        virtual ~PVFloat();
        virtual float get() const = 0;
        virtual void put(float value) = 0;
    };

    class PVDouble : public PVScalar {
    public:
        virtual ~PVDouble();
        virtual double get() const = 0;
        virtual void put(double value) = 0;
    };

    class PVString : public PVScalar {
    public:
        virtual ~PVString();
        virtual StringConstPtr get() const = 0;
        virtual void put(StringConstPtr value) = 0;
    };
        

    class PVDataCreate {
    public:
       PVField *createPVField(PVStructure *parent,
           FieldConstPtr field) const;
       PVField *createPVField(PVStructure *parent,
           StringConstPtr fieldName,FieldConstPtr fieldToClone) const;
       PVScalar *createPVScalar(PVStructure *parent,ScalarConstPtr scalar) const;
       PVScalar *createPVScalar(PVStructure *parent,
           StringConstPtr fieldName,ScalarType scalarType);
       PVScalar *createPVScalar(PVStructure *parent,
           StringConstPtr fieldName,ScalarConstPtr scalarToClone) const;
       PVScalarArray *createPVScalarArray(PVStructure *parent,
           ScalarArrayConstPtr scalarArray) const;
       PVScalarArray *createPVScalarArray(PVStructure *parent,
           StringConstPtr fieldName,ScalarType elementType);
       PVScalarArray *createPVScalarArray(PVStructure *parent,
           StringConstPtr fieldName,ScalarArrayConstPtr scalarArrayToClone) const;
       PVStructureArray *createPVStructureArray(PVStructure *parent,
           StructureArrayConstPtr structureArray) const;
       PVStructure *createPVStructure(PVStructure *parent,
           StructureConstPtr structure);
       PVStructure *createPVStructure(PVStructure *parent,
           StringConstPtr fieldName,FieldConstPtrArray fields);
       PVStructure *createPVStructure(PVStructure *parent,
           StringConstPtr fieldName,PVStructure *structToClone);
    protected:
       PVDataCreate();
    };

    extern PVDataCreate * getPVDataCreate();
    
}}
#endif  /* PVDATA_H */
