/*BasePVStructure.h*/
#ifndef BASEPVSTRUCTURE_H
#define BASEPVSTRUCTURE_H
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "pvData.h"
#include "factory.h"

namespace epics { namespace pvData {

    class PVStructurePvt {
    public:
        PVStructurePvt();
        ~PVStructurePvt();

        int numberFields;
        PVFieldArrayPtr pvFields;
        std::string const extendsStructureName;
    };

    PVStructurePvt::PVStructurePvt()
    : numberFields(0), pvFields(0),extendsStructureName(0)
    {
    }

    PVStructurePvt::~PVStructurePvt()
    {
        for(int i=0; i<numberFields; i++) {
            delete pvFields[i];
        }
    }

    PVStructure::PVStructure(PVStructure *parent,StructureConstPtr structure)
    : PVField(parent,structure),pImpl(new PVStructurePvt())
    {
        int numberFields = structure->getNumberFields();
        pImpl->numberFields = numberFields;
        pImpl->pvFields = new PVFieldPtr[numberFields];
        for(int i=0; i<numberFields; i++) {
            // MARTY FINISH THIS
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

    PVFieldArrayPtr PVStructure::getPVFields()
    {
        return pImpl->pvFields;
    }

    PVFieldPtr PVStructure::getSubField(StringConstPtr fieldName)
    {
        return 0;
    }

    PVFieldPtr PVStructure::getSubField(int fieldOffset)
    {
        return 0;
    }

    void PVStructure::appendPVField(PVFieldPtr pvField)
    {
    }

    void PVStructure::appendPVFields(PVFieldArrayPtr pvFields)
    {
    }

    void PVStructure::removePVField(StringConstPtr fieldName)
    {
    }

    PVBoolean *PVStructure::getBooleanField(StringConstPtr fieldName)
    {
        return 0;
    }

    PVByte *PVStructure::getByteField(StringConstPtr fieldName)
    {
        return 0;
    }

    PVShort *PVStructure::getShortField(StringConstPtr fieldName)
    {
        return 0;
    }

    PVInt *PVStructure::getIntField(StringConstPtr fieldName)
    {
        return 0;
    }

    PVLong *PVStructure::getLongField(StringConstPtr fieldName)
    {
        return 0;
    }

    PVFloat *PVStructure::getFloatField(StringConstPtr fieldName)
    {
        return 0;
    }

    PVDouble *PVStructure::getDoubleField(StringConstPtr fieldName)
    {
        return 0;
    }

    PVString *PVStructure::getStringField(StringConstPtr fieldName)
    {
        return 0;
    }

    PVStructure *PVStructure::getStructureField(StringConstPtr fieldName)
    {
        return 0;
    }

    PVScalarArray *PVStructure::getScalarArrayField(
        StringConstPtr fieldName,ScalarType elementType)
    {
        return 0;
    }

    PVStructureArray *PVStructure::getStructureArrayField(
        StringConstPtr fieldName)
    {
        return 0;
    }

    StringConstPtr PVStructure::getExtendsStructureName()
    {
        return 0;
    }

    epicsBoolean PVStructure::putExtendsStructureName(
        StringConstPtr extendsStructureName)
    {
        return epicsFalse;
    }

    void PVStructure::toString(StringPtr buf) const {toString(buf,0);}

    void PVStructure::toString(StringPtr buf,int indentLevel) const
    {
    }

    void PVStructure::serialize(
        ByteBuffer *pbuffer,SerializableControl *pflusher) const
    {
    }

    void PVStructure::deserialize(
        ByteBuffer *pbuffer,DeserializableControl *pflusher)
    {
    }

    void PVStructure::serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher, int offset, int count) const
    {
    }

    void PVStructure::serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher,BitSet *pbitSet) const
    {
    }

    void PVStructure::deserialize(ByteBuffer *pbuffer,
            DeserializableControl*pflusher,BitSet *pbitSet)
    {
    }

    class BasePVStructure : public PVStructure {
    public:
        BasePVStructure(PVStructure *parent,StructureConstPtr structure);
        ~BasePVStructure();
    private:
        BasePVStructure(); // not implemented
        BasePVStructure(BasePVStructure const & ); // not implemented
        BasePVStructure& operator=(BasePVStructure const &); //not implemented

    };

    BasePVStructure::BasePVStructure(PVStructure *parent,StructureConstPtr structure)
        : PVStructure(parent,structure) {}

    BasePVStructure::~BasePVStructure() {}

}}
#endif  /* BASEPVSTRUCTURE_H */
