/* printer.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
 
#include <deque>

#define epicsExportSharedSymbols
#include <pv/printer.h>

using std::string;

namespace {

void indentN(std::ostream& strm, size_t N)
{
    while(N--)
        strm.put(' ');
}

}

namespace epics { namespace pvData {

namespace format
{
    static int indent_index = std::ios_base::xalloc();

    long& indent_value(std::ios_base& ios)
    {
      return ios.iword(indent_index);
    }

    std::ostream& operator<<(std::ostream& os, indent_level const& indent)
	{
		indent_value(os) = indent.level;
		return os;
	}

	std::ostream& operator<<(std::ostream& os, indent const&)
	{
		long il = indent_value(os);
		std::size_t spaces = static_cast<std::size_t>(il) * 4;
		return os << string(spaces, ' ');
	}

	array_at_internal operator<<(std::ostream& str, array_at const& manip)
	{
		return array_at_internal(manip.index, str);
	}
};

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

void PrinterBase::beginUnion(const PVUnion&) {}
void PrinterBase::endUnion(const PVUnion&) {}

void PrinterBase::beginUnionArray(const PVUnionArray&) {}
void PrinterBase::endUnionArray(const PVUnionArray&) {}

void PrinterBase::encodeScalar(const PVScalar&) {}

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

            case union_:
                endUnion(*static_cast<const PVUnion *>(inprog.back()));
                break;

            case unionArray:
                endUnionArray(*static_cast<const PVUnionArray *>(inprog.back()));
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
            case union_: {
                    const PVUnion &fld = *static_cast<const PVUnion*>(next);

                    inprog.push_back(next);

                    beginUnion(fld);
                    PVFieldPtr val = fld.get();
                    if (val.get())          // TODO print "(none)" ?
                        todo.push_back(val.get());

                    todo.push_back(marker);
                    break;
                }
            case unionArray: {
                    const PVUnionArray &fld = *static_cast<const PVUnionArray*>(next);
                    PVUnionArray::const_svector vals(fld.view());

                    inprog.push_back(next);

                    beginUnionArray(fld);
                    for(PVUnionArray::const_svector::const_iterator it=vals.begin();
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

void PrinterPlain::beginUnion(const PVUnion& pv)
{
    indentN(S(), ilvl);
    S() << pv.getUnion()->getID() << " " << pv.getFieldName() << std::endl;
    ilvl++;
}

void PrinterPlain::endUnion(const PVUnion&) {ilvl--;}

void PrinterPlain::beginUnionArray(const PVUnionArray& pv)
{
    indentN(S(), ilvl);
    S() << pv.getUnionArray()->getID() << " "
        << pv.getFieldName() << "[] ";
    ilvl++;
}

void PrinterPlain::endUnionArray(const PVUnionArray&) {ilvl--;}

void PrinterPlain::encodeScalar(const PVScalar& pv)
{
    indentN(S(), ilvl);
    S() << pv.getScalar()->getID() << " "
        << pv.getFieldName() << " "
        << pv.getAs<string>() << std::endl;
}

void PrinterPlain::encodeArray(const PVScalarArray& pv)
{
    indentN(S(), ilvl);
    shared_vector<const string> temp;
    pv.getAs<string>(temp);

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
