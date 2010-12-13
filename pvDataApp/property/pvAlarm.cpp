/* pvAlarm.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <string>
#include <stdexcept>
#include "pvType.h"
#include "pvIntrospect.h"
#include "pvData.h"
#include "pvAlarm.h"
namespace epics { namespace pvData { 

static String noAlarmFound("No alarm structure found");
static String notAttached("Not attached to an alarm structure");

bool PVAlarm::attach(PVField *pvField)
{
    PVStructure *pvStructure = 0;
    if(pvField->getField()->getFieldName().compare("alarm")!=0) {
        if(pvField->getField()->getFieldName().compare("value")!=0) {
            pvField->message(noAlarmFound,errorMessage);
            return false;
        }
        PVStructure *pvParent = pvField->getParent();
        if(pvParent==0) {
            pvField->message(noAlarmFound,errorMessage);
            return false;
        }
        pvStructure = pvParent->getStructureField(String("alarm"));
        if(pvStructure==0) {
            pvField->message(noAlarmFound,errorMessage);
            return false;
        }
    } else {
        if(pvField->getField()->getType()!=structure) {
            pvField->message(noAlarmFound,errorMessage);
            return false;
        }
        pvStructure = static_cast<PVStructure*>(pvField);
    }
    PVInt *pvInt = pvStructure->getIntField(String("severity"));
    if(pvInt==0) {
        pvField->message(noAlarmFound,errorMessage);
        return false;
    }
    PVString *pvString = pvStructure->getStringField(String("message"));
    if(pvInt==0) {
        pvField->message(noAlarmFound,errorMessage);
        return false;
    }
    pvSeverity = pvInt;
    pvMessage = pvString;
    return true;
}

void PVAlarm::detach()
{
    pvSeverity = 0;
    pvMessage = 0;
}

Alarm PVAlarm::get() const
{
    if(pvSeverity==0 || pvMessage==0) {
        throw std::logic_error(notAttached);
    }
    Alarm alarm;
    alarm.setSeverity(AlarmSeverityFunc::getSeverity(pvSeverity->get()));
    alarm.setMessage(pvMessage->get());
    return alarm;
}

bool PVAlarm::set(Alarm alarm)
{
    if(pvSeverity==0 || pvMessage==0) {
        throw std::logic_error(notAttached);
    }
    if(pvSeverity->isImmutable() || pvMessage->isImmutable()) return false;
    pvSeverity->put(alarm.getSeverity());
    pvMessage->put(alarm.getMessage());
    return true;
}

}}
