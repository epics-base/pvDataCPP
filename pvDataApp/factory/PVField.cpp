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
#include "convert.h"
#include "CDRMonitor.h"

using std::tr1::const_pointer_cast;

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
   Convert *convert;
};

PVFieldPvt::PVFieldPvt(PVStructure *parent,FieldConstPtr field)
: parent(parent),field(field),
 fieldOffset(0), nextFieldOffset(0),
 pvAuxInfo(0),
 immutable(false),requester(0),postHandler(0),
 convert(getConvert())
{
}

PVFieldPvt::~PVFieldPvt()
{
    if(pvAuxInfo!=0) delete pvAuxInfo;
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

void PVField::message(String fieldName,String message,MessageType messageType)  
{
    if(pImpl->parent!=0) {
        String parentName = pImpl->parent->getField()->getFieldName();
        if(parentName.length()>0) {
            fieldName = parentName + "." + fieldName;
        }
        pImpl->parent->message(fieldName,message,messageType);
        return;
    }
    if(pImpl->requester) {
        String mess = fieldName + " " + message;
        pImpl->requester->message(mess,messageType);
    } else {
        printf("%s %s %s\n",
            messageTypeName[messageType].c_str(),
            fieldName.c_str(),
            message.c_str());
    }
}

void PVField::message(String message,MessageType messageType)  
{
    PVField::message(pImpl->field->getFieldName(),message,messageType);
}
void PVField::setRequester(Requester *requester)
{
    if(pImpl->parent!=0) {
        throw std::logic_error(String(
            "PVField::setRequester only legal for top level structure"));
    }
    if(pImpl->requester!=0) {
        if(pImpl->requester==requester) return;
        throw std::logic_error(String(
            "PVField::setRequester requester is already present"));
    }
    pImpl->requester = requester;
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
    Field::shared_pointer field(const_pointer_cast<Field>(pImpl->field));
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

void PVField::setParent(PVStructure * parent)
{
    pImpl->parent = parent;
}

bool PVField::equals(PVField &pv)
{
    return pImpl->convert->equals(*this,pv);
}

void PVField::toString(StringBuilder buf)  {toString(buf,0);}

void PVField::toString(StringBuilder buf,int indentLevel) 
{
   pImpl->convert->getString(buf,this,indentLevel);
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
