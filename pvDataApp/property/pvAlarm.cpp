/* pvAlarm.cpp */
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
#include <pv/pvAlarm.h>

namespace epics { namespace pvData { 

using std::tr1::static_pointer_cast;

String PVAlarm::noAlarmFound("No alarm structure found");
String PVAlarm::notAttached("Not attached to an alarm structure");

bool PVAlarm::attach(PVFieldPtr const & pvField)
{
    if(pvField->getField()->getType()!=structure) {
        pvField->message(noAlarmFound,errorMessage);
        return false;
    }
    PVStructurePtr pvStructure = static_pointer_cast<PVStructure>(pvField);
    pvSeverity = pvStructure->getIntField("severity");
    if(pvSeverity.get()==NULL) {
        pvField->message(noAlarmFound,errorMessage);
        return false;
    }
    pvStatus = pvStructure->getIntField("status");
    if(pvStatus.get()==NULL) {
        pvField->message(noAlarmFound,errorMessage);
        pvSeverity.reset();
        return false;
    }
    pvMessage = pvStructure->getStringField("message");
    if(pvMessage.get()==NULL) {
        pvField->message(noAlarmFound,errorMessage);
        pvSeverity.reset();
        pvStatus.reset();
        return false;
    }
    return true;
}

void PVAlarm::detach()
{
    pvSeverity.reset();
    pvStatus.reset();
    pvMessage.reset();
}

bool PVAlarm::isAttached()
{
    if(pvSeverity.get()==NULL) return false;
    return true;
}

void PVAlarm::get(Alarm & alarm) const
{
    if(pvSeverity.get()==NULL) {
        throw std::logic_error(notAttached);
    }
    alarm.setSeverity(AlarmSeverityFunc::getSeverity(pvSeverity->get()));
    alarm.setStatus(AlarmStatusFunc::getStatus(pvStatus->get()));
    alarm.setMessage(pvMessage->get());
}

bool PVAlarm::set(Alarm const & alarm)
{
    if(pvSeverity.get()==NULL) {
        throw std::logic_error(notAttached);
    }
    if(pvSeverity->isImmutable() || pvMessage->isImmutable()) return false;
    pvSeverity->put(alarm.getSeverity());
    pvStatus->put(alarm.getStatus());
    pvMessage->put(alarm.getMessage());
    return true;
}

}}
