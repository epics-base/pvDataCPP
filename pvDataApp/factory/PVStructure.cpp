/*PVStructure.cpp*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "pvData.h"
#include "factory.h"
#include "bitSet.h"

namespace epics { namespace pvData {

    class PVStructurePvt {
    public:
        PVStructurePvt();
        ~PVStructurePvt();

        int numberFields;
        PVFieldPtrArray pvFields;
        String extendsStructureName;
    };

    PVStructurePvt::PVStructurePvt()
    : numberFields(0), pvFields(0),extendsStructureName("")
    {
    }

    PVStructurePvt::~PVStructurePvt()
    {
        for(int i=0; i<numberFields; i++) {
            delete pvFields[i];
        }
        if (pvFields) delete[] pvFields;
    }

    static PVField *findSubField(String fieldName,PVStructure *pvStructure);

    PVStructure::PVStructure(PVStructure *parent,StructureConstPtr structurePtr)
    : PVField(parent,structurePtr),pImpl(new PVStructurePvt())
    {
        int numberFields = structurePtr->getNumberFields();
        FieldConstPtrArray fields = structurePtr->getFields();
        pImpl->numberFields = numberFields;
        pImpl->pvFields = new PVFieldPtr[numberFields];
        PVFieldPtrArray pvFields = pImpl->pvFields;
        PVDataCreate *pvDataCreate = getPVDataCreate();
        for(int i=0; i<numberFields; i++) {
            FieldConstPtr field = fields[i];
            switch(field->getType()) {
            case scalar: {
                ScalarConstPtr scalar = (ScalarConstPtr)field;
                pvFields[i] = pvDataCreate->createPVScalar(this,scalar);
                break;
            }
            case scalarArray: {
                ScalarArrayConstPtr array = (ScalarArrayConstPtr)field;
                pvFields[i] = pvDataCreate->createPVScalarArray(this,array);
                break;
            }
            case structure: {
                StructureConstPtr structPtr = (StructureConstPtr)field;
                pvFields[i] = pvDataCreate->createPVStructure(this, structPtr);
                break;
            }
            case structureArray: {
                StructureArrayConstPtr structArray = (StructureArrayConstPtr)field;
                pvFields[i] = pvDataCreate->createPVStructureArray(this,
                    structArray);
                break;
            }
            }
        }
    }

    PVStructure::PVStructure(PVStructure *parent,StructureConstPtr structurePtr,
        PVFieldPtrArray pvFields
    )
    : PVField(parent,structurePtr),pImpl(new PVStructurePvt())
    {
        int numberFields = structurePtr->getNumberFields();
        pImpl->numberFields = numberFields;
        pImpl->pvFields = pvFields;
        for(int i=0; i<numberFields; i++) {
            PVField *pvField = pvFields[i];
            setParentPvt(pvField,this);
        }
    }

    void PVStructure::setParentPvt(PVField *pvField,PVStructure *parent)
    {
        pvField->setParent(parent);
        if(pvField->getField()->getType()==structure) {
            PVStructure *subStructure = static_cast<PVStructure*>(pvField);
            PVFieldPtr *subFields = subStructure->getPVFields();
            int numberFields = subStructure->getStructure()->getNumberFields();
            for(int i=0; i<numberFields; i++) {
                PVField *subField = static_cast<PVField*>(subFields[i]);
                setParentPvt(subField,subStructure);
            }
        }
    }

    PVStructure::~PVStructure()
    {
        delete pImpl;
    }

    StructureConstPtr PVStructure::getStructure()
    {
        return (StructureConstPtr)PVField::getField();
    }

    PVFieldPtrArray PVStructure::getPVFields()
    {
        return pImpl->pvFields;
    }

    PVFieldPtr PVStructure::getSubField(String fieldName)
    {
        return findSubField(fieldName,this);
    }

    PVFieldPtr PVStructure::getSubField(int fieldOffset)
    {
        if(fieldOffset<=getFieldOffset()) {
            if(fieldOffset==getFieldOffset()) return this;
            return 0;
        }
        if(fieldOffset>getNextFieldOffset()) return 0;
        int numFields = pImpl->numberFields;
        PVFieldPtrArray pvFields = pImpl->pvFields;
        for(int i=0; i<numFields; i++) {
            PVField *pvField = pvFields[i];
            if(pvField->getFieldOffset()==fieldOffset) return pvField;
            if(pvField->getNextFieldOffset()<=fieldOffset) continue;
            if(pvField->getField()->getType()==structure) {
                return ((PVStructure*)pvField)->getSubField(fieldOffset);
            }
        }
        String message("PVStructure.getSubField: Logic error");
        throw std::logic_error(message);
    }

    void PVStructure::appendPVField(PVFieldPtr pvField)
    {
        Structure *structure = const_cast<Structure *>(getStructure());
        structure->appendField(pvField->getField());
        int origLength = pImpl->numberFields;
        PVFieldPtrArray oldPVFields = pImpl->pvFields;
        PVFieldPtrArray newPVFields = new PVFieldPtr[origLength + 1];
        for(int i=0; i<origLength; i++) {
            newPVFields[i] = oldPVFields[i];
        }
        // note that origLength IS new element
        newPVFields[origLength] = pvField;
        delete[] pImpl->pvFields;
        pImpl->pvFields = newPVFields;
        pImpl->numberFields = origLength + 1;
    }

    void PVStructure::appendPVFields(int numberNewFields,PVFieldPtrArray pvFields)
    {
        Structure *structure = const_cast<Structure *>(getStructure());
        FieldConstPtr fields[numberNewFields];
        for(int i=0; i<numberNewFields; i++) fields[i] = pvFields[i]->getField();
        structure->appendFields(numberNewFields,fields);
        int origLength = pImpl->numberFields;
        PVFieldPtrArray oldPVFields = pImpl->pvFields;
        int numberFields = origLength + numberNewFields;
        PVFieldPtrArray newPVFields = new PVFieldPtr[numberFields];
        for(int i=0; i<origLength; i++) {
            newPVFields[i] = oldPVFields[i];
        }
        for(int i=0; i<numberNewFields; i++) {
            newPVFields[i+origLength] = pvFields[i];
        }
        delete[] pImpl->pvFields;
        pImpl->pvFields = newPVFields;
        pImpl->numberFields = numberFields;
    }

    void PVStructure::removePVField(String fieldName)
    {
        PVField *pvField = getSubField(fieldName);
        if(pvField==0) {
            String message("removePVField ");
            message +=  fieldName + " does not exist";
            this->message(message, errorMessage);
            return;
        }
        int origLength = pImpl->numberFields;
        int newLength = origLength - 1;
        PVFieldPtrArray origPVFields = pImpl->pvFields;
        PVFieldPtrArray newPVFields = new PVFieldPtr[newLength];
        int newIndex = 0;
        int indRemove = -1;
        for(int i=0; i<origLength; i++) {
            if(origPVFields[i]==pvField) {
                indRemove = i;
            } else {
                newPVFields[newIndex++] = origPVFields[i];
            }
        }
        Structure *structure = const_cast<Structure *>(getStructure());
        structure->removeField(indRemove);
        delete[] pImpl->pvFields;
        pImpl->pvFields = newPVFields;
        pImpl->numberFields = newLength;
    }

    PVBoolean *PVStructure::getBooleanField(String fieldName)
    {
        PVField *pvField = findSubField(fieldName,this);
        if(pvField==0) {
            String message("fieldName ");
            message +=  fieldName + " does not exist";
            this->message(message, errorMessage);
            return 0;
        }
        if(pvField->getField()->getType()==scalar) {
            ScalarConstPtr scalar = (ScalarConstPtr)pvField->getField();
            if(scalar->getScalarType()==pvBoolean) {
                return (PVBoolean*)pvField;
            }
        }
        String message("fieldName ");
        message +=  fieldName + " does not have type boolean ";
        this->message(message, errorMessage);
        return 0;
    }

    PVByte *PVStructure::getByteField(String fieldName)
    {
        PVField *pvField = findSubField(fieldName,this);
        if(pvField==0) {
            String message("fieldName ");
            message +=  fieldName + " does not exist";
            this->message(message, errorMessage);
            return 0;
        }
        if(pvField->getField()->getType()==scalar) {
            ScalarConstPtr scalar = (ScalarConstPtr)pvField->getField();
            if(scalar->getScalarType()==pvByte) {
                return (PVByte*)pvField;
            }
        }
        String message("fieldName ");
        message +=  fieldName + " does not have type byte ";
        this->message(message, errorMessage);
        return 0;
    }

    PVShort *PVStructure::getShortField(String fieldName)
    {
        PVField *pvField = findSubField(fieldName,this);
        if(pvField==0) {
            String message("fieldName ");
            message +=  fieldName + " does not exist";
            this->message(message, errorMessage);
            return 0;
        }
        if(pvField->getField()->getType()==scalar) {
            ScalarConstPtr scalar = (ScalarConstPtr)pvField->getField();
            if(scalar->getScalarType()==pvShort) {
                return (PVShort*)pvField;
            }
        }
        String message("fieldName ");
        message +=  fieldName + " does not have type short ";
        this->message(message, errorMessage);
        return 0;
    }

    PVInt *PVStructure::getIntField(String fieldName)
    {
        PVField *pvField = findSubField(fieldName,this);
        if(pvField==0) {
            String message("fieldName ");
            message +=  fieldName + " does not exist";
            this->message(message, errorMessage);
            return 0;
        }
        if(pvField->getField()->getType()==scalar) {
            ScalarConstPtr scalar = (ScalarConstPtr)pvField->getField();
            if(scalar->getScalarType()==pvInt) {
                return (PVInt*)pvField;
            }
        }
        String message("fieldName ");
        message +=  fieldName + " does not have type int ";
        this->message(message, errorMessage);
        return 0;
    }

    PVLong *PVStructure::getLongField(String fieldName)
    {
        PVField *pvField = findSubField(fieldName,this);
        if(pvField==0) {
            String message("fieldName ");
            message +=  fieldName + " does not exist";
            this->message(message, errorMessage);
            return 0;
        }
        if(pvField->getField()->getType()==scalar) {
            ScalarConstPtr scalar = (ScalarConstPtr)pvField->getField();
            if(scalar->getScalarType()==pvLong) {
                return (PVLong*)pvField;
            }
        }
        String message("fieldName ");
        message +=  fieldName + " does not have type long ";
        this->message(message, errorMessage);
        return 0;
    }

    PVFloat *PVStructure::getFloatField(String fieldName)
    {
        PVField *pvField = findSubField(fieldName,this);
        if(pvField==0) {
            String message("fieldName ");
            message +=  fieldName + " does not exist";
            this->message(message, errorMessage);
            return 0;
        }
        if(pvField->getField()->getType()==scalar) {
            ScalarConstPtr scalar = (ScalarConstPtr)pvField->getField();
            if(scalar->getScalarType()==pvFloat) {
                return (PVFloat*)pvField;
            }
        }
        String message("fieldName ");
        message +=  fieldName + " does not have type float ";
        this->message(message, errorMessage);
        return 0;
    }

    PVDouble *PVStructure::getDoubleField(String fieldName)
    {
        PVField *pvField = findSubField(fieldName,this);
        if(pvField==0) {
            String message("fieldName ");
            message +=  fieldName + " does not exist";
            this->message(message, errorMessage);
            return 0;
        }
        if(pvField->getField()->getType()==scalar) {
            ScalarConstPtr scalar = (ScalarConstPtr)pvField->getField();
            if(scalar->getScalarType()==pvDouble) {
                return (PVDouble*)pvField;
            }
        }
        String message("fieldName ");
        message +=  fieldName + " does not have type double ";
        this->message(message, errorMessage);
        return 0;
    }

    PVString *PVStructure::getStringField(String fieldName)
    {
        PVField *pvField = findSubField(fieldName,this);
        if(pvField==0) {
            String message("fieldName ");
            message +=  fieldName + " does not exist";
            this->message(message, errorMessage);
            return 0;
        }
        if(pvField->getField()->getType()==scalar) {
            ScalarConstPtr scalar = (ScalarConstPtr)pvField->getField();
            if(scalar->getScalarType()==pvString) {
                return (PVString*)pvField;
            }
        }
        String message("fieldName ");
        message +=  fieldName + " does not have type string ";
        this->message(message, errorMessage);
        return 0;
    }

    PVStructure *PVStructure::getStructureField(String fieldName)
    {
        PVField *pvField = findSubField(fieldName,this);
        if(pvField==0) {
            String message("fieldName ");
            message +=  fieldName + " does not exist";
            this->message(message, errorMessage);
            return 0;
        }
        if(pvField->getField()->getType()==structure) {
            return((PVStructure *)pvField);
        }
        String message("fieldName ");
        message +=  fieldName + " does not have type structure ";
        this->message(message, errorMessage);
        return 0;
    }

    PVScalarArray *PVStructure::getScalarArrayField(
        String fieldName,ScalarType elementType)
    {
        PVField *pvField = findSubField(fieldName,this);
        if(pvField==0) {
            String message("fieldName ");
            message +=  fieldName + " does not exist";
            this->message(message, errorMessage);
            return 0;
        }
        FieldConstPtr field = pvField->getField();
        Type type = field->getType();
        if(type!=scalarArray) {
            String message("fieldName ");
            message +=  fieldName + " does not have type array ";
            this->message(message, errorMessage);
            return 0;
        }
        ScalarArrayConstPtr array = (ScalarArrayConstPtr)field;
        if(array->getElementType()!=elementType) {
            String message("fieldName ");
            message +=  fieldName + " is array but does not have elementType ";
            ScalarTypeFunc::toString(&message,elementType);
            this->message(message, errorMessage);
            return 0;
        }
        return (PVScalarArray*)pvField;
    }

    PVStructureArray *PVStructure::getStructureArrayField(
        String fieldName)
    {
        PVField *pvField = findSubField(fieldName,this);
        if(pvField==0) {
            String message("fieldName ");
            message +=  fieldName + " does not exist";
            this->message(message, errorMessage);
            return 0;
        }
        if(pvField->getField()->getType()==structureArray) {
            return((PVStructureArray *)pvField);
        }
        String message("fieldName ");
        message +=  fieldName + " does not have type structureArray ";
        this->message(message, errorMessage);
        return 0;
    }

    String PVStructure::getExtendsStructureName()
    {
        return pImpl->extendsStructureName;
    }

    bool PVStructure::putExtendsStructureName(
        String extendsStructureName)
    {
        if(pImpl->extendsStructureName.length()!=0) return false;
        pImpl->extendsStructureName = extendsStructureName;
        return true;
    }

    void PVStructure::serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher) const {
        for(int i = 0; i<pImpl->numberFields; i++)
            pImpl->pvFields[i]->serialize(pbuffer, pflusher);
    }

    void PVStructure::deserialize(ByteBuffer *pbuffer,
            DeserializableControl *pcontrol) {
        for(int i = 0; i<pImpl->numberFields; i++)
            pImpl->pvFields[i]->deserialize(pbuffer, pcontrol);

    }

    void PVStructure::serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher, BitSet *pbitSet) const {
        int offset = const_cast<PVStructure*>(this)->getFieldOffset();
        int numberFields = const_cast<PVStructure*>(this)->getNumberFields();
        int next = pbitSet->nextSetBit(offset);

        // no more changes or no changes in this structure
        if(next<0||next>=offset+numberFields) return;

        // entire structure
        if(offset==next) {
            serialize(pbuffer, pflusher);
            return;
        }

        for(int i = 0; i<pImpl->numberFields; i++) {
            PVField* pvField = pImpl->pvFields[i];
            offset = pvField->getFieldOffset();
            numberFields = pvField->getNumberFields();
            next = pbitSet->nextSetBit(offset);
            // no more changes
            if(next<0) return;
            //  no change in this pvField
            if(next>=offset+numberFields) continue;

            // serialize field or fields
            if(numberFields==1)
                pvField->serialize(pbuffer, pflusher);
            else
                ((PVStructure*)pvField)->serialize(pbuffer, pflusher,
                        pbitSet);
        }
    }

    void PVStructure::deserialize(ByteBuffer *pbuffer,
            DeserializableControl *pcontrol, BitSet *pbitSet) {
        int offset = getFieldOffset();
        int numberFields = getNumberFields();
        int next = pbitSet->nextSetBit(offset);

        // no more changes or no changes in this structure
        if(next<0||next>=offset+numberFields) return;

        // entire structure
        if(offset==next) {
            deserialize(pbuffer, pcontrol);
            return;
        }

        for(int i = 0; i<pImpl->numberFields; i++) {
            PVField* pvField = pImpl->pvFields[i];
            offset = pvField->getFieldOffset();
            numberFields = pvField->getNumberFields();
            next = pbitSet->nextSetBit(offset);
            // no more changes
            if(next<0) return;
            //  no change in this pvField
            if(next>=offset+numberFields) continue;

            // deserialize field or fields
            if(numberFields==1)
                pvField->deserialize(pbuffer, pcontrol);
            else
                ((PVStructure*)pvField)->deserialize(pbuffer, pcontrol,
                        pbitSet);
        }
    }


    bool PVStructure::operator==(PVField &obj)
    {
        return getConvert()->equals(this,&obj);
    }

    bool PVStructure::operator!=(PVField  &pv)
    {
        return !(getConvert()->equals(this,&pv));
    }

    static PVField *findSubField(String fieldName,PVStructure *pvStructure) {
        if( fieldName.length()<1) return 0;
        String::size_type index = fieldName.find('.');
        String name = fieldName;
        String restOfName = String();
        if(index>0) {
            name = fieldName.substr(0, index);
            if(fieldName.length()>index) {
                restOfName = fieldName.substr(index+1);
            }
        }
        PVFieldPtrArray pvFields = pvStructure->getPVFields();
        PVField *pvField = 0;
        int numFields = pvStructure->getStructure()->getNumberFields();
        for(int i=0; i<numFields; i++) {
            PVField *pvf = pvFields[i];
            int result = pvf->getField()->getFieldName().compare(name);
            if(result==0) {
                pvField = pvf;
                break;
            }
        }
        if(pvField==0) return 0;
        if(restOfName.length()==0) return pvField;
        if(pvField->getField()->getType()!=structure) return 0;
        return findSubField(restOfName,(PVStructure*)pvField);
    }

    class BasePVStructure : public PVStructure {
    public:
        BasePVStructure(PVStructure *parent,StructureConstPtr structure);
        BasePVStructure(PVStructure *parent,StructureConstPtr structure,
            PVFieldPtrArray pvFields);
        ~BasePVStructure();
    private:
    };

    BasePVStructure::BasePVStructure(PVStructure *parent,StructureConstPtr structure)
        : PVStructure(parent,structure) {}

    BasePVStructure::BasePVStructure(PVStructure *parent,StructureConstPtr structure,
        PVFieldPtrArray pvFields)
        : PVStructure(parent,structure,pvFields) {}

    BasePVStructure::~BasePVStructure() {}

}}
