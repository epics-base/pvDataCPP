/* pvTimeStamp.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <string>
#include <stdexcept>
#include "pvType.h"
#include "timeStamp.h"
#include "pvData.h"
#include "pvTimeStamp.h"
namespace epics { namespace pvData { 

static String noTimeStamp("No timeStamp structure found");
static String notAttached("Not attached to a timeStamp structure");

bool PVTimeStamp::attach(PVField *pvField)
{
    PVStructure *pvStructure = 0;
    if(pvField->getField()->getFieldName().compare("timeStamp")!=0) {
        PVStructure *pvParent = pvField->getParent();
        if(pvParent==0) {
            pvField->message(noTimeStamp,errorMessage);
            return false;
        }
        if(pvField->getField()->getFieldName().compare("value")!=0) {
            pvField->message(noTimeStamp,errorMessage);
            return false;
        }
        // look up the tree for a timeSyamp
        while(pvParent!=0) {
            PVStructure *pvs = pvParent->getStructureField(String("timeStamp"));
            if(pvs!=0) {
                pvStructure = pvs;
                break;
            }
            pvParent = pvParent->getParent();
        }
        if(pvStructure==0) {
            pvField->message(noTimeStamp,errorMessage);
            return false;
        }
    } else {
        if(pvField->getField()->getType()!=structure) {
            pvField->message(noTimeStamp,errorMessage);
            return false;
        }
        pvStructure = static_cast<PVStructure*>(pvField);
    }
    PVLong *pvLong = pvStructure->getLongField(String("secondsPastEpoch"));
    if(pvLong==0) {
            pvField->message(noTimeStamp,errorMessage);
            return false;
    }
    PVInt *pvInt = pvStructure->getIntField(String("nanoSeconds"));
    if(pvLong==0) {
            pvField->message(noTimeStamp,errorMessage);
            return false;
    }
    pvSecs = pvLong;
    pvNano = pvInt;
    return true;
}

void PVTimeStamp::detach()
{
    pvSecs = 0;
    pvNano = 0;
}

bool PVTimeStamp::isAttached() {
    if(pvSecs==0 || pvNano==0) return false;
    return true;
}

void PVTimeStamp::get(TimeStamp & timeStamp) const
{
    if(pvSecs==0 || pvNano==0) {
        throw std::logic_error(notAttached);
    }
    timeStamp.put(pvSecs->get(),pvNano->get());
}

bool PVTimeStamp::set(TimeStamp const & timeStamp)
{
    if(pvSecs==0 || pvNano==0) {
        throw std::logic_error(notAttached);
    }
    if(pvSecs->isImmutable() || pvNano->isImmutable()) return false;
    pvSecs->put(timeStamp.getSecondsPastEpoch());
    pvNano->put(timeStamp.getNanoSeconds());
    return true;
}
    
}}
