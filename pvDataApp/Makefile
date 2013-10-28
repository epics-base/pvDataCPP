TOP = ..
include $(TOP)/configure/CONFIG

PVDATA = $(TOP)/pvDataApp/

SRC_DIRS += $(PVDATA)/misc

INC += noDefaultMethods.h
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
INC += timeFunction.h
INC += timer.h
INC += queue.h
INC += messageQueue.h
INC += destroyable.h
INC += status.h
INC += sharedPtr.h
INC += localStaticLock.h
INC += typeCast.h
INC += printer.h
INC += sharedVector.h
INC += templateMeta.h

LIBSRCS += byteBuffer.cpp
LIBSRCS += bitSet.cpp
LIBSRCS += epicsException.cpp
LIBSRCS += requester.cpp
LIBSRCS += serializeHelper.cpp
LIBSRCS += event.cpp
LIBSRCS += executor.cpp
LIBSRCS += timeFunction.cpp
LIBSRCS += timer.cpp
LIBSRCS += status.cpp
LIBSRCS += messageQueue.cpp
LIBSRCS += localStaticLock.cpp
LIBSRCS += typeCast.cpp
LIBSRCS += parseToPOD.cpp

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
LIBSRCS += FieldCreateFactory.cpp
LIBSRCS += PVAuxInfoImpl.cpp
LIBSRCS += PVField.cpp
LIBSRCS += PVScalar.cpp
LIBSRCS += PVArray.cpp
LIBSRCS += PVScalarArray.cpp
LIBSRCS += PVStructure.cpp
LIBSRCS += PVStructureArray.cpp
LIBSRCS += PVDataCreateFactory.cpp
LIBSRCS += Convert.cpp
LIBSRCS += Compare.cpp
LIBSRCS += StandardField.cpp
LIBSRCS += StandardPVField.cpp
LIBSRCS += printer.cpp

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

LIBRARY = pvData
pvData_LIBS += Com

include $(TOP)/configure/RULES

