/*
 * Run pvData tests as a batch.
 *
 * Do *not* include performance measurements here, they don't help to
 * prove functionality (which is the point of this convenience routine).
 */

#include <stdio.h>
#include <epicsThread.h>
#include <epicsUnitTest.h>

int testBitSetUtil(int);
int testConvert(int);
int testFieldBuilder(int);
int testIntrospect(int);
int testOperators(int);
int testPVData(int);
int testPVScalarArray(int);
int testPVStructureArray(int);
int testPVType(int);
int testPVUnion(int);
int testStandardField(int);
int testStandardPVField(int);

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
