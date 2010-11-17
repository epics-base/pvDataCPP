TOP=../..

include $(TOP)/configure/CONFIG

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
INC += timeStamp.h
INC += timeFunction.h

LIBSRCS += byteBuffer.cpp
LIBSRCS += bitSet.cpp
LIBSRCS += serializeHelper.cpp
LIBSRCS += showConstructDestruct.cpp
LIBSRCS += linkedListVoid.cpp
LIBSRCS += event.cpp
LIBSRCS += thread.cpp
LIBSRCS += executor.cpp
LIBSRCS += timeStamp.cpp
LIBSRCS += timeFunction.cpp

LIBRARY=pvMisc

pvMisc_LIBS += Com

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

