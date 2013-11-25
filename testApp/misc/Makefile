TOP=../..

include $(TOP)/configure/CONFIG

PROD_LIBS += pvData Com

PROD_HOST += testThread
testThread_SRCS += testThread.cpp
TESTS += testThread

PROD_HOST += testTimer
testTimer_SRCS += testTimer.cpp
TESTS += testTimer

PROD_HOST += testBitSet
testBitSet_SRCS += testBitSet.cpp
TESTS += testBitSet

PROD_HOST += testOverrunBitSet
testOverrunBitSet_SRCS += testOverrunBitSet.cpp
TESTS += testOverrunBitSet

PROD_HOST += testByteOrder
testByteOrder_SRCS += testByteOrder.cpp

PROD_HOST += testByteBuffer
testByteBuffer_SRCS += testByteBuffer.cpp
TESTS += testByteBuffer

PROD_HOST += testBaseException
testBaseException_SRCS += testBaseException.cpp
TESTS += testBaseException

PROD_HOST += testSharedVector
testSharedVector_SRCS += testSharedVector.cpp
TESTS += testSharedVector

PROD_HOST += testSerialization
testSerialization_SRCS += testSerialization.cpp
TESTS += testSerialization

PROD_HOST += testTimeStamp
testTimeStamp_SRCS += testTimeStamp.cpp
TESTS += testTimeStamp

PROD_HOST += testQueue
testQueue_SRCS += testQueue.cpp
TESTS += testQueue

PROD_HOST += testMessageQueue
testMessageQueue_SRCS += testMessageQueue.cpp
TESTS += testMessageQueue

PROD_HOST += testTypeCast
testTypeCast_SRCS += testTypeCast.cpp
TESTS += testTypeCast

TESTSCRIPTS_HOST += $(TESTS:%=%.t)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

