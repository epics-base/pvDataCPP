/* pvTimeStamp.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <string>
#include <stdexcept>
#include <pv/pvType.h>
#include <pv/timeStamp.h>
#include <pv/pvData.h>
#include <pv/pvTimeStamp.h>
namespace epics { namespace pvData { 

using std::tr1::static_pointer_cast;

static String noTimeStamp("No timeStamp structure found");
static String notAttached("Not attached to a timeStamp structure");

bool PVTimeStamp::attach(PVFieldPtr pvField)
{
    if(pvField->getField()->getType()!=structure) {
            pvField->message(noTimeStamp,errorMessage);
            return false;
    }
    PVStructurePtr xxx = static_pointer_cast<PVStructure>(pvField);
    PVStructure* pvStructure = xxx.get();
    while(true) {
        PVLongPtr pvLong = pvStructure->getLongField("secondsPastEpoch");
        if(pvLong.get()!=NULL) {
            pvSecs = pvLong;
            pvNano = pvStructure->getIntField("nanoSeconds");
            pvUserTag = pvStructure->getIntField("userTag");
        }
        if(pvSecs.get()!=NULL
        && pvNano.get()!=NULL
        && pvUserTag.get()!=NULL) return true;
        detach();
        // look up the tree for a timeSyamp
        pvStructure = pvStructure->getParent();
        if(pvStructure==NULL) break;
    }
    return false;
}

void PVTimeStamp::detach()
{
    pvSecs.reset();
    pvUserTag.reset();
    pvNano.reset();
}

bool PVTimeStamp::isAttached() {
    if(pvSecs.get()==NULL) return false;
    return true;
}

void PVTimeStamp::get(TimeStamp & timeStamp) const
{
    if(pvSecs.get()==NULL) {
        throw std::logic_error(notAttached);
    }
    timeStamp.put(pvSecs->get(),pvNano->get());
    timeStamp.setUserTag(pvUserTag->get());
}

bool PVTimeStamp::set(TimeStamp const & timeStamp)
{
    if(pvSecs.get()==NULL) {
        throw std::logic_error(notAttached);
    }
    if(pvSecs->isImmutable() || pvNano->isImmutable()) return false;
    pvSecs->put(timeStamp.getSecondsPastEpoch());
    pvUserTag->put(timeStamp.getUserTag());
    pvNano->put(timeStamp.getNanoSeconds());
    return true;
}
    
}}
