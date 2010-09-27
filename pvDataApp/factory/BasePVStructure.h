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
        StringConst const extendsStructureName;
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

    PVFieldPtr PVStructure::getSubField(StringConst fieldName)
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

    void PVStructure::removePVField(StringConst fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVBoolean *PVStructure::getBooleanField(StringConst fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVByte *PVStructure::getByteField(StringConst fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVShort *PVStructure::getShortField(StringConst fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVInt *PVStructure::getIntField(StringConst fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVLong *PVStructure::getLongField(StringConst fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVFloat *PVStructure::getFloatField(StringConst fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVDouble *PVStructure::getDoubleField(StringConst fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVString *PVStructure::getStringField(StringConst fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVStructure *PVStructure::getStructureField(StringConst fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVScalarArray *PVStructure::getScalarArrayField(
        StringConst fieldName,ScalarType elementType)
    {
        throw std::logic_error(notImplemented);
    }

    PVStructureArray *PVStructure::getStructureArrayField(
        StringConst fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    StringConst PVStructure::getExtendsStructureName()
    {
        throw std::logic_error(notImplemented);
    }

    epicsBoolean PVStructure::putExtendsStructureName(
        StringConst extendsStructureName)
    {
        throw std::logic_error(notImplemented);
    }

    void PVStructure::toString(StringBuilder buf) const {toString(buf,0);}

    void PVStructure::toString(StringBuilder buf,int indentLevel) const
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
    };

    BasePVStructure::BasePVStructure(PVStructure *parent,StructureConstPtr structure)
        : PVStructure(parent,structure) {}

    BasePVStructure::~BasePVStructure() {}

}}
#endif  /* BASEPVSTRUCTURE_H */
