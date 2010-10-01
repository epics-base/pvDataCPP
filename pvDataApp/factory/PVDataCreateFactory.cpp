/*PVDataCreateFactory.cpp*/
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "lock.h"
#include "pvIntrospect.h"
#include "pvData.h"
#include "convert.h"
#include "factory.h"
#include "AbstractPVField.h"
#include "AbstractPVScalar.h"
#include "AbstractPVArray.h"
#include "AbstractPVScalarArray.h"
#include "BasePVBoolean.h"
#include "BasePVByte.h"
#include "BasePVShort.h"
#include "BasePVInt.h"
#include "BasePVLong.h"
#include "BasePVFloat.h"
#include "BasePVDouble.h"
#include "BasePVString.h"
#include "AbstractPVArray.h"
#include "BasePVBooleanArray.h"
#include "BasePVByteArray.h"
#include "BasePVShortArray.h"
#include "BasePVIntArray.h"
#include "BasePVLongArray.h"
#include "BasePVFloatArray.h"
#include "BasePVDoubleArray.h"
#include "BasePVStringArray.h"
#include "BasePVStructure.h"
#include "BasePVStructureArray.h"

namespace epics { namespace pvData {

   static FieldCreate * fieldCreate = 0;
   static PVDataCreate* pvDataCreate = 0;

   PVDataCreate::PVDataCreate(){};

   PVField *PVDataCreate::createPVField(PVStructure *parent,
           FieldConstPtr field)
   {
        switch(field->getType()) {
        case scalar:
            return createPVScalar(parent,(ScalarConstPtr)field);
        case scalarArray:
            return (PVField *)createPVScalarArray(parent,
                (ScalarArrayConstPtr)field);
        case structure:
            return (PVField *)createPVStructure(parent,
                (StructureConstPtr)field);
        case structureArray:
            return createPVStructureArray(parent,
                (StructureArrayConstPtr)field);
        }
        String  message("PVDataCreate::createPVField");
        throw std::invalid_argument(message);
   };

   PVField *PVDataCreate::createPVField(PVStructure *parent,
           String fieldName,PVField * fieldToClone)
   {
        switch(fieldToClone->getField()->getType()) {
        case scalar:
            return createPVScalar(parent,fieldName,(PVScalar*)fieldToClone);
        case scalarArray:
            return (PVField *)createPVScalarArray(parent,fieldName,
                (PVScalarArray *)fieldToClone);
        case structure:
            return (PVField *)createPVStructure(parent,fieldName,
                (PVStructure *)fieldToClone);
        case structureArray:
           String message(
           "PVDataCreate::createPVField structureArray not valid fieldToClone");
           throw std::invalid_argument(message);
        }
        String message("PVDataCreate::createPVField");
        throw std::logic_error(message);
   };

   PVScalar *PVDataCreate::createPVScalar(PVStructure *parent,ScalarConstPtr scalar)
   {
        ScalarType scalarType = scalar->getScalarType();
        switch(scalarType) {
        case pvBoolean:
            return new BasePVBoolean(parent,scalar);
        case pvByte:
            return new BasePVByte(parent,scalar);
        case pvShort:
            return new BasePVShort(parent,scalar);
        case pvInt:
            return new BasePVInt(parent,scalar);
        case pvLong:
            return new BasePVLong(parent,scalar);
        case pvFloat:
            return new BasePVFloat(parent,scalar);
        case pvDouble:
            return new BasePVDouble(parent,scalar);
        case pvString:
            return new BasePVString(parent,scalar);
        default:
            throw std::logic_error(notImplemented);
        }
        throw std::logic_error(notImplemented);
   };

   PVScalar *PVDataCreate::createPVScalar(PVStructure *parent,
           String fieldName,ScalarType scalarType)
   {
        if(fieldCreate==0) fieldCreate = getFieldCreate();
        ScalarConstPtr scalar = fieldCreate->createScalar(fieldName,scalarType);
        return createPVScalar(parent,scalar);
   };

   PVScalar *PVDataCreate::createPVScalar(PVStructure *parent,
           String fieldName,PVScalar * scalarToClone)
   {
        PVScalar *pvScalar = createPVScalar(parent,fieldName,
            scalarToClone->getScalar()->getScalarType());
        //MARTY MUST CALL CONVERT
        //MARTY MUST COPY AUXInfo
        return pvScalar;
   };

   PVScalarArray *PVDataCreate::createPVScalarArray(PVStructure *parent,
           ScalarArrayConstPtr scalarArray)
   {
        switch(scalarArray->getElementType()) {
        case pvBoolean:
              return new BasePVBooleanArray(parent,scalarArray);
        case pvByte:
              return new BasePVByteArray(parent,scalarArray);
        case pvShort:
              return new BasePVShortArray(parent,scalarArray);
        case pvInt:
              return new BasePVIntArray(parent,scalarArray);
        case pvLong:
              return new BasePVLongArray(parent,scalarArray);
        case pvFloat:
              return new BasePVFloatArray(parent,scalarArray);
        case pvDouble:
              return new BasePVDoubleArray(parent,scalarArray);
        case pvString:
              return new BasePVStringArray(parent,scalarArray);
        }
        throw std::logic_error(notImplemented);
        
   };

   PVScalarArray *PVDataCreate::createPVScalarArray(PVStructure *parent,
           String fieldName,ScalarType elementType)
   {
        throw std::logic_error(notImplemented);
   };

   PVScalarArray *PVDataCreate::createPVScalarArray(PVStructure *parent,
           String fieldName,PVScalarArray * scalarArrayToClone)
   {
        throw std::logic_error(notImplemented);
   };

   PVStructureArray *PVDataCreate::createPVStructureArray(PVStructure *parent,
           StructureArrayConstPtr structureArray)
   {
        throw std::logic_error(notImplemented);
   };

   PVStructure *PVDataCreate::createPVStructure(PVStructure *parent,
           StructureConstPtr structure)
   {
        return new BasePVStructure(parent,structure);
   };

   PVStructure *PVDataCreate::createPVStructure(PVStructure *parent,
           String fieldName,FieldConstPtrArray fields)
   {
        throw std::logic_error(notImplemented);
   };

   PVStructure *PVDataCreate::createPVStructure(PVStructure *parent,
           String fieldName,PVStructure *structToClone)
   {
        throw std::logic_error(notImplemented);
   };


   class PVDataCreateExt : public PVDataCreate {
   public:
       PVDataCreateExt(): PVDataCreate(){};
   };

    PVDataCreate * getPVDataCreate() {
        static Mutex mutex = Mutex();
        Lock xx(&mutex);

        if(pvDataCreate==0) pvDataCreate = new PVDataCreateExt();
        return pvDataCreate;
    }

}}
