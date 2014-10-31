TOP=../..

include $(TOP)/configure/CONFIG

TESTPROD_HOST += testCreateRequest
testCreateRequest_SRCS = testCreateRequest.cpp
testCreateRequest_LIBS = pvData Com
TESTS += testCreateRequest

TESTPROD_HOST += testPVCopy
testPVCopy_SRCS += testPVCopy.cpp
testPVCopy_LIBS += pvData Com
TESTS += testPVCopy

TESTSCRIPTS_HOST += $(TESTS:%=%.t)



include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

