# Makefile for the pvData tests

TOP = ..
include $(TOP)/configure/CONFIG

PVDATA_TEST = $(TOP)/testApp

PROD_LIBS += pvData Com

include $(PVDATA_TEST)/misc/Makefile
include $(PVDATA_TEST)/pv/Makefile
include $(PVDATA_TEST)/property/Makefile
include $(PVDATA_TEST)/copy/Makefile

# pvDataAllTests runs all the test programs in a known working order.
testHarness_SRCS += pvDataAllTests.c

# Name the application pvdTestHarness
pvdTestHarness_SRCS = $(testHarness_SRCS)

# Build for vxWorks
PROD_vxWorks += pvdTestHarness
TESTSPEC_vxWorks = pvdTestHarness.$(MUNCH_SUFFIX); pvDataAllTests

# Build for RTEMS, with harness code & OS configuration
PROD_RTEMS += pvdTestHarness
pvdTestHarness_SRCS_RTEMS = rtemsTestHarness.c
TESTSPEC_RTEMS = pvdTestHarness.$(MUNCH_SUFFIX); pvDataAllTests

# Build test scripts for hosts
TESTSCRIPTS_HOST += $(TESTS:%=%.t)

include $(TOP)/configure/RULES


