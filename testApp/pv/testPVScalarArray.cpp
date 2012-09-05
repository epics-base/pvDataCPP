/* testPVScalarArray.cpp */
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

static FieldCreatePtr fieldCreate = getFieldCreate();
static PVDataCreatePtr pvDataCreate = getPVDataCreate();
static StandardFieldPtr standardField = getStandardField();
static StandardPVFieldPtr standardPVField = getStandardPVField();
static ConvertPtr convert = getConvert();
static String builder;
static String alarmTimeStamp("alarm,timeStamp");
static String alarmTimeStampValueAlarm("alarm,timeStamp,valueAlarm");
static String allProperties("alarm,timeStamp,display,control,valueAlarm");
static FILE * fd = NULL;
static size_t length = 4;

static void byteArray()
{
    if(debug) fprintf(fd,"\nbyteArray\n");
    PVScalarArrayPtr pvScalarArray = pvDataCreate->createPVScalarArray(pvByte);;
    PVByteArrayPtr pvByteArray = static_pointer_cast<PVByteArray>(pvScalarArray);
    ByteArray value;
    value.reserve(length);
    int8 xxx = 0x7f;
    for(size_t i = 0; i<length; i++) value.push_back(xxx++);
    pvByteArray->put(0,length,value,0);
    builder.clear();
    pvByteArray->toString(&builder);
    if(debug) fprintf(fd,"put\n%s\n",builder.c_str());
    convert->fromByteArray(pvScalarArray,0,length,value,0);
    builder.clear();
    pvByteArray->toString(&builder);
    if(debug) fprintf(fd,"convert\n%s\n",builder.c_str());
    ByteArrayData data;
    pvByteArray->get(0,length,data);
    ByteArray_iterator iter = data.data.begin();
    if(debug) fprintf(fd,"iter  [");
    for(iter=data.data.begin();iter!=data.data.end();++iter) {
         if(debug) fprintf(fd,"%d ",*iter);
    }
    if(debug) fprintf(fd,"]\n");
    if(debug) fprintf(fd,"raw   [");
    int8 * pdata = get(data.data);
    for(size_t i=0; i<length; i++) {
         int val = pdata[i];
         if(debug) fprintf(fd,"%d  ",val);
    }
    if(debug) fprintf(fd,"]\n");
    if(debug) fprintf(fd,"direct[");
    for(size_t i=0; i<length; i++) {
         int val = data.data[i];
         if(debug) fprintf(fd,"%d ",val);
    }
    if(debug) fprintf(fd,"]\n");
    fprintf(fd,"byteArray PASSED\n");
}

static void ubyteArray()
{
    if(debug) fprintf(fd,"\nubyteArray\n");
    PVScalarArrayPtr pvScalarArray = pvDataCreate->createPVScalarArray(pvUByte);;
    PVUByteArrayPtr pvUByteArray = static_pointer_cast<PVUByteArray>(pvScalarArray);
    UByteArray value;
    value.reserve(length);
    uint8 xxx = 0x7f;
    for(size_t i = 0; i<length; i++) value.push_back(xxx++);
    pvUByteArray->put(0,length,value,0);
    builder.clear();
    pvUByteArray->toString(&builder);
    if(debug) fprintf(fd,"put\n%s\n",builder.c_str());
    convert->fromUByteArray(pvScalarArray,0,length,value,0);
    builder.clear();
    pvUByteArray->toString(&builder);
    if(debug) fprintf(fd,"convert\n%s\n",builder.c_str());
    UByteArrayData data;
    pvUByteArray->get(0,length,data);
    UByteArray_iterator iter = data.data.begin();
    if(debug) fprintf(fd,"iter  [");
    for(iter=data.data.begin();iter!=data.data.end();++iter) {
         if(debug) fprintf(fd,"%u ",*iter);
    }
    if(debug) fprintf(fd,"]\n");
    if(debug) fprintf(fd,"raw   [");
    uint8 * pdata = get(data.data);
    for(size_t i=0; i<length; i++) {
         unsigned int val = pdata[i];
         if(debug) fprintf(fd,"%d  ",val);
    }
    if(debug) fprintf(fd,"]\n");
    if(debug) fprintf(fd,"direct[");
    for(size_t i=0; i<length; i++) {
         unsigned int val = data.data[i];
         if(debug) fprintf(fd,"%d ",val);
    }
    if(debug) fprintf(fd,"]\n");
    fprintf(fd,"ubyteArray PASSED\n");
}

static void longArray()
{
    if(debug) fprintf(fd,"\nlongArray\n");
    PVScalarArrayPtr pvScalarArray = pvDataCreate->createPVScalarArray(pvLong);;
    PVLongArrayPtr pvLongArray = static_pointer_cast<PVLongArray>(pvScalarArray);
    LongArray value;
    value.reserve(length);
    int64 xxx = 0x7fffffffffffffffLL;
    for(size_t i = 0; i<length; i++) value.push_back(xxx++);
    pvLongArray->put(0,length,value,0);
    builder.clear();
    pvLongArray->toString(&builder);
    if(debug) fprintf(fd,"put\n%s\n",builder.c_str());
    convert->fromLongArray(pvScalarArray,0,length,value,0);
    builder.clear();
    pvLongArray->toString(&builder);
    if(debug) fprintf(fd,"convert\n%s\n",builder.c_str());
    LongArrayData data;
    pvLongArray->get(0,length,data);
    LongArray_iterator iter = data.data.begin();
    if(debug) fprintf(fd,"iter  [");
    for(iter=data.data.begin();iter!=data.data.end();++iter) {
         if(debug) fprintf(fd,"%lli ",*iter);
    }
    if(debug) fprintf(fd,"]\n");
    if(debug) fprintf(fd,"raw   [");
    int64 * pdata = get(data.data);
    for(size_t i=0; i<length; i++) {
         int64 val = pdata[i];
         if(debug) fprintf(fd,"%lli  ",val);
    }
    if(debug) fprintf(fd,"]\n");
    if(debug) fprintf(fd,"direct[");
    for(size_t i=0; i<length; i++) {
         int64 val = data.data[i];
         if(debug) fprintf(fd,"%lli ",val);
    }
    if(debug) fprintf(fd,"]\n");
    fprintf(fd,"longArray PASSED\n");
}

static void ulongArray()
{
    if(debug) fprintf(fd,"\nulongArray\n");
    PVScalarArrayPtr pvScalarArray = pvDataCreate->createPVScalarArray(pvULong);;
    PVULongArrayPtr pvULongArray = static_pointer_cast<PVULongArray>(pvScalarArray);
    ULongArray value;
    value.reserve(length);
    uint64 xxx = 0x7fffffffffffffffLL;
    for(size_t i = 0; i<length; i++) value.push_back(xxx++);
    pvULongArray->put(0,length,value,0);
    builder.clear();
    pvULongArray->toString(&builder);
    if(debug) fprintf(fd,"put\n%s\n",builder.c_str());
    convert->fromULongArray(pvScalarArray,0,length,value,0);
    builder.clear();
    pvULongArray->toString(&builder);
    if(debug) fprintf(fd,"convert\n%s\n",builder.c_str());
    ULongArrayData data;
    pvULongArray->get(0,length,data);
    ULongArray_iterator iter = data.data.begin();
    if(debug) fprintf(fd,"iter  [");
    for(iter=data.data.begin();iter!=data.data.end();++iter) {
         if(debug) fprintf(fd,"%llu ",*iter);
    }
    if(debug) fprintf(fd,"]\n");
    if(debug) fprintf(fd,"raw   [");
    uint64 * pdata = get(data.data);
    for(size_t i=0; i<length; i++) {
         uint64 val = pdata[i];
         if(debug) fprintf(fd,"%llu  ",val);
    }
    if(debug) fprintf(fd,"]\n");
    if(debug) fprintf(fd,"direct[");
    for(size_t i=0; i<length; i++) {
         uint64 val = data.data[i];
         if(debug) fprintf(fd,"%llu ",val);
    }
    if(debug) fprintf(fd,"]\n");
    fprintf(fd,"ulongArray PASSED\n");
}

static void floatArray()
{
    if(debug) fprintf(fd,"\nfloatArray\n");
    PVScalarArrayPtr pvScalarArray = pvDataCreate->createPVScalarArray(pvFloat);;
    PVFloatArrayPtr pvFloatArray = static_pointer_cast<PVFloatArray>(pvScalarArray);
    FloatArray value;
    value.reserve(length);
    for(size_t i = 0; i<length; i++) value.push_back(10.0*i);
    pvFloatArray->put(0,length,value,0);
    builder.clear();
    pvFloatArray->toString(&builder);
    if(debug) fprintf(fd,"put\n%s\n",builder.c_str());
    convert->fromFloatArray(pvScalarArray,0,length,value,0);
    builder.clear();
    pvFloatArray->toString(&builder);
    if(debug) fprintf(fd,"convert\n%s\n",builder.c_str());
    FloatArrayData data;
    pvFloatArray->get(0,length,data);
    FloatArray_iterator iter = data.data.begin();
    if(debug) fprintf(fd,"iter  [");
    for(iter=data.data.begin();iter!=data.data.end();++iter) {
         if(debug) fprintf(fd,"%f ",*iter);
    }
    if(debug) fprintf(fd,"]\n");
    if(debug) fprintf(fd,"raw   [");
    float * pdata = get(data.data);
    for(size_t i=0; i<length; i++) {
         float val = pdata[i];
         if(debug) fprintf(fd,"%f  ",val);
    }
    if(debug) fprintf(fd,"]\n");
    if(debug) fprintf(fd,"direct[");
    for(size_t i=0; i<length; i++) {
         float val = data.data[i];
         if(debug) fprintf(fd,"%f ",val);
    }
    if(debug) fprintf(fd,"]\n");
    fprintf(fd,"floatArray PASSED\n");
}

static void doubleArray()
{
    if(debug) fprintf(fd,"\ndoubleArray\n");
    PVScalarArrayPtr pvScalarArray = pvDataCreate->createPVScalarArray(pvDouble);;
    PVDoubleArrayPtr pvDoubleArray = static_pointer_cast<PVDoubleArray>(pvScalarArray);
    DoubleArray value;
    value.reserve(length);
    for(size_t i = 0; i<length; i++) value.push_back(10.0*i);
    pvDoubleArray->put(0,length,value,0);
    builder.clear();
    pvDoubleArray->toString(&builder);
    if(debug) fprintf(fd,"put\n%s\n",builder.c_str());
    convert->fromDoubleArray(pvScalarArray,0,length,value,0);
    builder.clear();
    pvDoubleArray->toString(&builder);
    if(debug) fprintf(fd,"convert\n%s\n",builder.c_str());
    DoubleArrayData data;
    pvDoubleArray->get(0,length,data);
    DoubleArray_iterator iter = data.data.begin();
    if(debug) fprintf(fd,"iter  [");
    for(iter=data.data.begin();iter!=data.data.end();++iter) {
         if(debug) fprintf(fd,"%lf ",*iter);
    }
    if(debug) fprintf(fd,"]\n");
    if(debug) fprintf(fd,"raw   [");
    double * pdata = get(data.data);
    for(size_t i=0; i<length; i++) {
         double val = pdata[i];
         if(debug) fprintf(fd,"%lf  ",val);
    }
    if(debug) fprintf(fd,"]\n");
    if(debug) fprintf(fd,"direct[");
    for(size_t i=0; i<length; i++) {
         double val = data.data[i];
         if(debug) fprintf(fd,"%lf ",val);
    }
    if(debug) fprintf(fd,"]\n");
    fprintf(fd,"doubleArray PASSED\n");
}

static void stringArray()
{
    if(debug) fprintf(fd,"\nstringArray\n");
    PVScalarArrayPtr pvScalarArray = pvDataCreate->createPVScalarArray(pvString);;
    PVStringArrayPtr pvStringArray = static_pointer_cast<PVStringArray>(pvScalarArray);
    StringArray value;
    value.reserve(length);
    for(size_t i = 0; i<length; i++) {
        char val[20];
        sprintf(val,"value%d",(int)i);
        value.push_back(val);
    }
    pvStringArray->put(0,length,value,0);
    builder.clear();
    pvStringArray->toString(&builder);
    if(debug) fprintf(fd,"put\n%s\n",builder.c_str());
    convert->fromStringArray(pvScalarArray,0,length,value,0);
    builder.clear();
    pvStringArray->toString(&builder);
    if(debug) fprintf(fd,"convert\n%s\n",builder.c_str());
    StringArrayData data;
    pvStringArray->get(0,length,data);
    StringArray_iterator iter = data.data.begin();
    if(debug) fprintf(fd,"iter  [");
    for(iter=data.data.begin();iter!=data.data.end();++iter) {
         String val = *iter;
         if(debug) fprintf(fd,"%s ",val.c_str());
    }
    if(debug) fprintf(fd,"]\n");
    if(debug) fprintf(fd,"raw   [");
    String*  pdata = get(data.data);
    for(size_t i=0; i<length; i++) {
         String val = pdata[i];
         if(debug) fprintf(fd,"%s  ",val.c_str());
    }
    if(debug) fprintf(fd,"]\n");
    if(debug) fprintf(fd,"direct[");
    for(size_t i=0; i<length; i++) {
         String val = data.data[i];
         if(debug) fprintf(fd,"%s ",val.c_str());
    }
    if(debug) fprintf(fd,"]\n");
    fprintf(fd,"stringArray PASSED\n");
}

static void shareArray()
{
    if(debug) fprintf(fd,"\nshareArray\n");
    PVScalarArrayPtr pvScalarArray = pvDataCreate->createPVScalarArray(pvDouble);;
    PVDoubleArrayPtr pvDoubleArray = static_pointer_cast<PVDoubleArray>(pvScalarArray);
    DoubleArray value;
    value.reserve(length);
    for(size_t i = 0; i<length; i++) value.push_back(10.0*i);
    pvDoubleArray->put(0,length,value,0);
    PVDoubleArrayPtr pvShareArray = static_pointer_cast<PVDoubleArray>(
          pvDataCreate->createPVScalarArray(pvDouble));
    pvShareArray->shareData(
        pvDoubleArray->getSharedVector(),
        pvDoubleArray->getCapacity(),
        pvDoubleArray->getLength());
    printf("pvDoubleArray->get() %p pvShareArray->get() %p\n",pvDoubleArray->get(),pvShareArray->get());
    printf("pvDoubleArray->getVector() %p pvShareArray->getVector() %p\n",
        &(pvDoubleArray->getVector()),&(pvShareArray->getVector()));
    printf("pvDoubleArray->getSharedVector() %p pvShareArray->getSharedVector() %p\n",
        &(pvDoubleArray->getSharedVector()),&(pvShareArray->getSharedVector()));
    assert(pvDoubleArray->get()==pvShareArray->get());
    builder.clear();
    pvShareArray->toString(&builder);
    if(debug) fprintf(fd,"pvShare\n%s\n",builder.c_str());
    fprintf(fd,"shareArray PASSED\n");
}

int main(int argc,char *argv[])
{
    char *fileName = 0;
    if(argc>1) fileName = argv[1];
    fd = stdout;
    if(fileName!=0 && fileName[0]!=0) {
        fd = fopen(fileName,"w+");
    }
    byteArray();
    ubyteArray();
    longArray();
    ulongArray();
    floatArray();
    doubleArray();
    stringArray();
    shareArray();
    return(0);
}

