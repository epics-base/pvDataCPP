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
INC += current_function.h

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
INC += pvSubArrayCopy.h

SRC_DIRS += $(PVDATA)/factory

INC += factory.h
LIBSRCS += TypeFunc.cpp
LIBSRCS += FieldCreateFactory.cpp
LIBSRCS += PVField.cpp
LIBSRCS += PVScalar.cpp
LIBSRCS += PVArray.cpp
LIBSRCS += PVScalarArray.cpp
LIBSRCS += PVStructure.cpp
LIBSRCS += PVStructureArray.cpp
LIBSRCS += PVUnion.cpp
LIBSRCS += PVUnionArray.cpp
LIBSRCS += PVDataCreateFactory.cpp
LIBSRCS += Convert.cpp
LIBSRCS += pvSubArrayCopy.cpp
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


SRC_DIRS += $(PVDATA)/copy
INC += createRequest.h
INC += pvCopy.h
LIBSRCS += createRequest.cpp
LIBSRCS += pvCopy.cpp


SRC_DIRS += $(PVDATA)/pvMisc
INC += bitSetUtil.h
LIBSRCS += bitSetUtil.cpp 

SRC_DIRS += $(PVDATA)/monitor
INC += monitor.h
LIBSRCS += monitor.cpp

LIBRARY = pvData
pvData_LIBS += Com

USR_CXXFLAGS_RTEMS = -D__rtems__

include $(TOP)/configure/RULES

