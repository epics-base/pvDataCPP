/*PVStructure.cpp*/
/*
 * License terms for this software can be found in the file LICENSE that is included with the distribution
 */
/**
 *  @author mrk
 */
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <vector>

#define epicsExportSharedSymbols
#include <pv/pvData.h>
#include <pv/pvIntrospect.h>
#include <pv/factory.h>
#include <pv/bitSet.h>

using std::tr1::static_pointer_cast;
using std::size_t;
using std::string;

namespace epics { namespace pvData {

PVFieldPtr PVStructure::nullPVField;
PVBooleanPtr PVStructure::nullPVBoolean;
PVBytePtr PVStructure::nullPVByte;
PVShortPtr PVStructure::nullPVShort;
PVIntPtr PVStructure::nullPVInt;
PVLongPtr PVStructure::nullPVLong;
PVUBytePtr PVStructure::nullPVUByte;
PVUShortPtr PVStructure::nullPVUShort;
PVUIntPtr PVStructure::nullPVUInt;
PVULongPtr PVStructure::nullPVULong;
PVFloatPtr PVStructure::nullPVFloat;
PVDoublePtr PVStructure::nullPVDouble;
PVStringPtr PVStructure::nullPVString;
PVStructurePtr PVStructure::nullPVStructure;
PVStructureArrayPtr PVStructure::nullPVStructureArray;
PVUnionPtr PVStructure::nullPVUnion;
PVUnionArrayPtr PVStructure::nullPVUnionArray;
PVScalarArrayPtr PVStructure::nullPVScalarArray;

PVStructure::PVStructure(StructureConstPtr const & structurePtr)
: PVField(structurePtr),
  structurePtr(structurePtr),
  extendsStructureName("")
{
    size_t numberFields = structurePtr->getNumberFields();
    FieldConstPtrArray const & fields = structurePtr->getFields();
    StringArray const & fieldNames = structurePtr->getFieldNames();
    pvFields.reserve(numberFields);
    PVDataCreatePtr pvDataCreate = getPVDataCreate();
    for(size_t i=0; i<numberFields; i++) {
        pvFields.push_back(pvDataCreate->createPVField(fields[i]));
    }
    for(size_t i=0; i<numberFields; i++) {
    	pvFields[i]->setParentAndName(this,fieldNames[i]);
    }
}

PVStructure::PVStructure(StructureConstPtr const & structurePtr,
    PVFieldPtrArray const & pvs
)
: PVField(structurePtr),
  structurePtr(structurePtr),
  extendsStructureName("")
{
    size_t numberFields = structurePtr->getNumberFields();
    StringArray const & fieldNames = structurePtr->getFieldNames();
    pvFields.reserve(numberFields);
    for(size_t i=0; i<numberFields; i++) {
        pvFields.push_back(pvs[i]);
    }
    for(size_t i=0; i<numberFields; i++) {
        pvFields[i]->setParentAndName(this,fieldNames[i]);
    }
}

PVStructure::~PVStructure()
{
}

void PVStructure::setImmutable()
{
    size_t numFields = pvFields.size();
    for(size_t i=0; i<numFields; i++) {
        PVFieldPtr pvField  = pvFields[i];
        pvField->setImmutable();
    }
    PVField::setImmutable();
}

StructureConstPtr PVStructure::getStructure() const
{
    return structurePtr;
}

const PVFieldPtrArray & PVStructure::getPVFields() const
{
    return pvFields;
}

PVFieldPtr  PVStructure::getSubField(const char * fieldName) const
{
    PVField * field = getSubFieldImpl(fieldName, false);
    if (field)
       return field->shared_from_this();
    else
       return PVFieldPtr();
}


PVFieldPtr  PVStructure::getSubField(size_t fieldOffset) const
{
    if(fieldOffset<=getFieldOffset()) {
        return nullPVField;
    }
    if(fieldOffset>getNextFieldOffset()) return nullPVField;
    size_t numFields = pvFields.size();
    for(size_t i=0; i<numFields; i++) {
        PVFieldPtr pvField  = pvFields[i];
        if(pvField->getFieldOffset()==fieldOffset) return pvFields[i];
        if(pvField->getNextFieldOffset()<=fieldOffset) continue;
        if(pvField->getField()->getType()==structure) {
            PVStructure *pvStructure = static_cast<PVStructure *>(pvField.get());
            return pvStructure->getSubField(fieldOffset);
        }
    }
    throw std::logic_error("PVStructure.getSubField: Logic error");
}

PVFieldPtr PVStructure::getSubFieldT(std::size_t fieldOffset) const
{
    PVFieldPtr pvField = getSubField(fieldOffset);
    if (pvField) return pvField;
    std::stringstream ss;
    ss << "Failed to get field with offset "  << fieldOffset << "(Invalid offset)" ;
    throw std::runtime_error(ss.str());
}

PVField* PVStructure::getSubFieldImpl(const char *name, bool throws) const
{
    const PVStructure *parent = this;
    if(!name)
    {
        if (throws)
            throw std::invalid_argument("Failed to get field: (Field name is NULL string)");
        else
            return NULL;
    }
    const char *fullName = name;
    while(true) {
        const char *sep=name;
        while(*sep!='\0' && *sep!='.' && *sep!=' ') sep++;
        if(*sep==' ')
        {
            if (throws)
            {
                std::stringstream ss;
                ss << "Failed to get field: " << fullName
                   << " (No spaces allowed in field name)";
                throw std::runtime_error(ss.str());
            }
            else
                return NULL;
        }
        size_t N = sep-name;
        if(N==0)
        {
            if (throws)
            {
                std::stringstream ss;
                ss << "Failed to get field: " << fullName
                   << " (Zero-length field name encountered)";
                throw std::runtime_error(ss.str());
            }
            else
                return NULL;
        }

        const PVFieldPtrArray& pvFields = parent->getPVFields();

        PVField *child = NULL;

        for(size_t i=0, n=pvFields.size(); i!=n; i++) {
            const PVFieldPtr& fld = pvFields[i];
            const std::string& fname = fld->getFieldName();

            if(fname.size()==N && memcmp(name, fname.c_str(), N)==0) {
                child = fld.get();
                break;
            }
        }

        if(!child)
        {
            if (throws)
            {
                std::stringstream ss;
                ss << "Failed to get field: " << fullName << " ("
                   << std::string(fullName, sep) << " not found)";
                throw std::runtime_error(ss.str());   
            }
            else
                return NULL;
        }

        if(*sep) {
            // this is not the requested leaf
            parent = dynamic_cast<PVStructure*>(child);
            if(!parent)
            {
                if (throws)
                {
                    std::stringstream ss;
                    ss << "Failed to get field: " << fullName
                       << " (" << std::string(fullName, sep)
                       <<  " is not a structure)"; 
                    throw std::runtime_error(ss.str());
                }
                else
                    return NULL;
            }
            child = NULL;
            name = sep+1; // skip past '.'
            // loop around to new parent

        } else {
            return child;
        }
    }
}


void PVStructure::serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher) const {
    size_t fieldsSize = pvFields.size();
    for(size_t i = 0; i<fieldsSize; i++)
        pvFields[i]->serialize(pbuffer, pflusher);
}

void PVStructure::deserialize(ByteBuffer *pbuffer,
        DeserializableControl *pcontrol) {
    size_t fieldsSize = pvFields.size();
    for(size_t i = 0; i<fieldsSize; i++)
        pvFields[i]->deserialize(pbuffer, pcontrol);

}

void PVStructure::serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher, BitSet *pbitSet) const {
    size_t numberFields = this->getNumberFields();
    size_t offset = this->getFieldOffset();
    int32 next = pbitSet->nextSetBit(static_cast<uint32>(offset));

    // no more changes or no changes in this structure
    if(next<0||next>=static_cast<int32>(offset+numberFields)) return;

    // entire structure
    if(static_cast<int32>(offset)==next) {
        serialize(pbuffer, pflusher);
        return;
    }

    size_t fieldsSize = pvFields.size();
    for(size_t i = 0; i<fieldsSize; i++) {
        PVFieldPtr pvField = pvFields[i];
        offset = pvField->getFieldOffset();
        int32 inumberFields = static_cast<int32>(pvField->getNumberFields());
        next = pbitSet->nextSetBit(static_cast<uint32>(offset));

        // no more changes
        if(next<0) return;
        //  no change in this pvField
        if(next>=static_cast<int32>(offset+inumberFields)) continue;

        // serialize field or fields
        if(inumberFields==1) {
            pvField->serialize(pbuffer, pflusher);
        } else {
            PVStructurePtr pvStructure = std::tr1::static_pointer_cast<PVStructure>(pvField);
            pvStructure->serialize(pbuffer, pflusher, pbitSet);
       }
    }
}

void PVStructure::deserialize(ByteBuffer *pbuffer,
        DeserializableControl *pcontrol, BitSet *pbitSet) {
    size_t offset = getFieldOffset();
    size_t numberFields = getNumberFields();
    int32 next = pbitSet->nextSetBit(static_cast<uint32>(offset));

    // no more changes or no changes in this structure
    if(next<0||next>=static_cast<int32>(offset+numberFields)) return;

    // entire structure
    if(static_cast<int32>(offset)==next) {
        deserialize(pbuffer, pcontrol);
        return;
    }

    size_t fieldsSize = pvFields.size();
    for(size_t i = 0; i<fieldsSize; i++) {
        PVFieldPtr pvField = pvFields[i];
        offset = pvField->getFieldOffset();
        int32 inumberFields = static_cast<int32>(pvField->getNumberFields());
        next = pbitSet->nextSetBit(static_cast<uint32>(offset));
        // no more changes
        if(next<0) return;
        //  no change in this pvField
        if(next>=static_cast<int32>(offset+inumberFields)) continue;

        // deserialize field or fields
        if(inumberFields==1) {
            pvField->deserialize(pbuffer, pcontrol);
        } else {
            PVStructurePtr pvStructure = std::tr1::static_pointer_cast<PVStructure>(pvField);
            pvStructure->deserialize(pbuffer, pcontrol, pbitSet);
        }
    }
}

std::ostream& PVStructure::dumpValue(std::ostream& o) const
{
    o << format::indent() << getStructure()->getID() << ' ' << getFieldName();
    o << std::endl;
    {
    	format::indent_scope s(o);

		PVFieldPtrArray const & fieldsData = getPVFields();
		if (fieldsData.size() != 0) {
			size_t length = getStructure()->getNumberFields();
			for(size_t i=0; i<length; i++) {
				PVFieldPtr fieldField = fieldsData[i];
				Type type = fieldField->getField()->getType();
				if (type == scalar || type == scalarArray)
					o << format::indent() << fieldField->getField()->getID() << ' ' << fieldField->getFieldName() << ' ' << *(fieldField.get()) << std::endl;
				else
					o << *(fieldField.get());
			}
		}
    }
 	return o;
}


void PVStructure::copy(const PVStructure& from)
{
    if(isImmutable())
        throw std::invalid_argument("destination is immutable");

    if(*getStructure() != *from.getStructure())
        throw std::invalid_argument("structure definitions do not match");

    copyUnchecked(from);
}

void PVStructure::copyUnchecked(const PVStructure& from)
{
    if (this == &from)
        return;

    PVFieldPtrArray const & fromPVFields = from.getPVFields();
    PVFieldPtrArray const & toPVFields = getPVFields();

    size_t fieldsSize = fromPVFields.size();
    for(size_t i = 0; i<fieldsSize; i++) {
        toPVFields[i]->copyUnchecked(*fromPVFields[i]);
    }
}

void PVStructure::copyUnchecked(const PVStructure& from, const BitSet& maskBitSet, bool inverse)
{
    if (this == &from)
        return;

    size_t numberFields = from.getNumberFields();
    size_t offset = from.getFieldOffset();
    int32 next = inverse ?
                maskBitSet.nextClearBit(static_cast<uint32>(offset)) :
                maskBitSet.nextSetBit(static_cast<uint32>(offset));

    // no more changes or no changes in this structure
    if(next<0||next>=static_cast<int32>(offset+numberFields)) return;

    // entire structure
    if(static_cast<int32>(offset)==next) {
        copyUnchecked(from);
        return;
    }

    PVFieldPtrArray const & fromPVFields = from.getPVFields();
    PVFieldPtrArray const & toPVFields = getPVFields();

    size_t fieldsSize = fromPVFields.size();
    for(size_t i = 0; i<fieldsSize; i++) {
        PVFieldPtr pvField = fromPVFields[i];
        offset = pvField->getFieldOffset();
        int32 inumberFields = static_cast<int32>(pvField->getNumberFields());
        next = inverse ?
                    maskBitSet.nextClearBit(static_cast<uint32>(offset)) :
                    maskBitSet.nextSetBit(static_cast<uint32>(offset));

        // no more changes
        if(next<0) return;
        //  no change in this pvField
        if(next>=static_cast<int32>(offset+inumberFields)) continue;

        // serialize field or fields
        if(inumberFields==1) {
            toPVFields[i]->copyUnchecked(*pvField);
        } else {
            PVStructure::shared_pointer fromPVStructure = std::tr1::static_pointer_cast<PVStructure>(pvField);
            PVStructure::shared_pointer toPVStructure = std::tr1::static_pointer_cast<PVStructure>(toPVFields[i]);
            toPVStructure->copyUnchecked(*fromPVStructure, maskBitSet, inverse);
       }
    }
}


}}
