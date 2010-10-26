/*AbstractPVField.h*/
#ifndef ABSTRACTPVFIELD_H
#define ABSTRACTPVFIELD_H
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "pvData.h"
#include "factory.h"

namespace epics { namespace pvData {

static String notImplemented("not implemented");

class PVFieldPvt {
public:
   PVFieldPvt(PVStructure *parent,FieldConstPtr field);
   ~PVFieldPvt();
   PVStructure *parent;
   FieldConstPtr field;
   int fieldOffset;
   int nextFieldOffset;
   PVAuxInfo *pvAuxInfo;
   bool immutable;
   Requester *requester;
   PostHandler *postHandler;
};

PVFieldPvt::PVFieldPvt(PVStructure *parent,FieldConstPtr field)
: parent(parent),field(field),
 fieldOffset(0), nextFieldOffset(0),
 pvAuxInfo(0),
 immutable(false),requester(0),postHandler(0)
{
   delete pvAuxInfo;
   field->incReferenceCount();
}

PVFieldPvt::~PVFieldPvt()
{
   field->decReferenceCount();
}


PVField::PVField(PVStructure *parent,FieldConstPtr field)
: pImpl(new PVFieldPvt(parent,field))
{}

PVField::~PVField()
{
   delete pImpl;
}

String PVField::getRequesterName() 
{
static String none("none");
if(pImpl->requester!=0) return pImpl->requester->getRequesterName();
return none;
}

void PVField::message(String message,MessageType messageType)  
{
if(pImpl->requester) {
    pImpl->requester->message(message,messageType);
} else {
    printf("%s %s %s\n",
        messageTypeName[messageType].c_str(),
        pImpl->field->getFieldName().c_str(),
        message.c_str());
}
}
void PVField::setRequester(Requester *prequester)
{
static String requesterPresent =
    "Logic Error. requester is already present";
if(pImpl->requester==0) {
    pImpl->requester = prequester;
    return;
}
throw std::logic_error(requesterPresent);
}

int PVField::getFieldOffset() 
{
if(pImpl->nextFieldOffset==0) computeOffset(this);
return pImpl->fieldOffset;
}

int PVField::getNextFieldOffset() 
{
if(pImpl->nextFieldOffset==0) computeOffset(this);
return pImpl->nextFieldOffset;
}

int PVField::getNumberFields() 
{
if(pImpl->nextFieldOffset==0) computeOffset(this);
return (pImpl->nextFieldOffset - pImpl->fieldOffset);
}

PVAuxInfo * PVField::getPVAuxInfo(){
if(pImpl->pvAuxInfo==0) {
    pImpl->pvAuxInfo = new PVAuxInfo(this);
}
return pImpl->pvAuxInfo;
}

bool PVField::isImmutable()  {return pImpl->immutable;}

void PVField::setImmutable() {pImpl->immutable = true;}

FieldConstPtr PVField::getField()  {return pImpl->field;}

PVStructure * PVField::getParent()  {return pImpl->parent;}

void PVField::replacePVField(PVField * newPVField)
{
   PVStructure *parent = getParent();
   if(parent==0) {
       String message("PVField::replacePVField no parent");
       throw std::invalid_argument(message);
   }
   PVFieldPtrArray pvFields = parent->getPVFields();
   int index = -1;
   String fieldName = pImpl->field->getFieldName();
   int length = parent->getStructure()->getNumberFields();
   for(int i=0; i<length; i++) {
   	PVField *pvField = pvFields[i];
   	if(pvField->getField()->getFieldName().compare(fieldName) ==0) {
   		index = i;
   		break;
   	}
   }
   if(index==-1) {
   String message("PVField::replacePVField did not find field in parent");
   throw std::logic_error(message);
   }
   pvFields[index] = newPVField;
   parent->replaceStructure(parent);
}

void PVField::renameField(String  newName)
{
    FieldCreate *fieldCreate = getFieldCreate();
    switch(pImpl->field->getType()) {
        case scalar: {
            ScalarConstPtr scalar = (ScalarConstPtr)pImpl->field;
            scalar = fieldCreate->createScalar(newName, scalar->getScalarType());
            pImpl->field = scalar;
            break;
        }
        case scalarArray: {
            ScalarArrayConstPtr array = (ScalarArrayConstPtr)pImpl->field;
            array = fieldCreate->createScalarArray(newName, array->getElementType());
            pImpl->field = array;
            break;
        }
        case structure: {
            StructureConstPtr structure = (StructureConstPtr)pImpl->field;
            FieldConstPtrArray origFields = structure->getFields();
            int numberFields = structure->getNumberFields();
            structure = fieldCreate->createStructure(newName,numberFields,origFields);
            pImpl->field = structure;
            break;
        }
        case structureArray: {
            StructureArrayConstPtr structureArray = (StructureArrayConstPtr)pImpl->field;
            structureArray = fieldCreate->createStructureArray(newName,
                structureArray->getStructure());
            pImpl->field = structureArray;
        }
    }
   String message("PVField::renameField logic error. should not get here");
   throw std::logic_error(message);
}

void PVField::postPut() 
{
   if(pImpl->postHandler!=0) pImpl->postHandler->postPut();
}

void PVField::setPostHandler(PostHandler *postHandler)
{
    if(pImpl->postHandler!=0) {
        if(postHandler==pImpl->postHandler) return;
        String message("PVField::setPostHandler a postHandler is already registered");
        throw std::logic_error(message);
    }
    pImpl->postHandler = postHandler;
}

void PVField::toString(StringBuilder buf)  {toString(buf,0);}

void PVField::toString(StringBuilder buf,int indentLevel) 
{
   if(pImpl->pvAuxInfo==0) return;
   pImpl->pvAuxInfo->toString(buf,indentLevel);
}

void PVField::computeOffset(PVField   *  pvField) {
	PVStructure *pvTop = pvField->getParent();
   Type type = pvField->getField()->getType();
   if(type!=structure) {
   pvField->pImpl->nextFieldOffset = 1;
   return;
   }
	if(pvTop==0) {
		pvTop = (PVStructure *)pvField;
	} else {
		while(pvTop->getParent()!=0) {
           pvTop = pvTop->getParent();
       }
	}
   int offset = 0;
   int nextOffset = 1;
   PVFieldPtrArray pvFields = pvTop->getPVFields();
   for(int i=0; i < pvTop->getStructure()->getNumberFields(); i++) {
   offset = nextOffset;
   PVField *pvField = pvFields[i];
   FieldConstPtr field = pvField->getField();
   switch(field->getType()) {
   case scalar:
   case scalarArray:
   case structureArray:{
       nextOffset++;
       pvField->pImpl->fieldOffset = offset;
       pvField->pImpl->nextFieldOffset = nextOffset;
       break;
   }
   case structure: {
       pvField->computeOffset(pvField,offset);
       nextOffset = pvField->getNextFieldOffset();
   }
   }
   }
   PVField *top = (PVField *)pvTop;
   top->pImpl->fieldOffset = 0;
   top->pImpl->nextFieldOffset = nextOffset;
}

void PVField::computeOffset(PVField   *  pvField,int offset) {
   int beginOffset = offset;
   int nextOffset = offset + 1;
   PVStructure *pvStructure = (PVStructure *)pvField;
   PVFieldPtrArray pvFields = pvStructure->getPVFields();
   for(int i=0; i < pvStructure->getStructure()->getNumberFields(); i++) {
   offset = nextOffset;
   PVField *pvSubField = pvFields[i];
   FieldConstPtr field = pvSubField->getField();
   switch(field->getType()) {
   case scalar:
   case scalarArray:
   case structureArray: {
       nextOffset++;
       pvSubField->pImpl->fieldOffset = offset;
       pvSubField->pImpl->nextFieldOffset = nextOffset;
       break;
   }
   case structure: {
       pvSubField->computeOffset(pvSubField,offset);
       nextOffset = pvSubField->getNextFieldOffset();
   }
   }
   }
   pvField->pImpl->fieldOffset = beginOffset;
   pvField->pImpl->nextFieldOffset = nextOffset;
}

void PVField::replaceStructure(PVStructure *pvStructure)
{
        PVFieldPtrArray pvFields = pvStructure->getPVFields();
        int length = pvStructure->getStructure()->getNumberFields();
        FieldConstPtrArray newFields = new FieldConstPtr[length];
        for(int i=0; i<length; i++) {
            newFields[i] = pvFields[i]->getField();
        }
        StructureConstPtr newStructure = getFieldCreate()->createStructure(
             pImpl->field->getFieldName(),length, newFields);
        pImpl->field = newStructure;
        PVStructure *parent = pImpl->parent;
        if(parent!=0) {
            parent->replaceStructure(parent);
        }
}


}}
#endif  /* ABSTRACTPVFIELD_H */
