TOP=../..

include $(TOP)/configure/CONFIG

TESTPROD += testProperty
testProperty_SRCS += testProperty.cpp
TESTS += testProperty

testProperty_LIBS += pvData  Com

TESTSCRIPTS_HOST += $(TESTS:%=%.t)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

