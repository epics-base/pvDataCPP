/*PVField.cpp*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>

#define epicsExportSharedSymbols
#include <pv/lock.h>
#include <pv/pvData.h>
#include <pv/factory.h>
#include <pv/convert.h>

using std::tr1::const_pointer_cast;
using std::size_t;

namespace epics { namespace pvData {


PVField::PVField(FieldConstPtr field)
: notImplemented("not implemented"),
  parent(NULL),field(field),
  fieldOffset(0), nextFieldOffset(0),
  immutable(false)
{
}

PVField::~PVField()
{ }


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


bool PVField::isImmutable() const {return immutable;}

void PVField::setImmutable() {immutable = true;}

const FieldConstPtr & PVField::getField() const {return field;}

PVStructure *PVField::getParent() const {return parent;}

void PVField::postPut() 
{
   if(postHandler.get()!=NULL) postHandler->postPut();
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

void PVField::setParentAndName(PVStructure * xxx,String const & name)
{
    parent = xxx;
    fieldName = name;
}

bool PVField::equals(PVField &pv)
{
    return pv==*this;
}

void PVField::toString(StringBuilder buf)
{
    toString(buf,0);
}

void PVField::toString(StringBuilder buf,int indentLevel) 
{
   Convert().getString(buf,this,indentLevel);
}

std::ostream& operator<<(std::ostream& o, const PVField& f)
{
	std::ostream& ro = f.dumpValue(o);
	return ro;
};

namespace format
{
	std::ostream& operator<<(std::ostream& os, indent_level const& indent)
	{
		indent_value(os) = indent.level;
		return os;
	}

	std::ostream& operator<<(std::ostream& os, indent const&)
	{
		long il = indent_value(os);
		std::size_t spaces = static_cast<std::size_t>(il) * 4;
		return os << std::string(spaces, ' ');
	}

	array_at_internal operator<<(std::ostream& str, array_at const& manip)
	{
		return array_at_internal(manip.index, str);
	}
};

String PVField::getFullName() const
{
    String ret(fieldName);
    for(PVField *fld=getParent(); fld; fld=fld->getParent())
    {
        if(fld->getFieldName().size()==0) break;
        ret = fld->getFieldName() + '.' + ret;
    }
    return ret;
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
    size_t offset = 0;
    size_t nextOffset = 1;
    PVFieldPtrArray  pvFields = pvTop->getPVFields();
    for(size_t i=0; i < pvTop->getStructure()->getNumberFields(); i++) {
        offset = nextOffset;
        PVField *pvField = pvFields[i].get();
        FieldConstPtr field = pvField->getField();
        switch(field->getType()) {
        case scalar:
        case scalarArray:
        case structureArray:
        case union_:
        case unionArray: {
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
    size_t beginOffset = offset;
    size_t nextOffset = offset + 1;
    const PVStructure *pvStructure = static_cast<const PVStructure *>(pvField);
    const PVFieldPtrArray  pvFields = pvStructure->getPVFields();
    for(size_t i=0; i < pvStructure->getStructure()->getNumberFields(); i++) {
        offset = nextOffset;
        PVField *pvSubField = pvFields[i].get();
        FieldConstPtr field = pvSubField->getField();
        switch(field->getType()) {
            case scalar:
            case scalarArray:
            case structureArray:
            case union_:
            case unionArray: {
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
