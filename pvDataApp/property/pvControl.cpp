/* pvControl.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#include <string>
#include <stdexcept>

#define epicsExportSharedSymbols
#include <pv/pvType.h>
#include <pv/pvIntrospect.h>
#include <pv/pvData.h>
#include <pv/pvControl.h>

namespace epics { namespace pvData { 

using std::tr1::static_pointer_cast;

String PVControl::noControlFound("No control structure found");
String PVControl::notAttached("Not attached to an control structure");

bool PVControl::attach(PVFieldPtr const & pvField)
{
    if(pvField->getField()->getType()!=structure) return false;
    PVStructurePtr pvStructure = static_pointer_cast<PVStructure>(pvField);
    pvLow = pvStructure->getDoubleField("limitLow");
    if(pvLow.get()==NULL) return false;
    pvHigh = pvStructure->getDoubleField(String("limitHigh"));
    if(pvHigh.get()==NULL) {
        pvLow.reset();
        return false;
    }
    return true;
}

void PVControl::detach()
{
    pvLow.reset();
    pvHigh.reset();
}

bool PVControl::isAttached(){
    if(pvLow.get()==NULL) return false;
    return true;
}

void PVControl::get(Control &control) const
{
    if(pvLow.get()==NULL) {
        throw std::logic_error(notAttached);
    }
    control.setLow(pvLow->get());
    control.setHigh(pvHigh->get());
}

bool PVControl::set(Control const & control)
{
    if(pvLow.get()==NULL) {
        throw std::logic_error(notAttached);
    }
    if(pvLow->isImmutable() || pvHigh->isImmutable()) return false;
    pvLow->put(control.getLow());
    pvHigh->put(control.getHigh());
    return true;
}

}}
