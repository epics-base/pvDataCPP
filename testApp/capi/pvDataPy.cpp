/* pvDataPy.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#include <pv/pvDataPy.h>

using std::tr1::static_pointer_cast;
using std::size_t;

namespace epics { namespace pvData { 

PVTopPyPtr PVTopPy::createTop(PVStructurePtr const & pvStructure)
{
   PVStructurePyPtr top = PVStructurePyPtr(create(pvStructure));
   return PVTopPyPtr(new PVTopPy(top));
    
}

PVStructurePyPtr PVTopPy::create(PVStructurePtr const & pvStructure)
{
    PVStructurePyPtr pvStructurePyPtr =
       PVStructurePyPtr(new PVStructurePy(pvStructure));
    const PVFieldPtrArray & pvFields = pvStructure->getPVFields();
    size_t n = pvFields.size();
    PVFieldPyPtrArray & pvPyFields = pvStructurePyPtr->pvPyFields;
    pvPyFields.reserve(n);
    for(size_t i = 0; i<n; i++) {
        PVFieldPtr pvField = pvFields[i];
        Type type = pvField->getField()->getType();
        PVFieldPyPtr pvFieldPy;
        switch(type) {
        case scalar : 
            {
                PVScalarPtr xxx = static_pointer_cast<PVScalar>(pvField);
                pvFieldPy = PVScalarPyPtr(new PVScalarPy(xxx));
            }
            break;
        case scalarArray :
            {
                PVScalarArrayPtr xxx = static_pointer_cast<PVScalarArray>(pvField);
                pvFieldPy = PVScalarArrayPyPtr(new PVScalarArrayPy(xxx));
            }
            break;
        case structureArray : 
            {
                PVStructureArrayPtr xxx = static_pointer_cast<PVStructureArray>(pvField);
                pvFieldPy = PVStructureArrayPyPtr(new PVStructureArrayPy(xxx));
            }
            break;
        case structure :
            {
                PVStructurePtr xxx = static_pointer_cast<PVStructure>(pvField);
                pvFieldPy = PVStructurePyPtr(create(xxx));
            }
            break;
        }
        pvPyFields.push_back(pvFieldPy);
    }
    return pvStructurePyPtr;
}

}}
