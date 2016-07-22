/* pvDisplay.cpp */
/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
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
#include <pv/pvDisplay.h>

using std::tr1::static_pointer_cast;
using std::string;

namespace epics { namespace pvData { 

string PVDisplay::noDisplayFound("No display structure found");
string PVDisplay::notAttached("Not attached to an display structure");

bool PVDisplay::attach(PVFieldPtr const & pvField)
{
    if(pvField->getField()->getType()!=structure) return false;
    PVStructurePtr pvStructure = static_pointer_cast<PVStructure>(pvField);
    pvDescription = pvStructure->getSubField<PVString>("description");
    if(pvDescription.get()==NULL) return false;
    pvFormat = pvStructure->getSubField<PVString>("format");
    if(pvFormat.get()==NULL) {
        detach();
        return false;
    }
    pvUnits = pvStructure->getSubField<PVString>("units");
    if(pvUnits.get()==NULL) {
        detach();
        return false;
    }
    pvLow = pvStructure->getSubField<PVDouble>(string("limitLow"));
    if(pvLow.get()==NULL) {
        detach();
        return false;
    }
    pvHigh = pvStructure->getSubField<PVDouble>(string("limitHigh"));
    if(pvHigh.get()==NULL) {
        detach();
        return false;
    }
    return true;
}

void PVDisplay::detach()
{
    pvDescription.reset();
    pvFormat.reset();
    pvUnits.reset();
    pvLow.reset();
    pvHigh.reset();
}

bool PVDisplay::isAttached() {
    if(pvDescription.get()) return false;
    return true;
}

void PVDisplay::get(Display & display) const
{
    if(pvDescription.get()==NULL) {
        throw std::logic_error(notAttached);
    }
    display.setDescription(pvDescription->get());
    display.setFormat(pvFormat->get());
    display.setUnits(pvUnits->get());
    display.setLow(pvLow->get());
    display.setHigh(pvHigh->get());
}

bool PVDisplay::set(Display const & display)
{
    if(pvDescription.get()==NULL) {
        throw std::logic_error(notAttached);
    }
    if(pvDescription->isImmutable() || pvFormat->isImmutable()) return false;
    if(pvUnits->isImmutable() || pvLow->isImmutable() || pvHigh->isImmutable())
         return false;
    pvDescription->put(display.getDescription());
    pvFormat->put(display.getFormat());
    pvUnits->put(display.getUnits());
    pvLow->put(display.getLow());
    pvHigh->put(display.getHigh());
    return true;
}

}}
