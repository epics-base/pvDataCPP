TOP=../..

include $(TOP)/configure/CONFIG

PROD_HOST = test
test_SRCS += test.cpp
test_LIBS += pvFactory

PROD_HOST += testDumpStdString
testDumpStdString_SRCS += testDumpStdString.cpp
testDumpStdString_LIBS += pvFactory

PROD_HOST = testPVScalar
testPVScalar_SRCS += testPVScalar.cpp
testPVScalar_LIBS += pvFactory

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

