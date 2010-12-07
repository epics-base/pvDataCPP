/* alarm.cpp */
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
#include "alarm.h"
namespace epics { namespace pvData { 

static String severityNames[4] =
{
     String("none"),
     String("minor"),
     String("major"),
     String("invalid")
};

AlarmSeverity AlarmSeverityFunc::getSeverity(int value)
{
    if(value<0 || value>3) {
         throw std::logic_error(String("getSeverity value is illegal"));
    }
    switch (value) {
    case 0: return noAlarm;
    case 1: return minorAlarm;
    case 2: return majorAlarm;
    case 3: return invalidAlarm;
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
    }
    throw std::logic_error(String("should never get here"));
}

}}
