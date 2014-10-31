TOP=../..

include $(TOP)/configure/CONFIG

PROD_LIBS += pvData Com

TESTPROD_HOST += testThread
testThread_SRCS += testThread.cpp
testHarness_SRCS += testThread.cpp
TESTS += testThread

TESTPROD_HOST += testTimer
testTimer_SRCS += testTimer.cpp
testHarness_SRCS += testTimer.cpp
TESTS += testTimer

TESTPROD_HOST += testBitSet
testBitSet_SRCS += testBitSet.cpp
testHarness_SRCS += testBitSet.cpp
TESTS += testBitSet

TESTPROD_HOST += testOverrunBitSet
testOverrunBitSet_SRCS += testOverrunBitSet.cpp
testHarness_SRCS += testOverrunBitSet.cpp
TESTS += testOverrunBitSet

TESTPROD_HOST += testByteOrder
testByteOrder_SRCS += testByteOrder.cpp

TESTPROD_HOST += testByteBuffer
testByteBuffer_SRCS += testByteBuffer.cpp
testHarness_SRCS += testByteBuffer.cpp
TESTS += testByteBuffer

TESTPROD_HOST += testBaseException
testBaseException_SRCS += testBaseException.cpp
testHarness_SRCS += testBaseException.cpp
TESTS += testBaseException

TESTPROD_HOST += testSharedVector
testSharedVector_SRCS += testSharedVector.cpp
testHarness_SRCS += testSharedVector.cpp
TESTS += testSharedVector

TESTPROD_HOST += testSerialization
testSerialization_SRCS += testSerialization.cpp
testHarness_SRCS += testSerialization.cpp
TESTS += testSerialization

TESTPROD_HOST += testTimeStamp
testTimeStamp_SRCS += testTimeStamp.cpp
testHarness_SRCS += testTimeStamp.cpp
TESTS += testTimeStamp

TESTPROD_HOST += testQueue
testQueue_SRCS += testQueue.cpp
testHarness_SRCS += testQueue.cpp
TESTS += testQueue

TESTPROD_HOST += testMessageQueue
testMessageQueue_SRCS += testMessageQueue.cpp
testHarness_SRCS += testMessageQueue.cpp
TESTS += testMessageQueue

TESTPROD_HOST += testTypeCast
testTypeCast_SRCS += testTypeCast.cpp
testHarness_SRCS += testTypeCast.cpp
TESTS += testTypeCast

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

