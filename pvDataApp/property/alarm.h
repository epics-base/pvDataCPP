/* alarm.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <string>
#include <pv/pvType.h>
#ifndef ALARM_H
#define ALARM_H
namespace epics { namespace pvData { 

enum AlarmSeverity {
 noAlarm,minorAlarm,majorAlarm,invalidAlarm
};

extern const size_t severityCount;
class AlarmSeverityFunc {
public:
    static AlarmSeverity getSeverity(int value);
    static StringArray getSeverityNames();
};

class Alarm {
public:
    Alarm() : severity(0), message(String("")) {}
    //default constructors and destructor are OK
    String getMessage() const {return message;}
    void setMessage(String value) {message = value;}
    AlarmSeverity getSeverity() const;
    void setSeverity(AlarmSeverity value) {severity = value;}
private:
    int32 severity;
    String message;
};
    
}}
#endif  /* ALARM_H */
