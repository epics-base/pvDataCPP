/*BasePVDouble.h*/
#ifndef BASEPVDOUBLE_H
#define BASEPVDOUBLE_H
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "pvData.h"
#include "factory.h"
#include "AbstractPVScalar.h"

namespace epics { namespace pvData {

    PVDouble::~PVDouble() {}

    class BasePVDouble : public AbstractPVScalar, public PVDouble {
    public:
        BasePVDouble(PVStructure *parent,ScalarConstPtr scalar)
        : AbstractPVScalar(parent,scalar),value(0.0) {}
        virtual ~BasePVDouble() {}
        // from Requester
        virtual StringConstPtr getRequesterName() const{
             return AbstractPVField::getRequesterName();}
        virtual void message(StringConstPtr message,MessageType messageType) const{
             AbstractPVField::message(message,messageType);}
        // from PVScalar
        virtual Scalar *getScalar() const{
             return (Scalar *)getField();}
        // from PVField
        virtual void setRequester(Requester *requester){
           AbstractPVField::setRequester(requester);}
        virtual int getFieldOffset() const{
           return AbstractPVField::getFieldOffset();}
        virtual int getNextFieldOffset() const{
           return AbstractPVField::getNextFieldOffset();}
        virtual int getNumberFields() const{
           return AbstractPVField::getNumberFields();}
        virtual PVAuxInfo * getPVAuxInfo() const{
           return AbstractPVField::getPVAuxInfo();}
        virtual epicsBoolean isImmutable() const{
           return AbstractPVField::isImmutable();}
        virtual void setImmutable(){
           AbstractPVField::setImmutable();}
        virtual FieldConstPtr getField() const{
           return AbstractPVField::getField();}
        virtual PVStructure * getParent() const{
           return AbstractPVField::getParent();}
        virtual void replacePVField(PVField * newPVField){
           AbstractPVField::replacePVField(newPVField);}
        virtual void renameField(StringConstPtr  newName){
           AbstractPVField::renameField(newName);}
        virtual void postPut() const{
           AbstractPVField::postPut();}
        virtual void setPostHandler(PostHandler *postHandler){
           AbstractPVField::setPostHandler(postHandler);}
        virtual void toString(StringPtr buf) const {toString(buf,0);}
        virtual void toString(StringPtr buf,int indentLevel) const{
           AbstractPVField::toString(buf,indentLevel);}
        // from PVDouble
        virtual double get()const { return value;}
        virtual void put(double val){value = val;}
       // from Serializable
       virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) const{}
       virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher){}
    private:
        double value;
    };


}}
#endif  /* BASEPVDOUBLE_H */
