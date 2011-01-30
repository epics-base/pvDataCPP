/*PVField.cpp*/
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
#include "pvData.h"
#include "factory.h"
#include "showConstructDestruct.h"

namespace epics { namespace pvData {

static String notImplemented("not implemented");

PVDATA_REFCOUNT_MONITOR_DEFINE(pvField);

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
}

PVFieldPvt::~PVFieldPvt()
{
    if(pvAuxInfo!=0) delete pvAuxInfo;
    if(parent==0) field->decReferenceCount();
}



PVField::PVField(PVStructure *parent,FieldConstPtr field)
: pImpl(new PVFieldPvt(parent,field))
{
    PVDATA_REFCOUNT_MONITOR_CONSTRUCT(pvField);
}

PVField::~PVField()
{
    PVDATA_REFCOUNT_MONITOR_DESTRUCT(pvField);
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


bool PVField::renameField(String  newName)
{
    if(pImpl->parent!=0) {
        StructureConstPtr structure = pImpl->parent->getStructure();
        int index = structure->getFieldIndex(newName);
        if(index>=0) return false;
    }
    Field * field = const_cast<Field *>(pImpl->field);
    field->renameField(newName);
    return true;
}

void PVField::postPut() 
{
   if(pImpl->postHandler!=0) pImpl->postHandler->postPut();
}

void PVField::setPostHandler(PostHandler *postHandler)
{
    if(pImpl->postHandler!=0) {
        if(postHandler==pImpl->postHandler) return;
        String message(
            "PVField::setPostHandler a postHandler is already registered");
        throw std::logic_error(message);
    }
    pImpl->postHandler = postHandler;
}

void PVField::toString(StringBuilder buf)  {toString(buf,0);}

void PVField::toString(StringBuilder buf,int indentLevel) 
{
   getConvert()->getString(buf,this,indentLevel);
   if(pImpl->pvAuxInfo==0) return;
   pImpl->pvAuxInfo->toString(buf,indentLevel);
}

void PVField::computeOffset(PVField   *  pvField) {
    PVStructure *pvTop = pvField->getParent();
    if(pvTop==0) {
        if(pvField->getField()->getType()!=structure) {
           pvField->pImpl->fieldOffset = 0;
           pvField->pImpl->nextFieldOffset = 1;
           return;
        }
        pvTop = static_cast<PVStructure *>(pvField);
    } else {
        while(pvTop->getParent()!=0) pvTop = pvTop->getParent();
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
   PVStructure *pvStructure = static_cast<PVStructure *>(pvField);
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

}}
