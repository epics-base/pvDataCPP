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

#if EPICS_VERSION_INT>=VERSION_INT(3,15,0,1)

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

void show_field(args& A, const pvd::PVField* fld);

void show_struct(args& A, const pvd::PVStructure* fld)
{
    const pvd::StructureConstPtr& type = fld->getStructure();
    const pvd::PVFieldPtrArray& children = fld->getPVFields();

    const pvd::StringArray& names = type->getFieldNames();

    A.strm.put('{');
    A.indent++;

    for(size_t i=0, N=names.size(); i<N; i++)
    {
        if(i!=0)
            A.strm.put(',');
        A.doIntent();
        A.strm<<'\"'<<names[i]<<"\": ";
        show_field(A, children[i].get());
    }

    A.indent--;
    A.doIntent();
    A.strm.put('}');
}

void show_field(args& A, const pvd::PVField* fld)
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
        break;
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
        break;
    case pvd::structure:
        show_struct(A, static_cast<const pvd::PVStructure*>(fld));
        break;
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
                show_struct(A, arr[i].get());
            else
                A.strm<<"NULL";
        }

        A.indent--;
        A.doIntent();
        A.strm.put(']');
    }
        break;
    case pvd::union_:
    {
        const pvd::PVUnion *U=static_cast<const pvd::PVUnion*>(fld);
        const pvd::PVField::const_shared_pointer& C(U->get());

        if(!C) {
            A.strm<<"null";
        } else {
            show_field(A, C.get());
        }
    }
        break;
    default:
        if(A.opts.ignoreUnprintable)
            A.strm<<"// unprintable field type";
        else
            throw std::runtime_error("Encountered unprintable field type");
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
               const PVField::const_shared_pointer& val,
               const JSONPrintOptions& opts)
{
    args A(strm, opts);
    show_field(A, val.get());
}

}} // namespace epics::pvData

#endif // EPICS_VERSION_INT
