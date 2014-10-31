/*
 * Run pvData tests as a batch.
 *
 * Do *not* include performance measurements here, they don't help to
 * prove functionality (which is the point of this convenience routine).
 */

#include <stdio.h>
#include <epicsThread.h>
#include <epicsUnitTest.h>

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

void epicsRunPVDataTests(void)
{
    testHarness();

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
