/* pvControl.cpp */
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
#include <pv/pvControl.h>
namespace epics { namespace pvData { 

static String noControlFound("No control structure found");
static String notAttached("Not attached to an control structure");

bool PVControl::attach(PVField *pvField)
{
    PVStructure *pvStructure = 0;
    if(pvField->getField()->getFieldName().compare("control")!=0) {
        if(pvField->getField()->getFieldName().compare("value")!=0) {
            pvField->message(noControlFound,errorMessage);
            return false;
        }
        PVStructure *pvParent = pvField->getParent();
        if(pvParent==0) {
            pvField->message(noControlFound,errorMessage);
            return false;
        }
        pvStructure = pvParent->getStructureField(String("control"));
        if(pvStructure==0) {
            pvField->message(noControlFound,errorMessage);
            return false;
        }
    } else {
        if(pvField->getField()->getType()!=structure) {
            pvField->message(noControlFound,errorMessage);
            return false;
        }
        pvStructure = static_cast<PVStructure*>(pvField);
    }
    PVDouble *pvDouble = pvStructure->getDoubleField(String("limit.low"));
    if(pvDouble==0) {
        pvField->message(noControlFound,errorMessage);
        return false;
    }
    pvLow = pvDouble;
    pvDouble = pvStructure->getDoubleField(String("limit.high"));
    if(pvDouble==0) {
        pvLow = 0;
        pvField->message(noControlFound,errorMessage);
        return false;
    }
    pvHigh = pvDouble;
    return true;
}

void PVControl::detach()
{
    pvLow = 0;
    pvHigh = 0;
}

bool PVControl::isAttached(){
    if(pvLow==0 || pvHigh==0) return false;
    return true;
}

void PVControl::get(Control &control) const
{
    if(pvLow==0 || pvHigh==0) {
        throw std::logic_error(notAttached);
    }
    control.setLow(pvLow->get());
    control.setHigh(pvHigh->get());
}

bool PVControl::set(Control const & control)
{
    if(pvLow==0 || pvHigh==0) {
        throw std::logic_error(notAttached);
    }
    if(pvLow->isImmutable() || pvHigh->isImmutable()) return false;
    pvLow->put(control.getLow());
    pvHigh->put(control.getHigh());
    return true;
}

}}
