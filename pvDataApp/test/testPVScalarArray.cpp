/* testPVArray.cpp */
/* Author:  Marty Kraimer Date: 2010.09 */

#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "requester.h"
#include "pvIntrospect.h"
#include "pvData.h"
#include "standardField.h"
#include "standardPVField.h"

using namespace epics::pvData;

static FieldCreate * pfieldCreate = 0;
static PVDataCreate *pvDataCreate = 0;
static String buffer("");

void testBooleanArray() {
    printf("\ntestBooleanArray\n");
    PVScalarArray *pvScalarArray =
        getStandardPVField()->scalarArrayValue(0,pvBoolean);
    PVBooleanArray *pvValue = (PVBooleanArray *)pvScalarArray;
    int length = 5;
    bool *value = new bool[length];
    for(int i=0; i<length; i++) value[i] = epicsTrue;
    pvValue->put(0,length,value,0);
    BooleanArrayData data = BooleanArrayData();
    pvValue->get(0,length,&data);
    bool * getValue = data.data;
    printf("(orig,get):");
    for(int i=0; i< length; i++) {
        printf("(%s,%s) ",
            ((value[i]==epicsTrue) ? "true" : "false"),
            ((getValue[i]==epicsTrue) ? "true" : "false"));
    }
    printf("\n");
    FieldConstPtr field = pvValue->getField();
    buffer.clear();
    field->toString(&buffer);
    printf("%s\n",buffer.c_str());
    buffer.clear();
    buffer += "pv ";
    pvValue->toString(&buffer);
    printf("%s\n",buffer.c_str());
    delete pvValue;
    delete[] value;
}
void testByteArray() {
    printf("\ntestByteArray\n");
    PVScalarArray *pvScalarArray =
        getStandardPVField()->scalarArrayValue(0,pvByte);
    PVByteArray *pvValue = (PVByteArray *)pvScalarArray;
    int length = 5;
    epicsInt8 *value = new epicsInt8[length];
    for(int i=0; i<length; i++) value[i] = i;
    pvValue->put(0,length,value,0);
    ByteArrayData data = ByteArrayData();
    pvValue->get(0,length,&data);
    epicsInt8 * getValue = data.data;
    printf("(orig,get):");
    for(int i=0; i< length; i++) {
        printf("(%d,%d) ",(int)value[i],(int)getValue[i]);
    }
    printf("\n");
    FieldConstPtr field = pvValue->getField();
    buffer.clear();
    field->toString(&buffer);
    printf("%s\n",buffer.c_str());
    buffer.clear();
    buffer += "pv ";
    pvValue->toString(&buffer);
    printf("%s\n",buffer.c_str());
    delete pvValue;
    delete[] value;
}
void testShortArray() {
    printf("\ntestShortArray\n");
    PVScalarArray *pvScalarArray =
        getStandardPVField()->scalarArrayValue(0,pvShort);
    PVShortArray *pvValue = (PVShortArray *)pvScalarArray;
    int length = 5;
    epicsInt16 *value = new epicsInt16[length];
    for(int i=0; i<length; i++) value[i] = i;
    pvValue->put(0,length,value,0);
    ShortArrayData data = ShortArrayData();
    pvValue->get(0,length,&data);
    epicsInt16 * getValue = data.data;
    printf("(orig,get):");
    for(int i=0; i< length; i++) {
        printf("(%d,%d) ",(int)value[i],(int)getValue[i]);
    }
    printf("\n");
    FieldConstPtr field = pvValue->getField();
    buffer.clear();
    field->toString(&buffer);
    printf("%s\n",buffer.c_str());
    buffer.clear();
    buffer += "pv ";
    pvValue->toString(&buffer);
    printf("%s\n",buffer.c_str());
    delete pvValue;
    delete[] value;
}
void testIntArray() {
    printf("\ntestIntArray\n");
    PVScalarArray *pvScalarArray =
        getStandardPVField()->scalarArrayValue(0,pvInt);
    PVIntArray *pvValue = (PVIntArray *)pvScalarArray;
    int length = 5;
    epicsInt32 *value = new epicsInt32[length];
    for(int i=0; i<length; i++) value[i] = i;
    pvValue->put(0,length,value,0);
    IntArrayData data = IntArrayData();
    pvValue->get(0,length,&data);
    epicsInt32 * getValue = data.data;
    printf("(orig,get):");
    for(int i=0; i< length; i++) {
        printf("(%d,%d) ",value[i],getValue[i]);
    }
    printf("\n");
    FieldConstPtr field = pvValue->getField();
    buffer.clear();
    field->toString(&buffer);
    printf("%s\n",buffer.c_str());
    buffer.clear();
    buffer += "pv ";
    pvValue->toString(&buffer);
    printf("%s\n",buffer.c_str());
    delete pvValue;
    delete[] value;
}

void testLongArray() {
    printf("\ntestLongArray\n");
    PVScalarArray *pvScalarArray =
        getStandardPVField()->scalarArrayValue(0,pvLong);
    PVLongArray *pvValue = (PVLongArray *)pvScalarArray;
    int length = 5;
    epicsInt64 *value = new epicsInt64[length];
    for(int i=0; i<length; i++) value[i] = i;
    pvValue->put(0,length,value,0);
    LongArrayData data = LongArrayData();
    pvValue->get(0,length,&data);
    epicsInt64 * getValue = data.data;
    printf("(orig,get):");
    for(int i=0; i< length; i++) {
        printf("(%ld,%ld) ",(long int)value[i],(long int)getValue[i]);
    }
    printf("\n");
    FieldConstPtr field = pvValue->getField();
    buffer.clear();
    field->toString(&buffer);
    printf("%s\n",buffer.c_str());
    buffer.clear();
    buffer += "pv ";
    pvValue->toString(&buffer);
    printf("%s\n",buffer.c_str());
    delete pvValue;
    delete[] value;
}

void testFloatArray() {
    printf("\ntestFloatArray\n");
    PVScalarArray *pvScalarArray =
        getStandardPVField()->scalarArrayValue(0,pvFloat);
    PVFloatArray *pvValue = (PVFloatArray *)pvScalarArray;
    int length = 5;
    float *value = new float[length];
    for(int i=0; i<length; i++) value[i] = i;
    pvValue->put(0,length,value,0);
    FloatArrayData data = FloatArrayData();
    pvValue->get(0,length,&data);
    float * getValue = data.data;
    printf("(orig,get):");
    for(int i=0; i< length; i++) {
        printf("(%f,%f) ",value[i],getValue[i]);
    }
    printf("\n");
    FieldConstPtr field = pvValue->getField();
    buffer.clear();
    field->toString(&buffer);
    printf("%s\n",buffer.c_str());
    buffer.clear();
    buffer += "pv ";
    pvValue->toString(&buffer);
    printf("%s\n",buffer.c_str());
    delete pvValue;
    delete[] value;
}

void testDoubleArray() {
    printf("\ntestDoubleArray\n");
    PVScalarArray *pvScalarArray =
        getStandardPVField()->scalarArrayValue(0,pvDouble);
    PVDoubleArray *pvValue = (PVDoubleArray *)pvScalarArray;
    int length = 5;
    double *value = new double[length];
    for(int i=0; i<length; i++) value[i] = i;
    pvValue->put(0,length,value,0);
    DoubleArrayData data = DoubleArrayData();
    pvValue->get(0,length,&data);
    double * getValue = data.data;
    printf("(orig,get):");
    for(int i=0; i< length; i++) {
        printf("(%f,%f) ",value[i],getValue[i]);
    }
    printf("\n");
    FieldConstPtr field = pvValue->getField();
    buffer.clear();
    field->toString(&buffer);
    printf("%s\n",buffer.c_str());
    buffer.clear();
    buffer += "pv ";
    pvValue->toString(&buffer);
    printf("%s\n",buffer.c_str());
    delete pvValue;
    delete[] value;
}

void testStringArray() {
    printf("\ntestStringArray\n");
    PVScalarArray *pvScalarArray =
        getStandardPVField()->scalarArrayValue(0,pvString);
    PVStringArray *pvValue = (PVStringArray *)pvScalarArray;
    int length = 5;
    String *value = new String[length];
    for(int i=0; i<length; i++) {
        char buf[16];
        sprintf(buf,"string%d",i);
        value[i] = buf;
    }
    pvValue->put(0,length,value,0);
    StringArrayData data = StringArrayData();
    pvValue->get(0,length,&data);
    String * getValue = data.data;
    printf("(orig,get):");
    for(int i=0; i< length; i++) {
        printf("(%s,%s) ",value[i].c_str(),getValue[i].c_str());
    }
    printf("\n");
    FieldConstPtr field = pvValue->getField();
    buffer.clear();
    field->toString(&buffer);
    printf("%s\n",buffer.c_str());
    buffer.clear();
    buffer += "pv ";
    pvValue->toString(&buffer);
    printf("%s\n",buffer.c_str());
    delete pvValue;
    delete[] value;
}

int main(int argc,char *argv[])
{
    pfieldCreate = getFieldCreate();
    pvDataCreate = getPVDataCreate();
    testBooleanArray();
    testByteArray();
    testShortArray();
    testIntArray();
    testLongArray();
    testFloatArray();
    testDoubleArray();
    testStringArray();
    printf("main returning\n");
    return(0);
}

