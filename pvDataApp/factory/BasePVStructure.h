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
        PVFieldPtrArray pvFields;
        String extendsStructureName;
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

    PVFieldPtrArray PVStructure::getPVFields()
    {
        return pImpl->pvFields;
    }

    PVFieldPtr PVStructure::getSubField(String fieldName)
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

    void PVStructure::appendPVFields(PVFieldPtrArray pvFields)
    {
        throw std::logic_error(notImplemented);
    }

    void PVStructure::removePVField(String fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVBoolean *PVStructure::getBooleanField(String fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVByte *PVStructure::getByteField(String fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVShort *PVStructure::getShortField(String fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVInt *PVStructure::getIntField(String fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVLong *PVStructure::getLongField(String fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVFloat *PVStructure::getFloatField(String fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVDouble *PVStructure::getDoubleField(String fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVString *PVStructure::getStringField(String fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVStructure *PVStructure::getStructureField(String fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    PVScalarArray *PVStructure::getScalarArrayField(
        String fieldName,ScalarType elementType)
    {
        throw std::logic_error(notImplemented);
    }

    PVStructureArray *PVStructure::getStructureArrayField(
        String fieldName)
    {
        throw std::logic_error(notImplemented);
    }

    String PVStructure::getExtendsStructureName()
    {
        throw std::logic_error(notImplemented);
    }

    epicsBoolean PVStructure::putExtendsStructureName(
        String extendsStructureName)
    {
        throw std::logic_error(notImplemented);
    }

    void PVStructure::toString(StringBuilder buf)  {toString(buf,0);}

    void PVStructure::toString(StringBuilder buf,int indentLevel) 
    {
        throw std::logic_error(notImplemented);
    }

    void PVStructure::serialize(
        ByteBuffer *pbuffer,SerializableControl *pflusher) 
    {
        throw std::logic_error(notImplemented);
    }

    void PVStructure::deserialize(
        ByteBuffer *pbuffer,DeserializableControl *pflusher)
    {
        throw std::logic_error(notImplemented);
    }

    void PVStructure::serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher, int offset, int count) 
    {
        throw std::logic_error(notImplemented);
    }

    void PVStructure::serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher,BitSet *pbitSet) 
    {
        throw std::logic_error(notImplemented);
    }

    void PVStructure::deserialize(ByteBuffer *pbuffer,
            DeserializableControl*pflusher,BitSet *pbitSet)
    {
        throw std::logic_error(notImplemented);
    }

    epicsBoolean PVStructure::equals(PVField  *pv) 
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
