/*
 * Run pvData tests as a batch.
 *
 * Do *not* include performance measurements here, they don't help to
 * prove functionality (which is the point of this convenience routine).
 */

#include <stdio.h>
#include <epicsThread.h>
#include <epicsUnitTest.h>

/* copy */
int testCreateRequest(void);
int testPVCopy(void);

/* misc */
int testBaseException(void);
int testBitSet(void);
int testByteBuffer(void);
int testMessageQueue(void);
int testOverrunBitSet(void);
int testQueue(void);
int testSerialization(void);
int testSharedVector(void);
int testThread(void);
int testTimeStamp(void);
int testTimer(void);
int testTypeCast(void);

/* property */
int testCreateRequest(void);

/* pv */
int testBitSetUtil(void);
int testConvert(void);
int testFieldBuilder(void);
int testIntrospect(void);
int testOperators(void);
int testPVData(void);
int testPVScalarArray(void);
int testPVStructureArray(void);
int testPVType(void);
int testPVUnion(void);
int testStandardField(void);
int testStandardPVField(void);

void pvDataAllTests(void)
{
    testHarness();

    /* copy */
    runTest(testCreateRequest);
    runTest(testPVCopy);

    /* misc */
    runTest(testBaseException);
    runTest(testBitSet);
    runTest(testByteBuffer);
    runTest(testMessageQueue);
    runTest(testOverrunBitSet);
    runTest(testQueue);
    runTest(testSerialization);
    runTest(testSharedVector);
    runTest(testThread);
    runTest(testTimeStamp);
    runTest(testTimer);
    runTest(testTypeCast);

    /* property */
    runTest(testCreateRequest);

    /* pv */
    runTest(testBitSetUtil);
    runTest(testConvert);
    runTest(testFieldBuilder);
    runTest(testIntrospect);
    runTest(testOperators);
    runTest(testPVData);
    runTest(testPVScalarArray);
    runTest(testPVStructureArray);
    runTest(testPVType);
    runTest(testPVUnion);
    runTest(testStandardField);
    runTest(testStandardPVField);

}

