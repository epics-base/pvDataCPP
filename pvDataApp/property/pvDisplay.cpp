/* pvDisplay.cpp */
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
#include <pv/pvType.h>
#include <pv/pvIntrospect.h>
#include <pv/pvData.h>
#include <pv/pvDisplay.h>
namespace epics { namespace pvData { 

using std::tr1::static_pointer_cast;

String PVDisplay::noDisplayFound("No display structure found");
String PVDisplay::notAttached("Not attached to an display structure");

bool PVDisplay::attach(PVFieldPtr const & pvField)
{
    if(pvField->getField()->getType()!=structure) {
            pvField->message(noDisplayFound,errorMessage);
            return false;
    }
    PVStructurePtr pvStructure = static_pointer_cast<PVStructure>(pvField);
    pvDescription = pvStructure->getStringField("description");
    if(pvDescription.get()==NULL) {
        pvField->message(noDisplayFound,errorMessage);
        return false;
    }
    pvFormat = pvStructure->getStringField("format");
    if(pvFormat.get()==NULL) {
        pvField->message(noDisplayFound,errorMessage);
        detach();
        return false;
    }
    pvUnits = pvStructure->getStringField("units");
    if(pvUnits.get()==NULL) {
        pvField->message(noDisplayFound,errorMessage);
        detach();
        return false;
    }
    pvLow = pvStructure->getDoubleField(String("limitLow"));
    if(pvLow.get()==NULL) {
        pvField->message(noDisplayFound,errorMessage);
        detach();
        return false;
    }
    pvHigh = pvStructure->getDoubleField(String("limitHigh"));
    if(pvHigh.get()==NULL) {
        pvField->message(noDisplayFound,errorMessage);
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
