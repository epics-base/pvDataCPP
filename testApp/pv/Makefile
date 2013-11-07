TOP=../..

include $(TOP)/configure/CONFIG

PROD_HOST += testBitSetUtil
testBitSetUtil_SRCS += testBitSetUtil.cpp
testBitSetUtil_LIBS += pvData Com

PROD_HOST += testIntrospect
testIntrospect_SRCS += testIntrospect.cpp
testIntrospect_LIBS += pvData Com
TESTS += testIntrospect

PROD_HOST += testPVAppend
testPVAppend_SRCS += testPVAppend.cpp
testPVAppend_LIBS += pvData Com

PROD_HOST += testPVType
testPVType_SRCS += testPVType.cpp
testPVType_LIBS += pvData Com

PROD_HOST += testPVAuxInfo
testPVAuxInfo_SRCS += testPVAuxInfo.cpp
testPVAuxInfo_LIBS += pvData Com

PROD_HOST += testStandardField
testStandardField_SRCS += testStandardField.cpp
testStandardField_LIBS += pvData Com

PROD_HOST += testStandardPVField
testStandardPVField_SRCS += testStandardPVField.cpp
testStandardPVField_LIBS += pvData Com

PROD_HOST += testPVData
testPVData_SRCS += testPVData.cpp
testPVData_LIBS += pvData Com

PROD_HOST += testConvert
testConvert_SRCS += testConvert.cpp
testConvert_LIBS += pvData Com

TESTPROD += testPVScalarArray
testPVScalarArray_SRCS += testPVScalarArray.cpp
testPVScalarArray_LIBS += pvData Com
TESTS += testPVScalarArray

PROD_HOST += testPVStructureArray
testPVStructureArray_SRCS += testPVStructureArray.cpp
testPVStructureArray_LIBS += pvData Com
TESTS += testPVStructureArray

PROD_HOST += testOperators
testOperators_SRCS += testOperators.cpp
testOperators_LIBS += pvData Com

PROD_HOST += testFieldBuilder
testFieldBuilder_SRCS += testFieldBuilder.cpp
testFieldBuilder_LIBS += pvData Com
TESTS += testFieldBuilder

TESTSCRIPTS_HOST += $(TESTS:%=%.t)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

