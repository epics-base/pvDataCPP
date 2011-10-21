/* alarm.cpp */
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
#include <pv/alarm.h>
namespace epics { namespace pvData { 

const size_t severityCount = 5;
static String severityNames[severityCount] =
{
     String("NONE"),
     String("MINOR"),
     String("MAJOR"),
     String("INVALID"),
     String("UNDEFINED")
};

AlarmSeverity AlarmSeverityFunc::getSeverity(int value)
{
    if(value<0 || value>4) {
         throw std::logic_error(String("getSeverity value is illegal"));
    }
    switch (value) {
    case 0: return noAlarm;
    case 1: return minorAlarm;
    case 2: return majorAlarm;
    case 3: return invalidAlarm;
    case 4: return undefinedAlarm;
    }
    throw std::logic_error(String("should never get here"));
}

StringArray AlarmSeverityFunc::getSeverityNames()
{
    return severityNames;
}

AlarmSeverity Alarm::getSeverity() const
{
    switch(severity) {
    case 0: return noAlarm;
    case 1: return minorAlarm;
    case 2: return majorAlarm;
    case 3: return invalidAlarm;
    case 4: return undefinedAlarm;
    }
    throw std::logic_error(String("should never get here"));
}

const size_t statusCount = 8;
static String statusNames[statusCount] =
{
     String("NONE"),
     String("DEVICE"),
     String("DRIVER"),
     String("RECORD"),
     String("DB"),
     String("CONF"),
     String("UNDEFINED"),
     String("CLIENT")
};

AlarmStatus AlarmStatusFunc::getStatus(int value)
{
    if(value<0 || value>7) {
         throw std::logic_error(String("getStatus value is illegal"));
    }
    switch (value) {
    case 0: return noStatus;
    case 1: return deviceStatus;
    case 2: return driverStatus;
    case 3: return recordStatus;
    case 4: return dbStatus;
    case 5: return confStatus;
    case 6: return undefinedStatus;
    case 7: return clientStatus;
    }
    throw std::logic_error(String("should never get here"));
}

StringArray AlarmStatusFunc::getStatusNames()
{
    return statusNames;
}

AlarmStatus Alarm::getStatus() const
{
    switch(status) {
    case 0: return noStatus;
    case 1: return deviceStatus;
    case 2: return driverStatus;
    case 3: return recordStatus;
    case 4: return dbStatus;
    case 5: return confStatus;
    case 6: return undefinedStatus;
    case 7: return clientStatus;
    }
    throw std::logic_error(String("should never get here"));
}

}}
