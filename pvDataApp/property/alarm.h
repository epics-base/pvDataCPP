/* alarm.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#include <string>
#include <pv/pvType.h>
#ifndef ALARM_H
#define ALARM_H
namespace epics { namespace pvData { 

enum AlarmSeverity {
 noAlarm,minorAlarm,majorAlarm,invalidAlarm,undefinedAlarm
};

enum AlarmStatus {
    noStatus,deviceStatus,driverStatus,recordStatus,
    dbStatus,confStatus,undefinedStatus,clientStatus
};


extern const size_t severityCount;
class AlarmSeverityFunc {
public:
    static AlarmSeverity getSeverity(int value);
    static StringArrayPtr getSeverityNames();
};

extern const size_t statusCount;
class AlarmStatusFunc {
public:
    static AlarmStatus getStatus(int value);
    static StringArrayPtr getStatusNames();
};

class Alarm {
public:
    Alarm() : severity(0),status(0), message(String("")) {}
    //default constructors and destructor are OK
    String getMessage() const {return message;}
    void setMessage(String const &value) {message = value;}
    AlarmSeverity getSeverity() const;
    void setSeverity(AlarmSeverity value) {severity = value;}
    AlarmStatus getStatus() const;
    void setStatus(AlarmStatus value) { status = value;}
private:
    int32 severity;
    int32 status;
    String message;
};
    
}}
#endif  /* ALARM_H */
