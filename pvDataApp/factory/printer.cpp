
#include "pv/printer.h"

namespace {

void indentN(std::ostream& strm, size_t N)
{
    while(N--)
        strm.put(' ');
}

}

namespace epics { namespace pvData {

PrinterBase::PrinterBase()
    :strm(NULL)
{}

PrinterBase::~PrinterBase() {}

void PrinterBase::setStream(std::ostream& s)
{
    strm = &s;
}

void PrinterBase::clearStream()
{
    strm = NULL;
}

void PrinterBase::print(const PVField& pv)
{
    if(!strm)
        throw std::runtime_error("No stream set for PV Printer");
    impl_print(pv);
}

void PrinterBase::beginStructure(const PVStructure&) {}
void PrinterBase::endStructure(const PVStructure&) {}

void PrinterBase::beginStructureArray(const PVStructureArray&) {}
void PrinterBase::endStructureArray(const PVStructureArray&) {}

void PrinterBase::encodeScalar(const PVScalar& pv) {}

void PrinterBase::encodeArray(const PVScalarArray&) {}

void PrinterBase::abortField(const PVField&) {}

void PrinterBase::impl_print(const PVField& pv)
{
    try {
        switch(pv.getField()->getType()) {
        case scalar: encodeScalar(static_cast<const PVScalar&>(pv)); return;
        case scalarArray: encodeArray(static_cast<const PVScalarArray&>(pv)); return;
        case structure: {
                const PVStructure &fld = static_cast<const PVStructure&>(pv);
                const PVFieldPtrArray& vals = fld.getPVFields();

                beginStructure(fld);
                for(size_t i=0, nfld=fld.getStructure()->getNumberFields(); i<nfld; i++)
                    this->print(*vals[i]);
                endStructure(fld);
            }
        case structureArray: {
                const PVStructureArray &fld = static_cast<const PVStructureArray&>(pv);
                const PVStructureArray::pointer vals = fld.get();

                beginStructureArray(fld);
                for(size_t i=0, nfld=fld.getLength(); i<nfld; i++)
                    this->print(*vals[i]);
                endStructureArray(fld);
            }
        }
    } catch(...) {
        abortField(pv);
        throw;
    }
}


PrinterPlain::PrinterPlain()
    :PrinterBase()
    ,ilvl(0)
{}

PrinterPlain::~PrinterPlain() {}

void PrinterPlain::beginStructure(const PVStructure& pv)
{
    indentN(S(), ilvl);
    S() << pv.getStructure()->getID() << " " << pv.getFieldName();
    String ename(pv.getExtendsStructureName());
    if(!ename.empty())
        S() << " extends " << ename;
    S() << std::endl;
    ilvl++;
}

void PrinterPlain::endStructure(const PVStructure&) {ilvl--;}

void PrinterPlain::beginStructureArray(const PVStructureArray& pv)
{
    indentN(S(), ilvl);
    S() << pv.getStructureArray()->getID() << " "
        << pv.getFieldName() << " ";
    ilvl++;
}

void PrinterPlain::endStructureArray(const PVStructureArray&) {ilvl--;}

void PrinterPlain::encodeScalar(const PVScalar& pv)
{
    indentN(S(), ilvl);
    S() << pv.getScalar()->getID() << " "
        << pv.getFieldName() << " "
        << pv.getAs<pvString>() << std::endl;
}

void PrinterPlain::encodeArray(const PVScalarArray& pv)
{
    indentN(S(), ilvl);
    StringArray temp(pv.getLength()); // TODO: no copy
    pv.getAs<pvString>(&temp[0], temp.size());

    S() << pv.getScalarArray()->getID() << " "
        << pv.getFieldName() << " [";
    for(size_t i=0, len=pv.getLength(); i<len; i++) {
        S() << temp[i];
        if(i!=len-1)
            S().put(',');
    }
    S() << "]" << std::endl;
}


}}
