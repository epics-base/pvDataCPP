# Makefile for the pvData library

TOP = ..
include $(TOP)/configure/CONFIG

INSTALL_INCLUDE = $(INSTALL_LOCATION)/include/pv
USR_INCLUDES += -I$(INSTALL_LOCATION)/include

PVDATA_SRC = $(TOP)/src

include $(PVDATA_SRC)/misc/Makefile
include $(PVDATA_SRC)/pv/Makefile
include $(PVDATA_SRC)/factory/Makefile
include $(PVDATA_SRC)/property/Makefile
include $(PVDATA_SRC)/pvMisc/Makefile
include $(PVDATA_SRC)/monitor/Makefile

LIBRARY = pvData

pvData_LIBS += Com

include $(TOP)/configure/RULES

