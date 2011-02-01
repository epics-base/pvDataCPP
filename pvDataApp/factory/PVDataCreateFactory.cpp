/*PVDataCreateFactory.cpp*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "lock.h"
#include "pvIntrospect.h"
#include "pvData.h"
#include "convert.h"
#include "factory.h"
#include "PVField.cpp"
#include "PVScalar.cpp"
#include "PVArray.cpp"
#include "PVScalarArray.cpp"
#include "PVStructure.cpp"
#include "DefaultPVBoolean.cpp"
#include "DefaultPVByte.cpp"
#include "DefaultPVShort.cpp"
#include "DefaultPVInt.cpp"
#include "DefaultPVLong.cpp"
#include "DefaultPVFloat.cpp"
#include "DefaultPVDouble.cpp"
#include "DefaultPVString.cpp"
#include "DefaultPVBooleanArray.cpp"
#include "DefaultPVByteArray.cpp"
#include "DefaultPVShortArray.cpp"
#include "DefaultPVIntArray.cpp"
#include "DefaultPVLongArray.cpp"
#include "DefaultPVFloatArray.cpp"
#include "DefaultPVDoubleArray.cpp"
#include "DefaultPVStringArray.cpp"
#include "DefaultPVStructureArray.cpp"

namespace epics { namespace pvData {

static Convert* convert = 0;
static FieldCreate * fieldCreate = 0;
static PVDataCreate* pvDataCreate = 0;

PVDataCreate::PVDataCreate(){ }

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
     String  message("PVDataCreate::createPVField should never get here");
     throw std::logic_error(message);
}

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
     String  message("PVDataCreate::createPVField should never get here");
     throw std::logic_error(message);
}

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
     }
     String  message("PVDataCreate::createPVScalar should never get here");
     throw std::logic_error(message);
}

PVScalar *PVDataCreate::createPVScalar(PVStructure *parent,
        String fieldName,ScalarType scalarType)
{
     ScalarConstPtr scalar = fieldCreate->createScalar(fieldName,scalarType);
     return createPVScalar(parent,scalar);
}


PVScalar *PVDataCreate::createPVScalar(PVStructure *parent,
        String fieldName,PVScalar * scalarToClone)
{
     scalarToClone->getField()->incReferenceCount();
     PVScalar *pvScalar = createPVScalar(parent,fieldName,
         scalarToClone->getScalar()->getScalarType());
     convert->copyScalar(scalarToClone, pvScalar);
     PVScalarMap attributes = scalarToClone->getPVAuxInfo()->getInfos();
     PVAuxInfo *pvAttribute = pvScalar->getPVAuxInfo();
     PVScalarMapIter p;
     for(p=attributes.begin(); p!=attributes.end(); p++) {
          String key = p->first;
          PVScalar *fromAttribute = p->second;
          PVScalar *toAttribute = pvAttribute->createInfo(key,
              fromAttribute->getScalar()->getScalarType());
          convert->copyScalar(fromAttribute,toAttribute);
     }
     return pvScalar;
}

PVScalarArray *PVDataCreate::createPVScalarArray(PVStructure *parent,
        ScalarArrayConstPtr scalarArray)
{
     switch(scalarArray->getElementType()) {
     case pvBoolean:
           return new DefaultPVBooleanArray(parent,scalarArray);
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
     String  message("PVDataCreate::createPVScalarArray should never get here");
     throw std::logic_error(message);
     
}

PVScalarArray *PVDataCreate::createPVScalarArray(PVStructure *parent,
        String fieldName,ScalarType elementType)
{
     return createPVScalarArray(parent,
         fieldCreate->createScalarArray(fieldName, elementType));
}

PVScalarArray *PVDataCreate::createPVScalarArray(PVStructure *parent,
        String fieldName,PVScalarArray * arrayToClone)
{
     arrayToClone->getField()->incReferenceCount();
     PVScalarArray *pvArray = createPVScalarArray(parent,fieldName,
          arrayToClone->getScalarArray()->getElementType());
     convert->copyScalarArray(arrayToClone,0, pvArray,0,arrayToClone->getLength());
     PVScalarMap attributes = arrayToClone->getPVAuxInfo()->getInfos();
     PVAuxInfo *pvAttribute = pvArray->getPVAuxInfo();
     PVScalarMapIter p;
     for(p=attributes.begin(); p!=attributes.end(); p++) {
          String key = p->first;
          PVScalar *fromAttribute = p->second;
          PVScalar *toAttribute = pvAttribute->createInfo(key,
              fromAttribute->getScalar()->getScalarType());
          convert->copyScalar(fromAttribute,toAttribute);
     }
    return pvArray;
}

PVStructureArray *PVDataCreate::createPVStructureArray(PVStructure *parent,
        StructureArrayConstPtr structureArray)
{
     return new BasePVStructureArray(parent,structureArray);
}

PVStructure *PVDataCreate::createPVStructure(PVStructure *parent,
        StructureConstPtr structure)
{
     PVStructure *pvStructure = new BasePVStructure(parent,structure);
     return pvStructure;
}

PVStructure *PVDataCreate::createPVStructure(PVStructure *parent,
        String fieldName,int numberFields,FieldConstPtrArray fields)
{
     StructureConstPtr structure = fieldCreate->createStructure(
         fieldName,numberFields, fields);
     return new BasePVStructure(parent,structure);
}

PVStructure *PVDataCreate::createPVStructure(PVStructure *parent,
        String fieldName,PVStructure *structToClone)
{
    FieldConstPtrArray fields = 0;
    int numberFields = 0;
    PVStructure *pvStructure = 0;;
    if(structToClone==0) {
        fields = new FieldConstPtr[0];
        StructureConstPtr structure = fieldCreate->createStructure(
            fieldName,numberFields,fields);
        pvStructure = new BasePVStructure(parent,structure);
    } else {
       StructureConstPtr structure = structToClone->getStructure();
       structure->incReferenceCount();
       pvStructure = new BasePVStructure(parent,structure);
       convert->copyStructure(structToClone,pvStructure);
    }
    return pvStructure;
}

 PVDataCreate * getPVDataCreate() {
     static Mutex mutex;
     Lock xx(&mutex);

     if(pvDataCreate==0){
          pvDataCreate = new PVDataCreate();
          convert = getConvert();
          fieldCreate = getFieldCreate();
     }
     return pvDataCreate;
 }

}}
