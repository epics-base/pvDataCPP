# Makefile for the pvData tests

TOP = ..
include $(TOP)/configure/CONFIG

PVDATA_TEST = $(TOP)/testApp

PROD_LIBS += pvData Com

include $(PVDATA_TEST)/misc/Makefile
include $(PVDATA_TEST)/pv/Makefile
include $(PVDATA_TEST)/property/Makefile
include $(PVDATA_TEST)/copy/Makefile

PROD_vxWorks = vxTestHarness
vxTestHarness_SRCS += pvDataAllTests
vxTestHarness_OBJS += misc/
TESTSPEC_vxWorks = vxTestHarness.$(MUNCH_SUFFIX); pvDataAllTests

TESTSCRIPTS_HOST += $(TESTS:%=%.t)

include $(TOP)/configure/RULES


