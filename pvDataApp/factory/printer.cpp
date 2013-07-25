
#include <deque>

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

void PrinterBase::encodeNull() {}

void PrinterBase::impl_print(const PVField& pv)
{
    static const PVField* marker = (const PVField*)&marker;

    /* Depth first recursive iteration.
     * Each PV to be printed is appended to the todo queue.
     * The last child of a structure is followed by a NULL.
     * As the tree is walked structures and structarrays
     * are appended to the inprog queue.
     */
    std::deque<const PVField*> todo, inprog;

    todo.push_back(&pv);

    while(!todo.empty()) {
        const PVField *next = todo.front();
        todo.pop_front();

        if(next==marker) {
            // finished with a structure or structarray,
            // now we fall back to its parent.
            assert(!inprog.empty());
            switch(inprog.back()->getField()->getType()) {
            case structure:
                endStructure(*static_cast<const PVStructure *>(inprog.back()));
                break;

            case structureArray:
                endStructureArray(*static_cast<const PVStructureArray *>(inprog.back()));
                break;

            default:
                assert(false); // oops!
                return;
            }
            inprog.pop_back();

        } else {
            // real field

            if(!next) {
                // NULL entry in a structure array
                encodeNull();
                continue;
            }

            switch(next->getField()->getType()) {
            case scalar:
                encodeScalar(*static_cast<const PVScalar*>(next));
                break;
            case scalarArray:
                encodeArray(*static_cast<const PVScalarArray*>(next));
                break;
            case structure: {
                    const PVStructure &fld = *static_cast<const PVStructure*>(next);
                    const PVFieldPtrArray& vals = fld.getPVFields();

                    inprog.push_back(next);

                    beginStructure(fld);
                    for(size_t i=0, nfld=fld.getStructure()->getNumberFields(); i<nfld; i++)
                        todo.push_back(vals[i].get());

                    todo.push_back(marker);
                    break;
                }
            case structureArray: {
                    const PVStructureArray &fld = *static_cast<const PVStructureArray*>(next);
                    PVStructureArray::const_svector vals(fld.view());

                    inprog.push_back(next);

                    beginStructureArray(fld);
                    for(PVStructureArray::const_svector::const_iterator it=vals.begin();
                        it!=vals.end(); ++it)
                    {
                        todo.push_back(it->get());
                    }

                    todo.push_back(marker);
                    break;
                }
            }
        }
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
        << pv.getFieldName() << "[] ";
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
    shared_vector<const String> temp;
    pv.getAs<pvString>(temp);

    S() << pv.getScalarArray()->getID() << " "
        << pv.getFieldName() << " [";
    for(size_t i=0, len=pv.getLength(); i<len; i++) {
        S() << temp[i];
        if(i!=len-1)
            S().put(',');
    }
    S() << "]" << std::endl;
}

void PrinterPlain::encodeNull()
{
    indentN(S(), ilvl);
    S() << "NULL" << std::endl;
}

}}
