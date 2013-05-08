TOP=../..

include $(TOP)/configure/CONFIG

PROD_LIBS += pvData Com

PROD_HOST += testThread
testThread_SRCS += testThread.cpp

PROD_HOST += testTimer
testTimer_SRCS += testTimer.cpp

PROD_HOST += testBitSet
testBitSet_SRCS += testBitSet.cpp

PROD_HOST += testByteOrder
testByteOrder_SRCS += testByteOrder.cpp

PROD_HOST += testByteBuffer
testByteBuffer_SRCS += testByteBuffer.cpp

PROD_HOST += testBaseException
testBaseException_SRCS += testBaseException.cpp

TESTPROD += testSerialization
testSerialization_SRCS += testSerialization.cpp
TESTS += testSerialization

PROD_HOST += testTimeStamp
testTimeStamp_SRCS += testTimeStamp.cpp

PROD_HOST += testQueue
testQueue_SRCS += testQueue.cpp

PROD_HOST += testMessageQueue
testMessageQueue_SRCS += testMessageQueue.cpp

TESTPROD += testTypeCast
testTypeCast_SRCS += testTypeCast.cpp
TESTS += testTypeCast

TESTPROD += testSharedVector
testSharedVector_SRCS += testSharedVector.cpp
TESTS += testSharedVector

TESTSCRIPTS_HOST += $(TESTS:%=%.t)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

