# Makefile for the pvData library

TOP = ..
include $(TOP)/configure/CONFIG

PVDATASRC = $(TOP)/src

include $(PVDATASRC)/misc/Makefile
include $(PVDATASRC)/pv/Makefile
include $(PVDATASRC)/factory/Makefile
include $(PVDATASRC)/property/Makefile
include $(PVDATASRC)/pvMisc/Makefile
include $(PVDATASRC)/monitor/Makefile

LIBRARY = pvData
pvData_LIBS += Com

include $(TOP)/configure/RULES

