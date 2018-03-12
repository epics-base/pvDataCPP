# Makefile for the pvData tests

TOP = ..
include $(TOP)/configure/CONFIG

PVDATA_TEST = $(TOP)/testApp

PROD_LIBS += pvData Com

include $(PVDATA_TEST)/misc/Makefile
include $(PVDATA_TEST)/pv/Makefile
include $(PVDATA_TEST)/property/Makefile
include $(PVDATA_TEST)/copy/Makefile

# The testHarness runs all the test programs in a known working order.
testHarness_SRCS += pvDataAllTests.c

PROD_vxWorks = vxTestHarness
vxTestHarness_SRCS += $(testHarness_SRCS)
TESTSPEC_vxWorks = vxTestHarness.$(MUNCH_SUFFIX); pvDataAllTests

PROD_RTEMS += pvdTestHarness
pvdTestHarness_SRCS += rtemsTestHarness.c rtemsConfig.c
pvdTestHarness_SRCS += $(testHarness_SRCS)
TESTSPEC_RTEMS = pvdTestHarness.$(MUNCH_SUFFIX); pvDataAllTests

TESTSCRIPTS_HOST += $(TESTS:%=%.t)

include $(TOP)/configure/RULES


