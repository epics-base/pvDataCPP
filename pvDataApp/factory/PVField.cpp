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
#include <pv/lock.h>
#include <pv/pvData.h>
#include <pv/factory.h>
#include <pv/convert.h>

using std::tr1::const_pointer_cast;
using std::size_t;

namespace epics { namespace pvData {

static String notImplemented("not implemented");


PVField::PVField(FieldConstPtr field)
: parent(NULL),field(field),
  fieldOffset(0), nextFieldOffset(0),
  immutable(false),requester(0),postHandler(0),
  convert(getConvert())
{
}

PVField::~PVField()
{ }

void PVField::message(String fieldName,String message,MessageType messageType)  
{
    if(parent!=NULL) {
        String parentName = parent->getFieldName();
        if(parentName.length()>0) {
            fieldName = parentName + "." + fieldName;
        }
        parent->message(fieldName,message,messageType);
        return;
    }
    if(requester) {
        String mess = fieldName + " " + message;
        requester->message(mess,messageType);
    } else {
        printf("%s %s %s\n",
            getMessageTypeName(messageType).c_str(),
            fieldName.c_str(),
            message.c_str());
    }
}

void PVField::message(String message,MessageType messageType)  
{
    PVField::message(fieldName,message,messageType);
}

String PVField::getFieldName()
{
    return fieldName;
}

void PVField::setRequester(Requester *requester)
{
    if(parent!=NULL) {
        throw std::logic_error(
            "PVField::setRequester only legal for top level structure");
    }
    if(requester!=NULL) {
        if(requester==requester) return;
        throw std::logic_error(
            "PVField::setRequester requester is already present");
    }
    requester = requester;
}

size_t PVField::getFieldOffset() 
{
    if(nextFieldOffset==0) computeOffset(this);
    return fieldOffset;
}

size_t PVField::getNextFieldOffset() 
{
    if(nextFieldOffset==0) computeOffset(this);
    return nextFieldOffset;
}

size_t PVField::getNumberFields() 
{
    if(nextFieldOffset==0) computeOffset(this);
    return (nextFieldOffset - fieldOffset);
}

PVAuxInfoPtr & PVField::getPVAuxInfo(){
    if(pvAuxInfo.get()==NULL) {
        pvAuxInfo = PVAuxInfoPtr(new PVAuxInfo(this));
    }
    return pvAuxInfo;
}

bool PVField::isImmutable()  {return immutable;}

void PVField::setImmutable() {immutable = true;}

FieldConstPtr & PVField::getField()  {return field;}

PVStructure *PVField::getParent()  {return parent;}

void PVField::replacePVField(PVFieldPtr & newPVField)
{
    if(parent==NULL) {
        throw std::logic_error("no parent");
    }
    PVFieldPtrArray  pvFields = parent->getPVFields();
    StructureConstPtr structure = parent->getStructure();
    StringArray fieldNames = structure->getFieldNames();
    for(size_t i=0; i<fieldNames.size(); i++) {
        if(newPVField->getFieldName().compare(fieldNames[i]) == 0) {
            pvFields[i] = newPVField;
            return;
        }
    }
    throw std::logic_error("Did not find field in parent");
}

void PVField::replaceField(FieldConstPtr &xxx)
{
    field = xxx;
}

void PVField::renameField(String  newName)
{
    if(parent==NULL) {
        throw std::logic_error("no parent");
    }
    std::tr1::shared_ptr<Structure> parentStructure = const_pointer_cast<Structure>(
         parent->getStructure());
    FieldConstPtrArray const &fields = parentStructure->getFields();
    for(size_t i=0; i<fields.size(); i++) {
         if(fields[i].get()==field.get()) {
             parentStructure->renameField(i,newName);
             fieldName = newName;
             return;
         }
    }
    throw std::logic_error("Did not find field in parent");
}

void PVField::postPut() 
{
   if(postHandler!=NULL) postHandler->postPut();
}

void PVField::setPostHandler(PostHandler *postHandler)
{
    if(postHandler!=NULL) {
        if(postHandler==postHandler) return;
        throw std::logic_error(
            "PVField::setPostHandler a postHandler is already registered");

    }
    postHandler = postHandler;
}

void PVField::setParentAndName(PVStructure * xxx,String & name)
{
    parent = xxx;
    fieldName = name;
}

bool PVField::equals(PVField &pv)
{
    return convert->equals(*this,pv);
}

void PVField::toString(StringBuilder buf)
{
    toString(buf,0);
}

void PVField::toString(StringBuilder buf,int indentLevel) 
{
   static ConvertPtr convert = getConvert();
   convert->getString(buf,this,indentLevel);
   if(pvAuxInfo.get()!=NULL) pvAuxInfo->toString(buf,indentLevel);
}

void PVField::computeOffset(PVField   *  pvField) {
    PVStructure * pvTop = pvField->getParent();
    if(pvTop==NULL) {
        if(pvField->getField()->getType()!=structure) {
           pvField->fieldOffset = 0;
           pvField->nextFieldOffset = 1;
           return;
        }
        pvTop = static_cast<PVStructure *>(pvField);
    } else {
        while(pvTop->getParent()!=NULL) pvTop = pvTop->getParent();
    }
    int offset = 0;
    int nextOffset = 1;
    PVFieldPtrArray  pvFields = pvTop->getPVFields();
    for(size_t i=0; i < pvTop->getStructure()->getNumberFields(); i++) {
        offset = nextOffset;
        PVField *pvField = pvFields[i].get();
        FieldConstPtr field = pvField->getField();
        switch(field->getType()) {
        case scalar:
        case scalarArray:
        case structureArray:{
            nextOffset++;
            pvField->fieldOffset = offset;
            pvField->nextFieldOffset = nextOffset;
            break;
        }
        case structure: {
            pvField->computeOffset(pvField,offset);
            nextOffset = pvField->getNextFieldOffset();
        }
        }
    }
    PVField *top = (PVField *)pvTop;
    top->fieldOffset = 0;
    top->nextFieldOffset = nextOffset;
}

void PVField::computeOffset(PVField   *  pvField,size_t offset) {
    int beginOffset = offset;
    int nextOffset = offset + 1;
    PVStructure *pvStructure = static_cast<PVStructure *>(pvField);
    PVFieldPtrArray  pvFields = pvStructure->getPVFields();
    for(size_t i=0; i < pvStructure->getStructure()->getNumberFields(); i++) {
        offset = nextOffset;
        PVField *pvSubField = pvFields[i].get();
        FieldConstPtr field = pvSubField->getField();
        switch(field->getType()) {
            case scalar:
            case scalarArray:
            case structureArray: {
                nextOffset++;
                pvSubField->fieldOffset = offset;
                pvSubField->nextFieldOffset = nextOffset;
                break;
            }
            case structure: {
                pvSubField->computeOffset(pvSubField,offset);
                nextOffset = pvSubField->getNextFieldOffset();
            }
        }
    }
    pvField->fieldOffset = beginOffset;
    pvField->nextFieldOffset = nextOffset;
}

}}
