TOP=../..

include $(TOP)/configure/CONFIG

PROD_LIBS += pvData Com

TESTPROD_HOST += testCreateRequest
testCreateRequest_SRCS = testCreateRequest.cpp
testHarness_SRCS += testCreateRequest.cpp
TESTS += testCreateRequest

TESTPROD_HOST += testPVCopy
testPVCopy_SRCS += testPVCopy.cpp
testHarness_SRCS += testPVCopy.cpp
TESTS += testPVCopy

# The testHarness runs all the test programs in a known working order.
testHarness_SRCS += epicsRunPVDataTests.c

PROD_vxWorks = vxTestHarness
vxTestHarness_SRCS += $(testHarness_SRCS)
TESTSPEC_vxWorks = vxTestHarness.$(MUNCH_SUFFIX); epicsRunPVDataTests.c

#PROD_RTEMS += rtemsTestHarness
#rtemsTestHarness_SRCS += rtemsTestHarness.c
#rtemsTestHarness_SRCS += $(testHarness_SRCS)
#TESTSPEC_RTEMS = rtemsTestHarness.boot; epicsRunPVDataTests.c

TESTSCRIPTS_HOST += $(TESTS:%=%.t)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

