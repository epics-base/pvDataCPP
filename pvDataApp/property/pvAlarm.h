/* pvAlarm.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <string>
#include "pvType.h"
#include "alarm.h"
#ifndef PVALARM_H
#define PVALARM_H
namespace epics { namespace pvData { 

class PVAlarm {
public:
    PVAlarm() : pvSeverity(0),pvMessage(0) {}
    //default constructors and destructor are OK
    //returns (false,true) if pvField(isNot, is valid enumerated structure
    //An automatic detach is issued if already attached.
    bool attach(PVField *pvField);
    void detach();
    // each of the following throws logic_error is not attached to PVField
    // set returns false if field is immutable
    Alarm get() const;
    bool set(Alarm alarm);
private:
    PVInt *pvSeverity;
    PVString *pvMessage;
};
    
}}
#endif  /* PVALARM_H */
