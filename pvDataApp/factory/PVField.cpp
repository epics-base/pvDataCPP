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
  immutable(false),
  convert(getConvert())
{
}

PVField::~PVField()
{ }

void PVField::message(String message,MessageType messageType,String fullFieldName)  
{
    if(parent!=NULL) {
        if(fullFieldName.length()>0) {
            fullFieldName = fieldName + '.' + fullFieldName;
        } else {
            fullFieldName = fieldName;
        }
        parent->message(message,messageType,fullFieldName);
        return;
    }
    message = fullFieldName + " " + message;
    if(requester) {
        requester->message(message,messageType);
    } else {
        printf("%s %s %s\n",
            getMessageTypeName(messageType).c_str(),
            fieldName.c_str(),
            message.c_str());
    }
}

void PVField::message(String message,MessageType messageType)  
{
    PVField::message(message,messageType,"");
}

String PVField::getFieldName() const
{
    return fieldName;
}

void PVField::setRequester(RequesterPtr const &req)
{
    if(parent!=NULL) {
        throw std::logic_error(
            "PVField::setRequester only legal for top level structure");
    }
    if(requester.get()!=NULL) {
        if(requester.get()==req.get()) return;
        throw std::logic_error(
            "PVField::setRequester requester is already present");
    }
    requester = req;
}

size_t PVField::getFieldOffset() const
{
    if(nextFieldOffset==0) computeOffset(this);
    return fieldOffset;
}

size_t PVField::getNextFieldOffset() const
{
    if(nextFieldOffset==0) computeOffset(this);
    return nextFieldOffset;
}

size_t PVField::getNumberFields() const
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

bool PVField::isImmutable() const {return immutable;}

void PVField::setImmutable() {immutable = true;}

const FieldConstPtr & PVField::getField() const {return field;}

PVStructure *PVField::getParent() const {return parent;}

void PVField::replacePVField(const PVFieldPtr & newPVField)
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
    PVFieldPtrArray  pvFields = parent->getPVFields();
    for(size_t i=0; i<pvFields.size(); i++) {
         if(pvFields[i].get()==this) {
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

void PVField::setPostHandler(PostHandlerPtr const &handler)
{
    if(postHandler.get()!=NULL) {
        if(postHandler.get()==handler.get()) return;
        throw std::logic_error(
            "PVField::setPostHandler a postHandler is already registered");

    }
    postHandler = handler;
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

void PVField::computeOffset(const PVField   *  pvField) {
    const PVStructure * pvTop = pvField->getParent();
    if(pvTop==NULL) {
        if(pvField->getField()->getType()!=structure) {
           PVField *xxx = const_cast<PVField *>(pvField);
           xxx->fieldOffset = 0;
           xxx->nextFieldOffset = 1;
           return;
        }
        pvTop = static_cast<const PVStructure *>(pvField);
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
    PVField *xxx = const_cast<PVField *>(top);
    xxx->fieldOffset = 0;
    xxx->nextFieldOffset = nextOffset;
}

void PVField::computeOffset(const PVField   *  pvField,size_t offset) {
    int beginOffset = offset;
    int nextOffset = offset + 1;
    const PVStructure *pvStructure = static_cast<const PVStructure *>(pvField);
    const PVFieldPtrArray  pvFields = pvStructure->getPVFields();
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
    PVField *xxx = const_cast<PVField *>(pvField);
    xxx->fieldOffset = beginOffset;
    xxx->nextFieldOffset = nextOffset;
}

}}
