/*
 * Run pvData tests as a batch.
 *
 * Do *not* include performance measurements here, they don't help to
 * prove functionality (which is the point of this convenience routine).
 */

#include <stdio.h>
#include <epicsThread.h>
#include <epicsUnitTest.h>

int testCreateRequest(void);

void epicsRunPVDataTests(void)
{
    testHarness();

    runTest(testCreateRequest);

}
