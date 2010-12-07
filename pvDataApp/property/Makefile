TOP=../..

include $(TOP)/configure/CONFIG

INC += alarm.h
INC += pvAlarm.h
INC += control.h
INC += pvControl.h
INC += display.h
INC += pvDisplay.h
INC += pvEnumerated.h
INC += pvTimeStamp.h

LIBSRCS += alarm.cpp 
LIBSRCS += pvAlarm.cpp 
LIBSRCS += pvControl.cpp 
LIBSRCS += pvDisplay.cpp 
LIBSRCS += pvEnumerated.cpp 
LIBSRCS += pvTimeStamp.cpp
enumerated_LIBS += pvFactory pvMisc Com

LIBRARY=pvProperty


include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

