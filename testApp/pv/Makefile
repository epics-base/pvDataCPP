TOP=../..

include $(TOP)/configure/CONFIG

PROD_LIBS += pvData Com

TESTPROD_HOST += testBitSetUtil
testBitSetUtil_SRCS += testBitSetUtil.cpp
testHarness_SRCS += testBitSetUtil.cpp
TESTS += testBitSetUtil

TESTPROD_HOST += testIntrospect
testIntrospect_SRCS += testIntrospect.cpp
testHarness_SRCS += testIntrospect.cpp
TESTS += testIntrospect

TESTPROD_HOST += testPVType
testPVType_SRCS += testPVType.cpp
testHarness_SRCS += testPVType.cpp
TESTS += testPVType

TESTPROD_HOST += testStandardField
testStandardField_SRCS += testStandardField.cpp
testHarness_SRCS += testStandardField.cpp
TESTS += testStandardField

TESTPROD_HOST += testStandardPVField
testStandardPVField_SRCS += testStandardPVField.cpp
testHarness_SRCS += testStandardPVField.cpp
TESTS += testStandardPVField

TESTPROD_HOST += testPVData
testPVData_SRCS += testPVData.cpp
testHarness_SRCS += testPVData.cpp
TESTS += testPVData

TESTPROD_HOST += testPVUnion
testPVUnion_SRCS += testPVUnion.cpp
testHarness_SRCS += testPVUnion.cpp
TESTS += testPVUnion

TESTPROD_HOST += testConvert
testConvert_SRCS += testConvert.cpp
testHarness_SRCS += testConvert.cpp
TESTS += testConvert

TESTPROD_HOST += testPVScalarArray
testPVScalarArray_SRCS += testPVScalarArray.cpp
testHarness_SRCS += testPVScalarArray.cpp
TESTS += testPVScalarArray

TESTPROD_HOST += testPVStructureArray
testPVStructureArray_SRCS += testPVStructureArray.cpp
testHarness_SRCS += testPVStructureArray.cpp
TESTS += testPVStructureArray

TESTPROD_HOST += testOperators
testOperators_SRCS += testOperators.cpp
testHarness_SRCS += testOperators.cpp
TESTS += testOperators

TESTPROD_HOST += testFieldBuilder
testFieldBuilder_SRCS += testFieldBuilder.cpp
testHarness_SRCS += testFieldBuilder.cpp
TESTS += testFieldBuilder

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

