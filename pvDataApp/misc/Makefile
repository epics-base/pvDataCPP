TOP=../..

include $(TOP)/configure/CONFIG

INC += noDefaultMethods.h
INC += lock.h
INC += requester.h
INC += serialize.h
INC += bitSet.h
INC += byteBuffer.h
INC += epicsException.h

LIBSRCS += byteBuffer.cpp
LIBSRCS += bitSet.cpp

LIBRARY=pvMisc

pvMisc_LIBS += Com

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

