/* pvAlarm.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#ifndef PVALARM_H
#define PVALARM_H

#include <string>

#include <pv/pvType.h>
#include <pv/alarm.h>
#include <pv/pvData.h>

#include <sharelib.h>

namespace epics { namespace pvData {

class epicsShareClass PVAlarm {
public:
    PVAlarm() {}
    //default constructors and destructor are OK
    //returns (false,true) if pvField(isNot, is valid enumerated structure
    //An automatic detach is issued if already attached.
    bool attach(PVFieldPtr const & pvField);
    void detach();
    bool isAttached();
    // each of the following throws logic_error is not attached to PVField
    // set returns false if field is immutable
    void get(Alarm & alarm) const;
    bool set(Alarm const & alarm);
private:
    PVIntPtr pvSeverity;
    PVIntPtr pvStatus;
    PVStringPtr pvMessage;
    static String noAlarmFound;
    static String notAttached;
};
    
}}
#endif  /* PVALARM_H */
