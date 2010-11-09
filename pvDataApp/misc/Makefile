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

LIBSRCS += byteBuffer.cpp
LIBSRCS += bitSet.cpp
LIBSRCS += serializeHelper.cpp
LIBSRCS += linkedListVoid.cpp

LIBRARY=pvMisc

pvMisc_LIBS += Com

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

