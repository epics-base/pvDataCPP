/* testIntrospect.cpp */
/* Author:  Marty Kraimer Date: 2010.09 */

#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>

#include <epicsAssert.h>

#include "requester.h"
#include "pvTypes.h"

using namespace epics::pvData;

int main(int argc,char *argv[])
{
    char *fileName = 0;
    if(argc>1) fileName = argv[1];
    FILE * fd = stdout;
    if(fileName!=0 && fileName[0]!=0) {
        fd = fopen(fileName,"w+");
    }
    assert(sizeof(int8)==1);
    assert(sizeof(int16)==2);
    assert(sizeof(int32)==4);
    assert(sizeof(int64)==8);
    assert(sizeof(uint32)==4);
    assert(sizeof(uint64)==8);

    int intValue;
    int8 byteInt;
    intValue = 0x7f;
    byteInt = intValue;
    fprintf(fd,"int8 max %d",(int)byteInt);
    intValue = 0x80;
    byteInt = intValue;
    fprintf(fd," min %d\n",(int)byteInt);

    int16 shortInt;
    intValue = 0x7fff;
    shortInt = intValue;
    fprintf(fd,"int8 max %hd",shortInt);
    intValue = 0x8000;
    shortInt = intValue;
    fprintf(fd," min %hd\n",shortInt);

    int32 intInt;
    intValue = 0x7fffffff;
    intInt = intValue;
    fprintf(fd,"int8 max %d",intInt);
    intValue = 0x80000000;
    intInt = intValue;
    fprintf(fd," min %d\n",intInt);

    int64 longInt = 0x7fffffff;
    longInt <<= 32;
    longInt |= 0xffffffff;
    fprintf(fd,"int8 max %lld",longInt);
    longInt = intValue = 0x80000000;;
    longInt <<= 32;
    fprintf(fd," min %lld\n",longInt);
    
    return(0);
}

