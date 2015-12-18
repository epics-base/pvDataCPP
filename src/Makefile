# Makefile for the pvData library

TOP = ..
include $(TOP)/configure/CONFIG

PVDATA_SRC = $(TOP)/src

include $(PVDATA_SRC)/misc/Makefile
include $(PVDATA_SRC)/pv/Makefile
include $(PVDATA_SRC)/factory/Makefile
include $(PVDATA_SRC)/property/Makefile
include $(PVDATA_SRC)/copy/Makefile
include $(PVDATA_SRC)/pvMisc/Makefile
include $(PVDATA_SRC)/monitor/Makefile

LIBRARY = pvData

pvData_LIBS += Com

# shared library ABI version.
SHRLIB_VERSION ?= 5.0

include $(TOP)/configure/RULES

