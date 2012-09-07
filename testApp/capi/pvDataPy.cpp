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

PVFieldPy::PVFieldPy(Type type)
: type(type){}

PVScalarPy::PVScalarPy(PVScalarPtr const & pvScalarPtr)
    : PVFieldPy(pvScalarPtr->getField()->getType()),
      pvScalarPtr(pvScalarPtr)
      {}

PVScalarArrayPy::PVScalarArrayPy(PVScalarArrayPtr const & pvScalarArrayPtr)
    : PVFieldPy(pvScalarArrayPtr->getField()->getType()),
      pvScalarArrayPtr(pvScalarArrayPtr)
      {}

PVStructureArrayPy::PVStructureArrayPy(PVStructureArrayPtr const & pvStructureArrayPtr)
    : PVFieldPy(pvStructureArrayPtr->getField()->getType()),
      pvStructureArrayPtr(pvStructureArrayPtr)
      {}

PVStructurePy:: PVStructurePy(PVStructurePtr const &pvStructurePtr)
       : PVFieldPy(structure),
         pvStructurePtr(pvStructurePtr)
         {}
 
PVTopPy::PVTopPy(PVStructurePyPtr const &pvStructurePyPtr)
    : pvStructurePyPtr(pvStructurePyPtr)
      {}


PVTopPyPtr PVTopPy::createTop(PVStructurePtr const & pvStructure)
{
   PVStructurePyPtr top = PVStructurePyPtr(create(pvStructure));
   PVTopPyPtr pvTopPyPtr(new PVTopPy(top));
   return pvTopPyPtr;
    
}

PVStructurePyPtr PVTopPy::create(PVStructurePtr const & pvStructurePtr)
{
    PVStructurePyPtr pvStructurePyPtr =
       PVStructurePyPtr(new PVStructurePy(pvStructurePtr));
    const PVFieldPtrArray & pvFields = pvStructurePtr->getPVFields();
    size_t n = pvFields.size();
    PVFieldPyPtrArrayPtr pvPyFields(new PVFieldPyPtrArray());
    pvPyFields->reserve(n);
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
        pvPyFields->push_back(pvFieldPy);
    }
    pvStructurePyPtr->pvFieldPyPtrArrayPtr = pvPyFields;
    return pvStructurePyPtr;
}

}}
