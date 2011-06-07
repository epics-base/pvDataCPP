/* pvDisplay.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <string>
#include <stdexcept>
#include <pv/pvType.h>
#include <pv/pvIntrospect.h>
#include <pv/pvData.h>
#include <pv/pvDisplay.h>
namespace epics { namespace pvData { 

static String noDisplayFound("No display structure found");
static String notAttached("Not attached to an display structure");

bool PVDisplay::attach(PVField *pvField)
{
    PVStructure *pvStructure = 0;
    if(pvField->getField()->getFieldName().compare("display")!=0) {
        if(pvField->getField()->getFieldName().compare("value")!=0) {
            pvField->message(noDisplayFound,errorMessage);
            return false;
        }
        PVStructure *pvParent = pvField->getParent();
        if(pvParent==0) {
            pvField->message(noDisplayFound,errorMessage);
            return false;
        }
        pvStructure = pvParent->getStructureField(String("display"));
        if(pvStructure==0) {
            pvField->message(noDisplayFound,errorMessage);
            return false;
        }
    } else {
        if(pvField->getField()->getType()!=structure) {
            pvField->message(noDisplayFound,errorMessage);
            return false;
        }
        pvStructure = static_cast<PVStructure*>(pvField);
    }
    pvDescription = pvStructure->getStringField(String("description"));
    if(pvDescription==0) {
        pvField->message(noDisplayFound,errorMessage);
        return false;
    }
    pvFormat = pvStructure->getStringField(String("format"));
    if(pvFormat==0) {
        pvField->message(noDisplayFound,errorMessage);
        detach();
        return false;
    }
    pvUnits = pvStructure->getStringField(String("units"));
    if(pvUnits==0) {
        pvField->message(noDisplayFound,errorMessage);
        detach();
        return false;
    }
    pvLow = pvStructure->getDoubleField(String("limit.low"));
    if(pvLow==0) {
        pvField->message(noDisplayFound,errorMessage);
        detach();
        return false;
    }
    pvHigh = pvStructure->getDoubleField(String("limit.high"));
    if(pvHigh==0) {
        pvField->message(noDisplayFound,errorMessage);
        detach();
        return false;
    }
    return true;
}

void PVDisplay::detach()
{
    pvDescription = 0;
    pvFormat = 0;
    pvUnits = 0;
    pvLow = 0;
    pvHigh = 0;
}

bool PVDisplay::isAttached() {
    if(pvDescription==0 || pvFormat==0 || pvUnits==0 || pvLow==0 || pvHigh==0)
        return false;
    return true;
}

void PVDisplay::get(Display & display) const
{
    if(pvDescription==0 || pvFormat==0 || pvUnits==0 || pvLow==0 || pvHigh==0) {
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
    if(pvDescription==0 || pvFormat==0 || pvUnits==0 || pvLow==0 || pvHigh==0) {
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
