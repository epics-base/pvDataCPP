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
            throw std::logic_error(notImplemented);
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
        throw std::logic_error(notImplemented);
    }

    PVFieldPtr PVStructure::getSubField(int fieldOffset)
    {
        throw std::logic_error(notImplemented);
    }

    void PVStructure::appendPVField(PVFieldPtr pvField)
    {
        throw std::logic_error(notImplemented);
    }

    void PVStructure::appendPVFields(PVFieldArrayPtr pvFields)
    {
        throw std::logic_error(notImplemented);
    }

    void PVStructure::removePVField(StringConstPtr fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVBoolean *PVStructure::getBooleanField(StringConstPtr fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVByte *PVStructure::getByteField(StringConstPtr fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVShort *PVStructure::getShortField(StringConstPtr fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVInt *PVStructure::getIntField(StringConstPtr fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVLong *PVStructure::getLongField(StringConstPtr fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVFloat *PVStructure::getFloatField(StringConstPtr fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVDouble *PVStructure::getDoubleField(StringConstPtr fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVString *PVStructure::getStringField(StringConstPtr fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVStructure *PVStructure::getStructureField(StringConstPtr fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVScalarArray *PVStructure::getScalarArrayField(
        StringConstPtr fieldName,ScalarType elementType)
    {
        throw std::logic_error(notImplemented);
    }

    PVStructureArray *PVStructure::getStructureArrayField(
        StringConstPtr fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    StringConstPtr PVStructure::getExtendsStructureName()
    {
        throw std::logic_error(notImplemented);
    }

    epicsBoolean PVStructure::putExtendsStructureName(
        StringConstPtr extendsStructureName)
    {
        throw std::logic_error(notImplemented);
    }

    void PVStructure::toString(StringPtr buf) const {toString(buf,0);}

    void PVStructure::toString(StringPtr buf,int indentLevel) const
    {
        throw std::logic_error(notImplemented);
    }

    void PVStructure::serialize(
        ByteBuffer *pbuffer,SerializableControl *pflusher) const
    {
        throw std::logic_error(notImplemented);
    }

    void PVStructure::deserialize(
        ByteBuffer *pbuffer,DeserializableControl *pflusher)
    {
        throw std::logic_error(notImplemented);
    }

    void PVStructure::serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher, int offset, int count) const
    {
        throw std::logic_error(notImplemented);
    }

    void PVStructure::serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher,BitSet *pbitSet) const
    {
        throw std::logic_error(notImplemented);
    }

    void PVStructure::deserialize(ByteBuffer *pbuffer,
            DeserializableControl*pflusher,BitSet *pbitSet)
    {
        throw std::logic_error(notImplemented);
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
