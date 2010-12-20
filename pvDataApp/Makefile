TOP = ..
include $(TOP)/configure/CONFIG

PVDATA = $(TOP)/pvDataApp/

SRC_DIRS += $(PVDATA)/misc

INC += pvType.h
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
INC += showConstructDestruct.h
INC += timeFunction.h
INC += timer.h
INC += queueVoid.h
INC += queue.h
INC += messageQueue.h
INC += destroyable.h
INC += status.h

LIBSRCS += byteBuffer.cpp
LIBSRCS += bitSet.cpp
LIBSRCS += requester.cpp
LIBSRCS += serializeHelper.cpp
LIBSRCS += showConstructDestruct.cpp
LIBSRCS += linkedListVoid.cpp
LIBSRCS += event.cpp
LIBSRCS += thread.cpp
LIBSRCS += executor.cpp
LIBSRCS += timeFunction.cpp
LIBSRCS += timer.cpp
LIBSRCS += queueVoid.cpp
LIBSRCS += messageQueue.cpp

SRC_DIRS += $(PVDATA)/pv

INC += pvIntrospect.h
INC += pvData.h
INC += convert.h
INC += standardField.h
INC += standardPVField.h

SRC_DIRS += $(PVDATA)/factory

INC += factory.h
INC += AbstractPVField.h
INC += AbstractPVScalar.h
INC += AbstractPVArray.h
INC += AbstractPVScalarArray.h
INC += BasePVStructure.h
INC += BasePVBoolean.h
INC += BasePVByte.h
INC += BasePVShort.h
INC += BasePVInt.h
INC += BasePVLong.h
INC += BasePVFloat.h
INC += BasePVDouble.h
INC += BasePVString.h
INC += BasePVBooleanArray.h
INC += BasePVByteArray.h
INC += BasePVShortArray.h
INC += BasePVIntArray.h
INC += BasePVLongArray.h
INC += BasePVFloatArray.h
INC += BasePVDoubleArray.h
INC += BasePVString.h
INC += BasePVStructure.h
INC += BasePVStructureArray.h
LIBSRCS += TypeFunc.cpp
LIBSRCS += PVAuxInfoImpl.cpp
LIBSRCS += FieldCreateFactory.cpp
LIBSRCS += PVDataCreateFactory.cpp
LIBSRCS += Convert.cpp
LIBSRCS += StandardField.cpp
LIBSRCS += StandardPVField.cpp
LIBSRCS += StatusCreateFactory.cpp 

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


LIBRARY=pvData

pvData_LIBS += Com

include $(TOP)/configure/RULES

