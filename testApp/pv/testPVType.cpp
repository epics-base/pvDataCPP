/* testPVType.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/* Author:  Marty Kraimer Date: 2010.09 */

#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>

#include <epicsUnitTest.h>
#include <testMain.h>

#include <pv/requester.h>
#include <pv/pvType.h>

using namespace epics::pvData;

MAIN(testPVType)
{
    testPlan(6);
    testOk1(sizeof(int8)==1);
    testOk1(sizeof(int16)==2);
    testOk1(sizeof(int32)==4);
    testOk1(sizeof(int64)==8);
    testOk1(sizeof(uint32)==4);
    testOk1(sizeof(uint64)==8);

    int intValue;
    int8 byteInt;
    intValue = 0x7f;
    byteInt = intValue;
    printf("int8 max %d",(int)byteInt);
    intValue = 0x80;
    byteInt = intValue;
    printf(" min %d\n",(int)byteInt);

    int16 shortInt;
    intValue = 0x7fff;
    shortInt = intValue;
    printf("int8 max %hd",shortInt);
    intValue = 0x8000;
    shortInt = intValue;
    printf(" min %hd\n",shortInt);

    int32 intInt;
    intValue = 0x7fffffff;
    intInt = intValue;
    printf("int8 max %d",intInt);
    intValue = 0x80000000;
    intInt = intValue;
    printf(" min %d\n",intInt);

    int64 longInt = 0x7fffffff;
    longInt <<= 32;
    longInt |= 0xffffffff;
    printf("int8 max %lli",(long long)longInt);
    longInt = intValue = 0x80000000;;
    longInt <<= 32;
    printf(" min %lli\n",(long long)longInt);

    printf("PASSED\n");
    return testDone();
}

