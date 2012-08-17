/* testPVScalarArray.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
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
    PVScalarArrayPtr pvScalarArray = pvDataCreate->createPVScalarArray(pvByte);;
    PVByteArrayPtr pvByteArray = static_pointer_cast<PVByteArray>(pvScalarArray);
    ByteArray value;
    value.reserve(length);
    int8 xxx = 0x7f;
    for(size_t i = 0; i<length; i++) value.push_back(xxx++);
    pvByteArray->put(0,length,value,0);
    builder.clear();
    pvByteArray->toString(&builder);
    fprintf(fd,"put\n%s\n",builder.c_str());
    convert->fromByteArray(pvScalarArray,0,length,value,0);
    builder.clear();
    pvByteArray->toString(&builder);
    fprintf(fd,"convert\n%s\n",builder.c_str());
    ByteArrayData data;
    pvByteArray->get(0,length,data);
    ByteArray_iterator iter = data.data.begin();
    fprintf(fd,"iter  [");
    for(iter=data.data.begin();iter!=data.data.end();++iter) {
         fprintf(fd,"%d ",*iter);
    }
    fprintf(fd,"]\n");
    fprintf(fd,"raw   [");
    int8 * pdata = get(data.data);
    for(size_t i=0; i<length; i++) {
         int val = pdata[i];
         fprintf(fd,"%d  ",val);
    }
    fprintf(fd,"]\n");
    fprintf(fd,"direct[");
    for(size_t i=0; i<length; i++) {
         int val = data.data[i];
         fprintf(fd,"%d ",val);
    }
    fprintf(fd,"]\n");
}

static void ubyteArray()
{
    PVScalarArrayPtr pvScalarArray = pvDataCreate->createPVScalarArray(pvUByte);;
    PVUByteArrayPtr pvUByteArray = static_pointer_cast<PVUByteArray>(pvScalarArray);
    UByteArray value;
    value.reserve(length);
    uint8 xxx = 0x7f;
    for(size_t i = 0; i<length; i++) value.push_back(xxx++);
    pvUByteArray->put(0,length,value,0);
    builder.clear();
    pvUByteArray->toString(&builder);
    fprintf(fd,"put\n%s\n",builder.c_str());
    convert->fromUByteArray(pvScalarArray,0,length,value,0);
    builder.clear();
    pvUByteArray->toString(&builder);
    fprintf(fd,"convert\n%s\n",builder.c_str());
    UByteArrayData data;
    pvUByteArray->get(0,length,data);
    UByteArray_iterator iter = data.data.begin();
    fprintf(fd,"iter  [");
    for(iter=data.data.begin();iter!=data.data.end();++iter) {
         fprintf(fd,"%u ",*iter);
    }
    fprintf(fd,"]\n");
    fprintf(fd,"raw   [");
    uint8 * pdata = get(data.data);
    for(size_t i=0; i<length; i++) {
         unsigned int val = pdata[i];
         fprintf(fd,"%d  ",val);
    }
    fprintf(fd,"]\n");
    fprintf(fd,"direct[");
    for(size_t i=0; i<length; i++) {
         unsigned int val = data.data[i];
         fprintf(fd,"%d ",val);
    }
    fprintf(fd,"]\n");
}

static void longArray()
{
    PVScalarArrayPtr pvScalarArray = pvDataCreate->createPVScalarArray(pvLong);;
    PVLongArrayPtr pvLongArray = static_pointer_cast<PVLongArray>(pvScalarArray);
    LongArray value;
    value.reserve(length);
    int64 xxx = 0x7fffffffffffffffL;
    for(size_t i = 0; i<length; i++) value.push_back(xxx++);
    pvLongArray->put(0,length,value,0);
    builder.clear();
    pvLongArray->toString(&builder);
    fprintf(fd,"put\n%s\n",builder.c_str());
    convert->fromLongArray(pvScalarArray,0,length,value,0);
    builder.clear();
    pvLongArray->toString(&builder);
    fprintf(fd,"convert\n%s\n",builder.c_str());
    LongArrayData data;
    pvLongArray->get(0,length,data);
    LongArray_iterator iter = data.data.begin();
    fprintf(fd,"iter  [");
    for(iter=data.data.begin();iter!=data.data.end();++iter) {
         fprintf(fd,"%lli ",*iter);
    }
    fprintf(fd,"]\n");
    fprintf(fd,"raw   [");
    int64 * pdata = get(data.data);
    for(size_t i=0; i<length; i++) {
         int64 val = pdata[i];
         fprintf(fd,"%lli  ",val);
    }
    fprintf(fd,"]\n");
    fprintf(fd,"direct[");
    for(size_t i=0; i<length; i++) {
         int64 val = data.data[i];
         fprintf(fd,"%lli ",val);
    }
    fprintf(fd,"]\n");
}

static void ulongArray()
{
    PVScalarArrayPtr pvScalarArray = pvDataCreate->createPVScalarArray(pvULong);;
    PVULongArrayPtr pvULongArray = static_pointer_cast<PVULongArray>(pvScalarArray);
    ULongArray value;
    value.reserve(length);
    uint64 xxx = 0x7fffffffffffffffL;
    for(size_t i = 0; i<length; i++) value.push_back(xxx++);
    pvULongArray->put(0,length,value,0);
    builder.clear();
    pvULongArray->toString(&builder);
    fprintf(fd,"put\n%s\n",builder.c_str());
    convert->fromULongArray(pvScalarArray,0,length,value,0);
    builder.clear();
    pvULongArray->toString(&builder);
    fprintf(fd,"convert\n%s\n",builder.c_str());
    ULongArrayData data;
    pvULongArray->get(0,length,data);
    ULongArray_iterator iter = data.data.begin();
    fprintf(fd,"iter  [");
    for(iter=data.data.begin();iter!=data.data.end();++iter) {
         fprintf(fd,"%llu ",*iter);
    }
    fprintf(fd,"]\n");
    fprintf(fd,"raw   [");
    uint64 * pdata = get(data.data);
    for(size_t i=0; i<length; i++) {
         uint64 val = pdata[i];
         fprintf(fd,"%llu  ",val);
    }
    fprintf(fd,"]\n");
    fprintf(fd,"direct[");
    for(size_t i=0; i<length; i++) {
         uint64 val = data.data[i];
         fprintf(fd,"%llu ",val);
    }
    fprintf(fd,"]\n");
}

static void floatArray()
{
    PVScalarArrayPtr pvScalarArray = pvDataCreate->createPVScalarArray(pvFloat);;
    PVFloatArrayPtr pvFloatArray = static_pointer_cast<PVFloatArray>(pvScalarArray);
    FloatArray value;
    value.reserve(length);
    for(size_t i = 0; i<length; i++) value.push_back(10.0*i);
    pvFloatArray->put(0,length,value,0);
    builder.clear();
    pvFloatArray->toString(&builder);
    fprintf(fd,"put\n%s\n",builder.c_str());
    convert->fromFloatArray(pvScalarArray,0,length,value,0);
    builder.clear();
    pvFloatArray->toString(&builder);
    fprintf(fd,"convert\n%s\n",builder.c_str());
    FloatArrayData data;
    pvFloatArray->get(0,length,data);
    FloatArray_iterator iter = data.data.begin();
    fprintf(fd,"iter  [");
    for(iter=data.data.begin();iter!=data.data.end();++iter) {
         fprintf(fd,"%f ",*iter);
    }
    fprintf(fd,"]\n");
    fprintf(fd,"raw   [");
    float * pdata = get(data.data);
    for(size_t i=0; i<length; i++) {
         float val = pdata[i];
         fprintf(fd,"%f  ",val);
    }
    fprintf(fd,"]\n");
    fprintf(fd,"direct[");
    for(size_t i=0; i<length; i++) {
         float val = data.data[i];
         fprintf(fd,"%f ",val);
    }
    fprintf(fd,"]\n");
}

static void doubleArray()
{
    PVScalarArrayPtr pvScalarArray = pvDataCreate->createPVScalarArray(pvDouble);;
    PVDoubleArrayPtr pvDoubleArray = static_pointer_cast<PVDoubleArray>(pvScalarArray);
    DoubleArray value;
    value.reserve(length);
    for(size_t i = 0; i<length; i++) value.push_back(10.0*i);
    pvDoubleArray->put(0,length,value,0);
    builder.clear();
    pvDoubleArray->toString(&builder);
    fprintf(fd,"put\n%s\n",builder.c_str());
    convert->fromDoubleArray(pvScalarArray,0,length,value,0);
    builder.clear();
    pvDoubleArray->toString(&builder);
    fprintf(fd,"convert\n%s\n",builder.c_str());
    DoubleArrayData data;
    pvDoubleArray->get(0,length,data);
    DoubleArray_iterator iter = data.data.begin();
    fprintf(fd,"iter  [");
    for(iter=data.data.begin();iter!=data.data.end();++iter) {
         fprintf(fd,"%lf ",*iter);
    }
    fprintf(fd,"]\n");
    fprintf(fd,"raw   [");
    double * pdata = get(data.data);
    for(size_t i=0; i<length; i++) {
         double val = pdata[i];
         fprintf(fd,"%lf  ",val);
    }
    fprintf(fd,"]\n");
    fprintf(fd,"direct[");
    for(size_t i=0; i<length; i++) {
         double val = data.data[i];
         fprintf(fd,"%lf ",val);
    }
    fprintf(fd,"]\n");
}

static void stringArray()
{
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
    fprintf(fd,"put\n%s\n",builder.c_str());
    convert->fromStringArray(pvScalarArray,0,length,value,0);
    builder.clear();
    pvStringArray->toString(&builder);
    fprintf(fd,"convert\n%s\n",builder.c_str());
    StringArrayData data;
    pvStringArray->get(0,length,data);
    StringArray_iterator iter = data.data.begin();
    fprintf(fd,"iter  [");
    for(iter=data.data.begin();iter!=data.data.end();++iter) {
         String val = *iter;
         fprintf(fd,"%s ",val.c_str());
    }
    fprintf(fd,"]\n");
    fprintf(fd,"raw   [");
    String*  pdata = get(data.data);
    for(size_t i=0; i<length; i++) {
         String val = pdata[i];
         fprintf(fd,"%s  ",val.c_str());
    }
    fprintf(fd,"]\n");
    fprintf(fd,"direct[");
    for(size_t i=0; i<length; i++) {
         String val = data.data[i];
         fprintf(fd,"%s ",val.c_str());
    }
    fprintf(fd,"]\n");
}

static void shareArray()
{
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
    fprintf(fd,"pvShare\n%s\n",builder.c_str());
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

