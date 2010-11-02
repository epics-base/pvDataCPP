/* testPVscalar.cpp */
/* Author:  Marty Kraimer Date: 2010.09 */

#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <epicsAssert.h>

#include "requester.h"
#include "pvIntrospect.h"
#include "pvData.h"
#include "standardField.h"
#include "standardPVField.h"

using namespace epics::pvData;

static FieldCreate * pfieldCreate = 0;
static PVDataCreate *pvDataCreate = 0;
static String buffer("");

void testBoolean() {
    printf("\ntestBoolean\n");
    PVScalar *pvScalar = getStandardPVField()->scalarValue(0,pvBoolean);
    PVBoolean *pvValue = (PVBoolean *)pvScalar;
    bool value = true;
    pvValue->put(value);
    bool getValue = pvValue->get();
    printf("put %s get %s\n",
        ((value==false) ? "false" : "true"),
        ((getValue==false) ? "false" : "true"));
    if(value!=getValue) {
        fprintf(stderr,"ERROR getValue put %s get %s\n",
            ((value==false) ? "false" : "true"),
            ((getValue==false) ? "false" : "true"));
    }
    FieldConstPtr field = pvValue->getField();
    buffer.clear();
    field->toString(&buffer);
    printf("%s\n",buffer.c_str());
    bool isImmutable = pvValue->isImmutable();
    PVStructure *pvParent = pvValue->getParent();
    printf("immutable %s parent %p\n",
        ((isImmutable==false) ? "false" : "true"),
        pvParent);
    int offset = pvValue->getFieldOffset();
    int nextOffset = pvValue->getNextFieldOffset();
    int numberFields = pvValue->getNumberFields();
    printf("offset %d nextOffset %d numberFields %d\n",
        offset,nextOffset,numberFields);
    ScalarConstPtr scalar = dynamic_cast<ScalarConstPtr>(field);
    if(scalar!=field) {
        fprintf(stderr,"ERROR field!=scalar field %p scalar %p\n",field,scalar);
    }
    buffer.clear();
    buffer += "value ";
    pvValue->toString(&buffer);
    printf("%s\n",buffer.c_str());
    pvScalar->message(String("this is a test message"),infoMessage);
    PVScalar *other = getStandardPVField()->scalarValue(0,pvDouble);
    bool isEqual = pvScalar==other;
    printf("expect false isEqual %s\n",(isEqual ? "true" : "false"));
    delete pvValue;
}

void testByte() {
    printf("\ntestByte\n");
    PVScalar *pvScalar = getStandardPVField()->scalarValue(0,pvByte);
    PVByte *pvValue = (PVByte *)pvScalar;
    int8 value = 2;
    pvValue->put(value);
    int getValue = pvValue->get();
    printf("put %d get %d\n",value,getValue);
    if(value!=getValue) {
        fprintf(stderr,"ERROR getValue put %d get %d\n",value,getValue);
    }
    FieldConstPtr field = pvValue->getField();
    buffer.clear();
    field->toString(&buffer);
    printf("%s\n",buffer.c_str());
    bool isImmutable = pvValue->isImmutable();
    PVStructure *pvParent = pvValue->getParent();
    printf("immutable %s parent %p\n",
        ((isImmutable==false) ? "false" : "true"),
        pvParent);
    int offset = pvValue->getFieldOffset();
    int nextOffset = pvValue->getNextFieldOffset();
    int numberFields = pvValue->getNumberFields();
    printf("offset %d nextOffset %d numberFields %d\n",
        offset,nextOffset,numberFields);
    ScalarConstPtr scalar = dynamic_cast<ScalarConstPtr>(field);
    if(scalar!=field) {
        fprintf(stderr,"ERROR field!=scalar field %p scalar %p\n",field,scalar);
    }
    buffer.clear();
    buffer += "value ";
    pvValue->toString(&buffer);
    printf("%s\n",buffer.c_str());
    pvScalar->message(String("this is a test message"),infoMessage);
    PVScalar *other = getStandardPVField()->scalarValue(0,pvDouble);
    bool isEqual = pvScalar==other;
    printf("expect false isEqual %s\n",(isEqual ? "true" : "false"));
    delete pvValue;
}

void testShort() {
    printf("\ntestShort\n");
    PVScalar *pvScalar = getStandardPVField()->scalarValue(0,pvShort);
    PVShort *pvValue = (PVShort *)pvScalar;
    int16 value = 2;
    pvValue->put(value);
    int getValue = pvValue->get();
    printf("put %d get %d\n",value,getValue);
    if(value!=getValue) {
        fprintf(stderr,"ERROR getValue put %d get %d\n",value,getValue);
    }
    FieldConstPtr field = pvValue->getField();
    buffer.clear();
    field->toString(&buffer);
    printf("%s\n",buffer.c_str());
    bool isImmutable = pvValue->isImmutable();
    PVStructure *pvParent = pvValue->getParent();
    printf("immutable %s parent %p\n",
        ((isImmutable==false) ? "false" : "true"),
        pvParent);
    int offset = pvValue->getFieldOffset();
    int nextOffset = pvValue->getNextFieldOffset();
    int numberFields = pvValue->getNumberFields();
    printf("offset %d nextOffset %d numberFields %d\n",
        offset,nextOffset,numberFields);
    ScalarConstPtr scalar = dynamic_cast<ScalarConstPtr>(field);
    if(scalar!=field) {
        fprintf(stderr,"ERROR field!=scalar field %p scalar %p\n",field,scalar);
    }
    buffer.clear();
    buffer += "value ";
    pvValue->toString(&buffer);
    printf("%s\n",buffer.c_str());
    pvScalar->message(String("this is a test message"),infoMessage);
    PVScalar *other = getStandardPVField()->scalarValue(0,pvDouble);
    bool isEqual = pvScalar==other;
    printf("expect false isEqual %s\n",(isEqual ? "true" : "false"));
    delete pvValue;
}

void testInt() {
    printf("\ntestInt\n");
    PVScalar *pvScalar = getStandardPVField()->scalarValue(0,pvInt);
    PVInt *pvValue = (PVInt *)pvScalar;
    int32 value = 2;
    pvValue->put(value);
    int getValue = pvValue->get();
    printf("put %d get %d\n",value,getValue);
    if(value!=getValue) {
        fprintf(stderr,"ERROR getValue put %d get %d\n",value,getValue);
    }
    FieldConstPtr field = pvValue->getField();
    buffer.clear();
    field->toString(&buffer);
    printf("%s\n",buffer.c_str());
    bool isImmutable = pvValue->isImmutable();
    PVStructure *pvParent = pvValue->getParent();
    printf("immutable %s parent %p\n",
        ((isImmutable==false) ? "false" : "true"),
        pvParent);
    int offset = pvValue->getFieldOffset();
    int nextOffset = pvValue->getNextFieldOffset();
    int numberFields = pvValue->getNumberFields();
    printf("offset %d nextOffset %d numberFields %d\n",
        offset,nextOffset,numberFields);
    ScalarConstPtr scalar = dynamic_cast<ScalarConstPtr>(field);
    if(scalar!=field) {
        fprintf(stderr,"ERROR field!=scalar field %p scalar %p\n",field,scalar);
    }
    buffer.clear();
    buffer += "value ";
    pvValue->toString(&buffer);
    printf("%s\n",buffer.c_str());
    pvScalar->message(String("this is a test message"),infoMessage);
    PVScalar *other = getStandardPVField()->scalarValue(0,pvDouble);
    bool isEqual = pvScalar==other;
    printf("expect false isEqual %s\n",(isEqual ? "true" : "false"));
    delete pvValue;
}

void testLong() {
    printf("\ntestLong\n");
    PVScalar *pvScalar = getStandardPVField()->scalarValue(0,pvLong);
    PVLong *pvValue = (PVLong *)pvScalar;
    int64 value = 2;
    pvValue->put(value);
    int64 getValue = pvValue->get();
    printf("put %ld get %ld\n",(long int)value,(long int)getValue);
    if(value!=getValue) {
        fprintf(stderr,"ERROR getValue put %ld get %ld\n",(long int)value,(long int)getValue);
    }
    FieldConstPtr field = pvValue->getField();
    buffer.clear();
    field->toString(&buffer);
    printf("%s\n",buffer.c_str());
    bool isImmutable = pvValue->isImmutable();
    PVStructure *pvParent = pvValue->getParent();
    printf("immutable %s parent %p\n",
        ((isImmutable==false) ? "false" : "true"),
        pvParent);
    int offset = pvValue->getFieldOffset();
    int nextOffset = pvValue->getNextFieldOffset();
    int numberFields = pvValue->getNumberFields();
    printf("offset %d nextOffset %d numberFields %d\n",
        offset,nextOffset,numberFields);
    ScalarConstPtr scalar = dynamic_cast<ScalarConstPtr>(field);
    if(scalar!=field) {
        fprintf(stderr,"ERROR field!=scalar field %p scalar %p\n",field,scalar);
    }
    buffer.clear();
    buffer += "value ";
    pvValue->toString(&buffer);
    printf("%s\n",buffer.c_str());
    pvScalar->message(String("this is a test message"),infoMessage);
    PVScalar *other = getStandardPVField()->scalarValue(0,pvDouble);
    bool isEqual = pvScalar==other;
    printf("expect false isEqual %s\n",(isEqual ? "true" : "false"));
    delete pvValue;
}

void testFloat() {
    printf("\ntestFloat\n");
    PVScalar *pvScalar = getStandardPVField()->scalarValue(0,pvFloat);
    PVFloat *pvValue = (PVFloat *)pvScalar;
    float value = 2;
    pvValue->put(value);
    float getValue = pvValue->get();
    printf("put %f get %f\n",value,getValue);
    if(value!=getValue) {
        fprintf(stderr,"ERROR getValue put %f get %f\n",value,getValue);
    }
    FieldConstPtr field = pvValue->getField();
    buffer.clear();
    field->toString(&buffer);
    printf("%s\n",buffer.c_str());
    bool isImmutable = pvValue->isImmutable();
    PVStructure *pvParent = pvValue->getParent();
    printf("immutable %s parent %p\n",
        ((isImmutable==false) ? "false" : "true"),
        pvParent);
    int offset = pvValue->getFieldOffset();
    int nextOffset = pvValue->getNextFieldOffset();
    int numberFields = pvValue->getNumberFields();
    printf("offset %d nextOffset %d numberFields %d\n",
        offset,nextOffset,numberFields);
    ScalarConstPtr scalar = dynamic_cast<ScalarConstPtr>(field);
    if(scalar!=field) {
        fprintf(stderr,"ERROR field!=scalar field %p scalar %p\n",field,scalar);
    }
    buffer.clear();
    buffer += "value ";
    pvValue->toString(&buffer);
    printf("%s\n",buffer.c_str());
    pvScalar->message(String("this is a test message"),infoMessage);
    bool isEqual = pvScalar==pvScalar;
    printf("expect true isEqual %s\n",(isEqual ? "true" : "false"));
    delete pvValue;
}

void testDouble() {
    printf("\ntestDouble\n");
    PVScalar *pvScalar = getStandardPVField()->scalarValue(0,pvDouble);
    PVDouble *pvValue = (PVDouble *)pvScalar;
    double value = 2;
    pvValue->put(value);
    double getValue = pvValue->get();
    printf("put %lf get %lf\n",value,getValue);
    if(value!=getValue) {
        fprintf(stderr,"ERROR getValue put %lf get %lf\n",value,getValue);
    }
    FieldConstPtr field = pvValue->getField();
    buffer.clear();
    field->toString(&buffer);
    printf("%s\n",buffer.c_str());
    bool isImmutable = pvValue->isImmutable();
    PVStructure *pvParent = pvValue->getParent();
    printf("immutable %s parent %p\n",
        ((isImmutable==false) ? "false" : "true"),
        pvParent);
    int offset = pvValue->getFieldOffset();
    int nextOffset = pvValue->getNextFieldOffset();
    int numberFields = pvValue->getNumberFields();
    printf("offset %d nextOffset %d numberFields %d\n",
        offset,nextOffset,numberFields);
    ScalarConstPtr scalar = dynamic_cast<ScalarConstPtr>(field);
    if(scalar!=field) {
        fprintf(stderr,"ERROR field!=scalar field %p scalar %p\n",field,scalar);
    }
    buffer.clear();
    buffer += "value ";
    pvValue->toString(&buffer);
    printf("%s\n",buffer.c_str());
    pvScalar->message(String("this is a test message"),infoMessage);
    bool isEqual = pvScalar==pvScalar;
    printf("expect true isEqual %s\n",(isEqual ? "true" : "false"));
    delete pvValue;
}

void testString() {
    printf("\ntestString\n");
    PVScalar *pvScalar = getStandardPVField()->scalarValue(0,pvString);
    PVString *pvValue = (PVString *)pvScalar;
    String value = "testString";
    pvValue->put(value);
    String getValue = pvValue->get();
    printf("put %s get %s\n",value.c_str(),getValue.c_str());
    if(value!=getValue) {
        fprintf(stderr,"ERROR getValue put %s get %s\n",
            value.c_str(),getValue.c_str());
    }
    FieldConstPtr field = pvValue->getField();
    buffer.clear();
    field->toString(&buffer);
    printf("%s\n",buffer.c_str());
    bool isImmutable = pvValue->isImmutable();
    PVStructure *pvParent = pvValue->getParent();
    printf("immutable %s parent %p\n",
        ((isImmutable==false) ? "false" : "true"),
        pvParent);
    int offset = pvValue->getFieldOffset();
    int nextOffset = pvValue->getNextFieldOffset();
    int numberFields = pvValue->getNumberFields();
    printf("offset %d nextOffset %d numberFields %d\n",
        offset,nextOffset,numberFields);
    ScalarConstPtr scalar = dynamic_cast<ScalarConstPtr>(field);
    if(scalar!=field) {
        fprintf(stderr,"ERROR field!=scalar field %p scalar %p\n",field,scalar);
    }
    buffer.clear();
    buffer += "value ";
    pvValue->toString(&buffer);
    printf("%s\n",buffer.c_str());
    pvScalar->message(String("this is a test message"),infoMessage);
    bool isEqual = pvScalar==pvScalar;
    printf("expect true isEqual %s\n",(isEqual ? "true" : "false"));
    delete pvValue;
}

int main(int argc,char *argv[])
{
    pfieldCreate = getFieldCreate();
    pvDataCreate = getPVDataCreate();
    testBoolean();
    testByte();
    testShort();
    testInt();
    testLong();
    testFloat();
    testDouble();
    testString();
    return(0);
}

