TOP = ..
include $(TOP)/configure/CONFIG

PVDATA = $(TOP)/pvDataApp/

SRC_DIRS += $(PVDATA)/misc

INC += noDefaultMethods.h
INC += linkedListVoid.h
INC += linkedList.h
INC += lock.h
INC += requester.h
INC += serialize.h
INC += bitSet.h
INC += byteBuffer.h
INC += epicsException.h
INC += serializeHelper.h
INC += event.h
INC += thread.h
INC += executor.h
INC += CDRMonitor.h
INC += timeFunction.h
INC += timer.h
INC += queueVoid.h
INC += queue.h
INC += messageQueue.h
INC += destroyable.h
INC += status.h
INC += sharedPtr.h

LIBSRCS += CDRMonitor.cpp
LIBSRCS += byteBuffer.cpp
LIBSRCS += bitSet.cpp
LIBSRCS += epicsException.cpp
LIBSRCS += requester.cpp
LIBSRCS += serializeHelper.cpp
LIBSRCS += linkedListVoid.cpp
LIBSRCS += event.cpp
LIBSRCS += executor.cpp
LIBSRCS += timeFunction.cpp
LIBSRCS += timer.cpp
LIBSRCS += queueVoid.cpp
LIBSRCS += messageQueue.cpp
LIBSRCS += status.cpp

SRC_DIRS += $(PVDATA)/pv

INC += pvType.h
INC += pvIntrospect.h
INC += pvData.h
INC += convert.h
INC += standardField.h
INC += standardPVField.h

SRC_DIRS += $(PVDATA)/factory

INC += factory.h
LIBSRCS += TypeFunc.cpp
LIBSRCS += PVAuxInfoImpl.cpp
LIBSRCS += FieldCreateFactory.cpp
LIBSRCS += PVField.cpp
LIBSRCS += PVScalar.cpp
LIBSRCS += PVArray.cpp
LIBSRCS += PVScalarArray.cpp
LIBSRCS += PVStructure.cpp
LIBSRCS += DefaultPVStructureArray.cpp
LIBSRCS += PVDataCreateFactory.cpp
LIBSRCS += Convert.cpp
LIBSRCS += Compare.cpp
LIBSRCS += StandardField.cpp
LIBSRCS += StandardPVField.cpp

SRC_DIRS += $(PVDATA)/property

INC += alarm.h
INC += pvAlarm.h
INC += control.h
INC += pvControl.h
INC += display.h
INC += pvDisplay.h
INC += pvEnumerated.h
INC += timeStamp.h
INC += pvTimeStamp.h

LIBSRCS += alarm.cpp 
LIBSRCS += pvAlarm.cpp 
LIBSRCS += pvControl.cpp 
LIBSRCS += pvDisplay.cpp 
LIBSRCS += pvEnumerated.cpp 
LIBSRCS += timeStamp.cpp
LIBSRCS += pvTimeStamp.cpp

SRC_DIRS += $(PVDATA)/pvMisc
INC += bitSetUtil.h
LIBSRCS += bitSetUtil.cpp 

SRC_DIRS += $(PVDATA)/monitor
INC += monitor.h
INC += monitorQueue.h
LIBSRCS += monitorQueue.cpp


LIBRARY=pvData

pvData_LIBS += Com

include $(TOP)/configure/RULES

