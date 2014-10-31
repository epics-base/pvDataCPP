TOP=../..

include $(TOP)/configure/CONFIG

PROD_LIBS = pvData Com

TESTPROD_HOST += testProperty
testProperty_SRCS += testProperty.cpp
testHarness_SRCS += testProperty.cpp
TESTS += testProperty

# The testHarness runs all the test programs in a known working order.
testHarness_SRCS += epicsRunPVDataTests.c

PROD_vxWorks = vxTestHarness
vxTestHarness_SRCS += $(testHarness_SRCS)
TESTSPEC_vxWorks = vxTestHarness.$(MUNCH_SUFFIX); epicsRunPVDataTests

#PROD_RTEMS += rtemsTestHarness
#rtemsTestHarness_SRCS += rtemsTestHarness.c
#rtemsTestHarness_SRCS += $(testHarness_SRCS)
#TESTSPEC_RTEMS = rtemsTestHarness.boot; epicsRunPVDataTests

TESTSCRIPTS_HOST += $(TESTS:%=%.t)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

