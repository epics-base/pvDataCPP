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


static StringConst notImplemented("not implemented");

    static Convert *convert = 0;

    class PVFieldPvt {
    public:
        PVFieldPvt(PVStructure *parent,FieldConstPtr field);
        ~PVFieldPvt();
        PVStructure *parent;
        FieldConstPtr field;
        int fieldOffset;
        int nextFieldOffset;
        PVAuxInfo *pvAuxInfo;
        epicsBoolean immutable;
        Requester *requester;
        PostHandler *postHandler;
    private:
        static void init();
    };

    void PVFieldPvt::init()
    {
        convert = getConvert();
    }

    PVFieldPvt::PVFieldPvt(PVStructure *parent,FieldConstPtr field)
    : parent(parent),field(field),
      fieldOffset(0), nextFieldOffset(0),
      pvAuxInfo(0),
      immutable(epicsFalse),requester(0),postHandler(0)
    {
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

     StringConst PVField::getRequesterName() const
     {
         static StringConst none("none");
         if(pImpl->requester!=0) return pImpl->requester->getRequesterName();
         return none;
     }

     void PVField::message(StringConst message,MessageType messageType) const 
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
         static StringConst requesterPresent =
             "Logic Error. requester is already present";
         if(pImpl->requester==0) {
             pImpl->requester = prequester;
             return;
         }
         throw std::logic_error(requesterPresent);
     }

     int PVField::getFieldOffset() const
     {
         if(pImpl->nextFieldOffset==0) computeOffset(this);
         return pImpl->fieldOffset;
     }

     int PVField::getNextFieldOffset() const
     {
         if(pImpl->nextFieldOffset==0) computeOffset(this);
         return pImpl->nextFieldOffset;
     }

     int PVField::getNumberFields() const
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

     epicsBoolean PVField::isImmutable() const {return pImpl->immutable;}

     void PVField::setImmutable() {pImpl->immutable = epicsTrue;}

     FieldConstPtr PVField::getField() const {return pImpl->field;}

     PVStructure * PVField::getParent() const {return pImpl->parent;}

     void PVField::replacePVField(PVField * newPVField)
     {
         throw std::logic_error(notImplemented);
     }

     void PVField::renameField(StringConst  newName)
     {
        throw std::logic_error(notImplemented);
     }

     void PVField::postPut() const
     {
        throw std::logic_error(notImplemented);
     }

     void PVField::setPostHandler(PostHandler *ppostHandler)
     {
        throw std::logic_error(notImplemented);
     }

     void PVField::toString(StringBuilder buf) const {toString(buf,0);}

     void PVField::toString(StringBuilder buf,int indentLevel) const
     {
        throw std::logic_error(notImplemented);
     }

     void PVField::computeOffset(PVField  const * const pvField) {
    	PVStructure *pvTop = pvField->getParent();
    	if(pvTop==0) {
    		pvTop = (PVStructure *)pvField;
    	} else {
    		while(pvTop->getParent()!=0) {
                    pvTop = pvTop->getParent();
                }
    	}
        int offset = 0;
        int nextOffset = 1;
        PVFieldArrayPtr pvFields = pvTop->getPVFields();
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

     void PVField::computeOffset(PVField  const * const pvField,int offset) {
        int beginOffset = offset;
        int nextOffset = offset + 1;
        PVStructure *pvStructure = (PVStructure *)pvField;
        PVFieldArrayPtr pvFields = pvStructure->getPVFields();
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
#endif  /* ABSTRACTPVFIELD_H */
