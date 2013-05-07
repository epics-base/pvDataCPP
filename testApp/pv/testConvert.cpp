/* testConvert.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/* Author:  Marty Kraimer Date: 2010.11 */

#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>

#include <epicsAssert.h>
#include <epicsExit.h>

#include <pv/requester.h>
#include <pv/pvIntrospect.h>
#include <pv/pvData.h>
#include <pv/convert.h>
#include <pv/standardField.h>
#include <pv/standardPVField.h>

using namespace epics::pvData;
using std::tr1::static_pointer_cast;

static bool debug = false;

static FieldCreatePtr fieldCreate;
static PVDataCreatePtr pvDataCreate;
static StandardFieldPtr standardField;
static StandardPVFieldPtr standardPVField;
static ConvertPtr convert;
static String builder("");

static void testConvertScalar(FILE *fd) {
    PVScalarPtr pvBytePtr = pvDataCreate->createPVScalar(pvByte);
    PVScalarPtr pvUBytePtr = pvDataCreate->createPVScalar(pvUByte);
    PVScalarPtr pvShortPtr = pvDataCreate->createPVScalar(pvShort);
    PVScalarPtr pvUShortPtr = pvDataCreate->createPVScalar(pvUShort);
    PVScalarPtr pvIntPtr = pvDataCreate->createPVScalar(pvInt);
    PVScalarPtr pvUIntPtr = pvDataCreate->createPVScalar(pvUInt);
    PVScalarPtr pvLongPtr = pvDataCreate->createPVScalar(pvLong);
    PVScalarPtr pvULongPtr = pvDataCreate->createPVScalar(pvULong);
    PVScalarPtr pvFloatPtr = pvDataCreate->createPVScalar(pvFloat);
    PVScalarPtr pvDoublePtr = pvDataCreate->createPVScalar(pvDouble);

    fprintf(fd,"testConvertScalar\n");
    if(debug) fprintf(fd,"\nfromByte\n");
    int8 bval = 127;
    for(int i=0; i<3; i++) {
        convert->fromByte(pvBytePtr, bval);
        builder.clear(); pvBytePtr->toString(&builder);
        if(debug) fprintf(fd,"byte %s\n",builder.c_str());
        convert->fromByte(pvUBytePtr, bval);
        builder.clear(); pvUBytePtr->toString(&builder);
        if(debug) fprintf(fd,"ubyte %s\n",builder.c_str());
        convert->fromByte(pvShortPtr, bval);
        builder.clear(); pvShortPtr->toString(&builder);
        if(debug) fprintf(fd,"short %s\n",builder.c_str());
        convert->fromByte(pvUShortPtr, bval);
        builder.clear(); pvUShortPtr->toString(&builder);
        if(debug) fprintf(fd,"ushort %s\n",builder.c_str());
        convert->fromByte(pvIntPtr, bval);
        builder.clear(); pvIntPtr->toString(&builder);
        if(debug) fprintf(fd,"int %s\n",builder.c_str());
        convert->fromByte(pvUIntPtr, bval);
        builder.clear(); pvUIntPtr->toString(&builder);
        if(debug) fprintf(fd,"uint %s\n",builder.c_str());
        convert->fromByte(pvLongPtr, bval);
        builder.clear(); pvLongPtr->toString(&builder);
        if(debug) fprintf(fd,"long %s\n",builder.c_str());
        convert->fromByte(pvULongPtr, bval);
        builder.clear(); pvULongPtr->toString(&builder);
        if(debug) fprintf(fd,"ulong %s\n",builder.c_str());
        convert->fromByte(pvFloatPtr, bval);
        builder.clear(); pvFloatPtr->toString(&builder);
        if(debug) fprintf(fd,"float %s\n",builder.c_str());
        convert->fromByte(pvDoublePtr, bval);
        builder.clear(); pvDoublePtr->toString(&builder);
        if(debug) fprintf(fd,"double %s\n",builder.c_str());
        convert->copyScalar(pvUBytePtr, pvFloatPtr);
        builder.clear(); pvFloatPtr->toString(&builder);
        if(debug) fprintf(fd,"float from unsigned %s\n",builder.c_str());
        convert->copyScalar(pvUBytePtr, pvDoublePtr);
        builder.clear(); pvDoublePtr->toString(&builder);
        if(debug) fprintf(fd,"double from unsigned %s\n",builder.c_str());
        bval++;
    }
    fprintf(fd,"fromByte PASSED\n");

    if(debug) fprintf(fd,"\nfromShort\n");
    int16 sval = 0x7fff;
    for(int i=0; i<3; i++) {
        convert->fromShort(pvBytePtr, sval);
        builder.clear(); pvBytePtr->toString(&builder);
        if(debug) fprintf(fd,"byte %s\n",builder.c_str());
        convert->fromShort(pvUBytePtr, sval);
        builder.clear(); pvUBytePtr->toString(&builder);
        if(debug) fprintf(fd,"ubyte %s\n",builder.c_str());
        convert->fromShort(pvShortPtr, sval);
        builder.clear(); pvShortPtr->toString(&builder);
        if(debug) fprintf(fd,"short %s\n",builder.c_str());
        convert->fromShort(pvUShortPtr, sval);
        builder.clear(); pvUShortPtr->toString(&builder);
        if(debug) fprintf(fd,"ushort %s\n",builder.c_str());
        convert->fromShort(pvIntPtr, sval);
        builder.clear(); pvIntPtr->toString(&builder);
        if(debug) fprintf(fd,"int %s\n",builder.c_str());
        convert->fromShort(pvUIntPtr, sval);
        builder.clear(); pvUIntPtr->toString(&builder);
        if(debug) fprintf(fd,"uint %s\n",builder.c_str());
        convert->fromShort(pvLongPtr, sval);
        builder.clear(); pvLongPtr->toString(&builder);
        if(debug) fprintf(fd,"long %s\n",builder.c_str());
        convert->fromShort(pvULongPtr, sval);
        builder.clear(); pvULongPtr->toString(&builder);
        if(debug) fprintf(fd,"ulong %s\n",builder.c_str());
        convert->fromShort(pvFloatPtr, sval);
        builder.clear(); pvFloatPtr->toString(&builder);
        if(debug) fprintf(fd,"float %s\n",builder.c_str());
        convert->fromShort(pvDoublePtr, sval);
        builder.clear(); pvDoublePtr->toString(&builder);
        if(debug) fprintf(fd,"double %s\n",builder.c_str());
        convert->copyScalar(pvUShortPtr, pvFloatPtr);
        builder.clear(); pvFloatPtr->toString(&builder);
        if(debug) fprintf(fd,"float from unsigned %s\n",builder.c_str());
        convert->copyScalar(pvUShortPtr, pvDoublePtr);
        builder.clear(); pvDoublePtr->toString(&builder);
        if(debug) fprintf(fd,"double from unsigned %s\n",builder.c_str());
        sval++;
    }
    fprintf(fd,"fromShort PASSED\n");

    if(debug) fprintf(fd,"\nfromInt\n");
    int32 ival = 0x7fffffff;
    for(int i=0; i<3; i++) {
        convert->fromInt(pvBytePtr, ival);
        builder.clear(); pvBytePtr->toString(&builder);
        if(debug) fprintf(fd,"byte %s\n",builder.c_str());
        convert->fromInt(pvUBytePtr, ival);
        builder.clear(); pvUBytePtr->toString(&builder);
        if(debug) fprintf(fd,"ubyte %s\n",builder.c_str());
        convert->fromInt(pvShortPtr, ival);
        builder.clear(); pvShortPtr->toString(&builder);
        if(debug) fprintf(fd,"short %s\n",builder.c_str());
        convert->fromInt(pvUShortPtr, ival);
        builder.clear(); pvUShortPtr->toString(&builder);
        if(debug) fprintf(fd,"ushort %s\n",builder.c_str());
        convert->fromInt(pvIntPtr, ival);
        builder.clear(); pvIntPtr->toString(&builder);
        if(debug) fprintf(fd,"int %s\n",builder.c_str());
        convert->fromInt(pvUIntPtr, ival);
        builder.clear(); pvUIntPtr->toString(&builder);
        if(debug) fprintf(fd,"uint %s\n",builder.c_str());
        convert->fromInt(pvLongPtr, ival);
        builder.clear(); pvLongPtr->toString(&builder);
        if(debug) fprintf(fd,"long %s\n",builder.c_str());
        convert->fromInt(pvULongPtr, ival);
        builder.clear(); pvULongPtr->toString(&builder);
        if(debug) fprintf(fd,"ulong %s\n",builder.c_str());
        convert->fromInt(pvFloatPtr, ival);
        builder.clear(); pvFloatPtr->toString(&builder);
        if(debug) fprintf(fd,"float %s\n",builder.c_str());
        convert->fromInt(pvDoublePtr, ival);
        builder.clear(); pvDoublePtr->toString(&builder);
        if(debug) fprintf(fd,"double %s\n",builder.c_str());
        convert->copyScalar(pvUIntPtr, pvFloatPtr);
        builder.clear(); pvFloatPtr->toString(&builder);
        if(debug) fprintf(fd,"float from unsigned %s\n",builder.c_str());
        convert->copyScalar(pvUIntPtr, pvDoublePtr);
        builder.clear(); pvDoublePtr->toString(&builder);
        if(debug) fprintf(fd,"double from unsigned %s\n",builder.c_str());
        ival++;
    }
    fprintf(fd,"fromInt PASSED\n");

    if(debug) fprintf(fd,"\nfromLong\n");
    int64 lval = 0x7fffffffffffffffLL;
    for(int i=0; i<3; i++) {
        convert->fromLong(pvBytePtr, lval);
        builder.clear(); pvBytePtr->toString(&builder);
        if(debug) fprintf(fd,"byte %s\n",builder.c_str());
        convert->fromLong(pvUBytePtr, lval);
        builder.clear(); pvUBytePtr->toString(&builder);
        if(debug) fprintf(fd,"ubyte %s\n",builder.c_str());
        convert->fromLong(pvShortPtr, lval);
        builder.clear(); pvShortPtr->toString(&builder);
        if(debug) fprintf(fd,"short %s\n",builder.c_str());
        convert->fromLong(pvUShortPtr, lval);
        builder.clear(); pvUShortPtr->toString(&builder);
        if(debug) fprintf(fd,"ushort %s\n",builder.c_str());
        convert->fromLong(pvIntPtr, lval);
        builder.clear(); pvIntPtr->toString(&builder);
        if(debug) fprintf(fd,"int %s\n",builder.c_str());
        convert->fromLong(pvUIntPtr, lval);
        builder.clear(); pvUIntPtr->toString(&builder);
        if(debug) fprintf(fd,"uint %s\n",builder.c_str());
        convert->fromLong(pvLongPtr, lval);
        builder.clear(); pvLongPtr->toString(&builder);
        if(debug) fprintf(fd,"long %s\n",builder.c_str());
        convert->fromLong(pvULongPtr, lval);
        builder.clear(); pvULongPtr->toString(&builder);
        if(debug) fprintf(fd,"ulong %s\n",builder.c_str());
        convert->fromLong(pvFloatPtr, lval);
        builder.clear(); pvFloatPtr->toString(&builder);
        if(debug) fprintf(fd,"float %s\n",builder.c_str());
        convert->fromLong(pvDoublePtr, lval);
        builder.clear(); pvDoublePtr->toString(&builder);
        if(debug) fprintf(fd,"double %s\n",builder.c_str());
        convert->copyScalar(pvULongPtr, pvFloatPtr);
        builder.clear(); pvFloatPtr->toString(&builder);
        if(debug) fprintf(fd,"float from unsigned %s\n",builder.c_str());
        convert->copyScalar(pvULongPtr, pvDoublePtr);
        builder.clear(); pvDoublePtr->toString(&builder);
        if(debug) fprintf(fd,"double from unsigned %s\n",builder.c_str());
        lval++;
    }
    fprintf(fd,"fromLong PASSED\n");

    if(debug) fprintf(fd,"\nfromUByte\n");
    uint8 ubval = 127;
    for(int i=0; i<3; i++) {
        convert->fromUByte(pvBytePtr, ubval);
        builder.clear(); pvBytePtr->toString(&builder);
        if(debug) fprintf(fd,"byte %s\n",builder.c_str());
        convert->fromUByte(pvUBytePtr, ubval);
        builder.clear(); pvUBytePtr->toString(&builder);
        if(debug) fprintf(fd,"ubyte %s\n",builder.c_str());
        convert->fromUByte(pvShortPtr, ubval);
        builder.clear(); pvShortPtr->toString(&builder);
        if(debug) fprintf(fd,"short %s\n",builder.c_str());
        convert->fromUByte(pvUShortPtr, ubval);
        builder.clear(); pvUShortPtr->toString(&builder);
        if(debug) fprintf(fd,"ushort %s\n",builder.c_str());
        convert->fromUByte(pvIntPtr, ubval);
        builder.clear(); pvIntPtr->toString(&builder);
        if(debug) fprintf(fd,"int %s\n",builder.c_str());
        convert->fromUByte(pvUIntPtr, ubval);
        builder.clear(); pvUIntPtr->toString(&builder);
        if(debug) fprintf(fd,"uint %s\n",builder.c_str());
        convert->fromUByte(pvLongPtr, ubval);
        builder.clear(); pvLongPtr->toString(&builder);
        if(debug) fprintf(fd,"long %s\n",builder.c_str());
        convert->fromUByte(pvULongPtr, ubval);
        builder.clear(); pvULongPtr->toString(&builder);
        if(debug) fprintf(fd,"ulong %s\n",builder.c_str());
        convert->fromUByte(pvFloatPtr, ubval);
        builder.clear(); pvFloatPtr->toString(&builder);
        if(debug) fprintf(fd,"float %s\n",builder.c_str());
        convert->fromUByte(pvDoublePtr, ubval);
        builder.clear(); pvDoublePtr->toString(&builder);
        if(debug) fprintf(fd,"double %s\n",builder.c_str());
        convert->copyScalar(pvUBytePtr, pvFloatPtr);
        builder.clear(); pvFloatPtr->toString(&builder);
        if(debug) fprintf(fd,"float from unsigned %s\n",builder.c_str());
        convert->copyScalar(pvUBytePtr, pvDoublePtr);
        builder.clear(); pvDoublePtr->toString(&builder);
        if(debug) fprintf(fd,"double from unsigned %s\n",builder.c_str());
        ubval++;
    }
    fprintf(fd,"fromUByte PASSED\n");

    if(debug) fprintf(fd,"\nfromUShort\n");
    uint16 usval = 0x7fff;
    for(int i=0; i<3; i++) {
        convert->fromUShort(pvBytePtr, usval);
        builder.clear(); pvBytePtr->toString(&builder);
        if(debug) fprintf(fd,"byte %s\n",builder.c_str());
        convert->fromUShort(pvUBytePtr, usval);
        builder.clear(); pvUBytePtr->toString(&builder);
        if(debug) fprintf(fd,"ubyte %s\n",builder.c_str());
        convert->fromUShort(pvShortPtr, usval);
        builder.clear(); pvShortPtr->toString(&builder);
        if(debug) fprintf(fd,"short %s\n",builder.c_str());
        convert->fromUShort(pvUShortPtr, usval);
        builder.clear(); pvUShortPtr->toString(&builder);
        if(debug) fprintf(fd,"ushort %s\n",builder.c_str());
        convert->fromUShort(pvIntPtr, usval);
        builder.clear(); pvIntPtr->toString(&builder);
        if(debug) fprintf(fd,"int %s\n",builder.c_str());
        convert->fromUShort(pvUIntPtr, usval);
        builder.clear(); pvUIntPtr->toString(&builder);
        if(debug) fprintf(fd,"uint %s\n",builder.c_str());
        convert->fromUShort(pvLongPtr, usval);
        builder.clear(); pvLongPtr->toString(&builder);
        if(debug) fprintf(fd,"long %s\n",builder.c_str());
        convert->fromUShort(pvULongPtr, usval);
        builder.clear(); pvULongPtr->toString(&builder);
        if(debug) fprintf(fd,"ulong %s\n",builder.c_str());
        convert->fromUShort(pvFloatPtr, usval);
        builder.clear(); pvFloatPtr->toString(&builder);
        if(debug) fprintf(fd,"float %s\n",builder.c_str());
        convert->fromUShort(pvDoublePtr, usval);
        builder.clear(); pvDoublePtr->toString(&builder);
        if(debug) fprintf(fd,"double %s\n",builder.c_str());
        convert->copyScalar(pvUShortPtr, pvFloatPtr);
        builder.clear(); pvFloatPtr->toString(&builder);
        if(debug) fprintf(fd,"float from unsigned %s\n",builder.c_str());
        convert->copyScalar(pvUShortPtr, pvDoublePtr);
        builder.clear(); pvDoublePtr->toString(&builder);
        if(debug) fprintf(fd,"double from unsigned %s\n",builder.c_str());
        usval++;
    }
    fprintf(fd,"fromUShort PASSED\n");

    if(debug) fprintf(fd,"\nfromUInt\n");
    uint32 uival = 0x7fffffff;
    for(int i=0; i<3; i++) {
        convert->fromUInt(pvBytePtr, uival);
        builder.clear(); pvBytePtr->toString(&builder);
        if(debug) fprintf(fd,"byte %s\n",builder.c_str());
        convert->fromUInt(pvUBytePtr, uival);
        builder.clear(); pvUBytePtr->toString(&builder);
        if(debug) fprintf(fd,"ubyte %s\n",builder.c_str());
        convert->fromUInt(pvShortPtr, uival);
        builder.clear(); pvShortPtr->toString(&builder);
        if(debug) fprintf(fd,"short %s\n",builder.c_str());
        convert->fromUInt(pvUShortPtr, uival);
        builder.clear(); pvUShortPtr->toString(&builder);
        if(debug) fprintf(fd,"ushort %s\n",builder.c_str());
        convert->fromUInt(pvIntPtr, uival);
        builder.clear(); pvIntPtr->toString(&builder);
        if(debug) fprintf(fd,"int %s\n",builder.c_str());
        convert->fromUInt(pvUIntPtr, uival);
        builder.clear(); pvUIntPtr->toString(&builder);
        if(debug) fprintf(fd,"uint %s\n",builder.c_str());
        convert->fromUInt(pvLongPtr, uival);
        builder.clear(); pvLongPtr->toString(&builder);
        if(debug) fprintf(fd,"long %s\n",builder.c_str());
        convert->fromUInt(pvULongPtr, uival);
        builder.clear(); pvULongPtr->toString(&builder);
        if(debug) fprintf(fd,"ulong %s\n",builder.c_str());
        convert->fromUInt(pvFloatPtr, uival);
        builder.clear(); pvFloatPtr->toString(&builder);
        if(debug) fprintf(fd,"float %s\n",builder.c_str());
        convert->fromUInt(pvDoublePtr, uival);
        builder.clear(); pvDoublePtr->toString(&builder);
        if(debug) fprintf(fd,"double %s\n",builder.c_str());
        convert->copyScalar(pvUIntPtr, pvFloatPtr);
        builder.clear(); pvFloatPtr->toString(&builder);
        if(debug) fprintf(fd,"float from unsigned %s\n",builder.c_str());
        convert->copyScalar(pvUIntPtr, pvDoublePtr);
        builder.clear(); pvDoublePtr->toString(&builder);
        if(debug) fprintf(fd,"double from unsigned %s\n",builder.c_str());
        uival++;
    }
    fprintf(fd,"fromUInt PASSED\n");

    if(debug) fprintf(fd,"\nfromULong\n");
    uint64 ulval = 0x7fffffffffffffffLL;
    for(int i=0; i<3; i++) {
        convert->fromULong(pvBytePtr, ulval);
        builder.clear(); pvBytePtr->toString(&builder);
        if(debug) fprintf(fd,"byte %s\n",builder.c_str());
        convert->fromULong(pvUBytePtr, ulval);
        builder.clear(); pvUBytePtr->toString(&builder);
        if(debug) fprintf(fd,"ubyte %s\n",builder.c_str());
        convert->fromULong(pvShortPtr, ulval);
        builder.clear(); pvShortPtr->toString(&builder);
        if(debug) fprintf(fd,"short %s\n",builder.c_str());
        convert->fromULong(pvUShortPtr, ulval);
        builder.clear(); pvUShortPtr->toString(&builder);
        if(debug) fprintf(fd,"ushort %s\n",builder.c_str());
        convert->fromULong(pvIntPtr, ulval);
        builder.clear(); pvIntPtr->toString(&builder);
        if(debug) fprintf(fd,"int %s\n",builder.c_str());
        convert->fromULong(pvUIntPtr, ulval);
        builder.clear(); pvUIntPtr->toString(&builder);
        if(debug) fprintf(fd,"uint %s\n",builder.c_str());
        convert->fromULong(pvLongPtr, ulval);
        builder.clear(); pvLongPtr->toString(&builder);
        if(debug) fprintf(fd,"long %s\n",builder.c_str());
        convert->fromULong(pvULongPtr, ulval);
        builder.clear(); pvULongPtr->toString(&builder);
        if(debug) fprintf(fd,"ulong %s\n",builder.c_str());
        convert->fromULong(pvFloatPtr, ulval);
        builder.clear(); pvFloatPtr->toString(&builder);
        if(debug) fprintf(fd,"float %s\n",builder.c_str());
        convert->fromULong(pvDoublePtr, ulval);
        builder.clear(); pvDoublePtr->toString(&builder);
        if(debug) fprintf(fd,"double %s\n",builder.c_str());
        convert->copyScalar(pvULongPtr, pvFloatPtr);
        builder.clear(); pvFloatPtr->toString(&builder);
        if(debug) fprintf(fd,"float from unsigned %s\n",builder.c_str());
        convert->copyScalar(pvULongPtr, pvDoublePtr);
        builder.clear(); pvDoublePtr->toString(&builder);
        if(debug) fprintf(fd,"double from unsigned %s\n",builder.c_str());
        ulval++;
    }
    fprintf(fd,"fromULong PASSED\n");
}

int main(int argc,char *argv[])
{
    char *fileName = 0;
    if(argc>1) fileName = argv[1];
    FILE * fd = stdout;
    if(fileName!=0 && fileName[0]!=0) {
        fd = fopen(fileName,"w+");
    }
    fieldCreate = getFieldCreate();
    pvDataCreate = getPVDataCreate();
    standardField = getStandardField();
    standardPVField = getStandardPVField();
    convert = getConvert();
    testConvertScalar(fd);
    fprintf(fd,"THIS NEEDS MANY MORE TESTS AND ASSERTS\n");
    return(0);
}

