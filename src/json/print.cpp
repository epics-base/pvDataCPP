/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */

#include <vector>
#include <sstream>

#define epicsExportSharedSymbols
#include <pv/pvdVersion.h>
#include <pv/pvData.h>
#include <pv/valueBuilder.h>
#include <pv/bitSet.h>
#include "pv/json.h"

namespace pvd = epics::pvData;

namespace {

struct args {
    std::ostream& strm;
    const pvd::JSONPrintOptions& opts;

    unsigned indent;

    args(std::ostream& strm,
         const pvd::JSONPrintOptions& opts)
        :strm(strm)
        ,opts(opts)
        ,indent(opts.indent)
    {}

    void doIntent() {
        if(!opts.multiLine) return;
        strm.put('\n');
        unsigned i=indent;
        while(i--) strm.put(' ');
    }
};

void show_field(args& A, const pvd::PVField* fld, const pvd::BitSet *mask);

void show_struct(args& A, const pvd::PVStructure* fld, const pvd::BitSet *mask)
{
    const pvd::StructureConstPtr& type = fld->getStructure();
    const pvd::PVFieldPtrArray& children = fld->getPVFields();

    const pvd::StringArray& names = type->getFieldNames();

    A.strm.put('{');
    A.indent++;

    bool first = true;
    for(size_t i=0, N=names.size(); i<N; i++)
    {
        if(mask && !mask->get(children[i]->getFieldOffset())) continue;

        if(first)
            first = false;
        else
            A.strm.put(',');
        A.doIntent();
        A.strm<<'\"'<<names[i]<<"\": ";
        show_field(A, children[i].get(), mask);
    }

    A.indent--;
    A.doIntent();
    A.strm.put('}');
}

void show_field(args& A, const pvd::PVField* fld, const pvd::BitSet *mask)
{
    switch(fld->getField()->getType())
    {
    case pvd::scalar:
    {
        const pvd::PVScalar *scalar=static_cast<const pvd::PVScalar*>(fld);
        if(scalar->getScalar()->getScalarType()==pvd::pvString) {
            A.strm<<'\"'<<scalar->getAs<std::string>()<<'\"';
        } else {
            A.strm<<scalar->getAs<std::string>();
        }
    }
        return;
    case pvd::scalarArray:
    {
        const pvd::PVScalarArray *scalar=static_cast<const pvd::PVScalarArray*>(fld);
        const bool isstring = scalar->getScalarArray()->getElementType()==pvd::pvString;

        pvd::shared_vector<const void> arr;
        scalar->getAs<void>(arr);

        pvd::shared_vector<const std::string> sarr(pvd::shared_vector_convert<const std::string>(arr));

        A.strm.put('[');
        for(size_t i=0, N=sarr.size(); i<N; i++) {
            if(i!=0)
                A.strm.put(',');
            if(isstring)
                A.strm.put('\"');
            A.strm<<sarr[i];
            if(isstring)
                A.strm.put('\"');
        }
        A.strm.put(']');
    }
        return;
    case pvd::structure:
        show_struct(A, static_cast<const pvd::PVStructure*>(fld), mask);
        return;
    case pvd::structureArray:
    {
        pvd::PVStructureArray::const_svector arr(static_cast<const pvd::PVStructureArray*>(fld)->view());
        A.strm.put('[');
        A.indent++;

        for(size_t i=0, N=arr.size(); i<N; i++) {
            if(i!=0)
                A.strm.put(',');
            A.doIntent();
            if(arr[i])
                show_struct(A, arr[i].get(), 0);
            else
                A.strm<<"NULL";
        }

        A.indent--;
        A.doIntent();
        A.strm.put(']');
    }
        return;
    case pvd::union_:
    {
        const pvd::PVUnion *U=static_cast<const pvd::PVUnion*>(fld);
        const pvd::PVField::const_shared_pointer& C(U->get());

        if(!C) {
            A.strm<<"null";
        } else {
            show_field(A, C.get(), 0);
        }
    }
        return;
    case pvd::unionArray: {
        const pvd::PVUnionArray *U=static_cast<const pvd::PVUnionArray*>(fld);
        pvd::PVUnionArray::const_svector arr(U->view());
        A.strm.put('[');
        A.indent++;

        for(size_t i=0, N=arr.size(); i<N; i++) {
            if(i!=0)
                A.strm.put(',');
            A.doIntent();
            if(arr[i])
                show_field(A, arr[i].get(), 0);
            else
                A.strm<<"NULL";
        }

        A.indent--;
        A.doIntent();
        A.strm.put(']');

    }
        return;
    }
    // should not be reached
    if(A.opts.ignoreUnprintable)
        A.strm<<"// unprintable field type";
    else
        throw std::runtime_error("Encountered unprintable field type");
}

void expandBS(const pvd::PVStructure& top, pvd::BitSet& mask, bool parents) {
    if(mask.get(0)) { // special handling because getSubField(0) not allowed
        // wildcard
        for(size_t idx=1, N=top.getNumberFields(); idx<N; idx++) {
            mask.set(idx);
        }

    } else {
        for(pvd::int32 idx = mask.nextSetBit(0), N=top.getNumberFields(); idx>=0 && idx<N; idx=mask.nextSetBit(idx+1)) {
            pvd::PVField::const_shared_pointer fld = top.getSubFieldT(idx);

            // look forward and mark all children
            for(size_t i=idx+1, N=fld->getNextFieldOffset(); i<N; i++)
                mask.set(i);

            if(parents) {
                // look back and mark all parents
                // we've already stepped past all parents so siblings will not be automatically marked
                for(const pvd::PVStructure *parent = fld->getParent(); parent; parent = parent->getParent()) {
                    mask.set(parent->getFieldOffset());
                }
            }
        }
    }
}

} // namespace

namespace epics{namespace pvData{

JSONPrintOptions::JSONPrintOptions()
    :multiLine(true)
    ,ignoreUnprintable(true)
    ,indent(0)
{}

void printJSON(std::ostream& strm,
               const PVStructure& val,
               const BitSet& mask,
               const JSONPrintOptions& opts)
{
    args A(strm, opts);
    pvd::BitSet emask(mask);
    expandBS(val, emask, true);
    if(!emask.get(0)) return;
    show_struct(A, &val, &emask);
}

void printJSON(std::ostream& strm,
               const PVField& val,
               const JSONPrintOptions& opts)
{
    args A(strm, opts);
    show_field(A, &val, 0);
}

}} // namespace epics::pvData
