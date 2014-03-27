# Makefile for the pvData library

TOP = ..
include $(TOP)/configure/CONFIG

PVDATAAPP = $(TOP)/pvDataApp

include $(PVDATAAPP)/misc/Makefile
include $(PVDATAAPP)/pv/Makefile
include $(PVDATAAPP)/factory/Makefile
include $(PVDATAAPP)/property/Makefile
include $(PVDATAAPP)/pvMisc/Makefile
include $(PVDATAAPP)/monitor/Makefile

LIBRARY = pvData
pvData_LIBS += Com

USR_CXXFLAGS_RTEMS = -D__rtems__

include $(TOP)/configure/RULES

