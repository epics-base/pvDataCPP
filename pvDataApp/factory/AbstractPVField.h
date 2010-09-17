/*AbstractPVField.cpp*/
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "pvData.h"
#include "factory.h"

namespace epics { namespace pvData {

    class AbstractPVField : public PVField {
    public:
        AbstractPVField(PVStructure *parent,FieldConstPtr field);
        virtual ~AbstractPVField();
        // from Requester
        virtual StringConstPtr getRequesterName() const;
        virtual void message(StringConstPtr message,MessageType messageType) const;
        // from PVField
        virtual void setRequester(Requester *prequester);
        virtual int getFieldOffset() const;
        virtual int getNextFieldOffset() const;
        virtual int getNumberFields() const;
        virtual PVAuxInfo * getPVAuxInfo() const;
        virtual epicsBoolean isImmutable() const;
        virtual void setImmutable();
        virtual FieldConstPtr getField() const;
        virtual PVStructure * getParent() const;
        virtual void replacePVField(PVField * newPVField);
        virtual void renameField(StringConstPtr  newName);
        virtual void postPut() const;
        virtual void setPostHandler(PostHandler *ppostHandler);
        virtual void toString(StringPtr buf) const;
        virtual void toString(StringPtr buf,int indentLevel) const;
    protected:
        void replaceStructure();
    private:
        AbstractPVField(AbstractPVField const & ); // not implemented
        AbstractPVField & operator=(AbstractPVField const &); //not implemented
        int fieldOffset;
        int nextFieldOffset;
        PVAuxInfo *pvAuxInfo;
        epicsBoolean immutable;
        PVStructure *parent;
        FieldConstPtr field;
        Requester *requester;
        PostHandler *postHandler;
    };

    AbstractPVField::AbstractPVField(PVStructure *parent,FieldConstPtr field)
    : fieldOffset(0),nextFieldOffset(0),pvAuxInfo(0),immutable(epicsFalse),
      parent(parent),field(field),requester(0),postHandler(0)
    {
        field->incReferenceCount();
    }

    AbstractPVField::~AbstractPVField()
    {
        field->decReferenceCount();
    }

     StringConstPtr AbstractPVField::getRequesterName() const
     {
         static std::string none("none");
         if(requester!=0) return requester->getRequesterName();
         return &none;
     }

     void AbstractPVField::message(StringConstPtr message,MessageType messageType) const
     {
         if(requester) {
             requester->message(message,messageType);
         } else {
             printf("%s %s %s\n",
                 messageTypeName[messageType].c_str(),
                 field->getFieldName()->c_str(),
                 message->c_str());
         }
     }
     void AbstractPVField::setRequester(Requester *prequester)
     {
         static std::string requesterPresent = "Logic Error. requester is already present";
         if(requester==0) {
             requester = prequester;
             return;
         }
         throw std::logic_error(requesterPresent);
     }

}}
