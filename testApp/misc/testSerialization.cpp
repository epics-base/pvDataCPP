/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/*
 * testSerialization.cpp
 *
 *  Created on: Oct 25, 2010
 *      Author: Miha Vitorovic
 */
#include <iostream>

#include <epicsAssert.h>

#include <epicsExit.h>
#include "pvIntrospect.h"
#include "pvData.h"
#include "serialize.h"
#include "noDefaultMethods.h"
#include "byteBuffer.h"
#include "CDRMonitor.h"

#define BYTE_MAX_VALUE 127
#define BYTE_MIN_VALUE -128
#define SHORT_MAX_VALUE 32767
#define SHORT_MIN_VALUE -32768
#define INT_MAX_VALUE 2147483647
#define INT_MIN_VALUE (-INT_MAX_VALUE - 1)
#define LONG_MAX_VALUE 9223372036854775807LL
#define LONG_MIN_VALUE (-LONG_MAX_VALUE - 1LL)
#define FLOAT_MAX_VALUE 3.4028235E38
#define FLOAT_MIN_VALUE 1.4E-45
#define DOUBLE_MAX_VALUE 1.7976931348623157E308
#define DOUBLE_MIN_VALUE 4.9E-324

using namespace epics::pvData;
using std::cout;

namespace epics {
    namespace pvData {

        class SerializableControlImpl : public SerializableControl,
                public NoDefaultMethods {
        public:
            virtual void flushSerializeBuffer() {
            }

            virtual void ensureBuffer(int size) {
            }

            SerializableControlImpl() {
            }

            virtual ~SerializableControlImpl() {
            }
        };

        class DeserializableControlImpl : public DeserializableControl,
                public NoDefaultMethods {
        public:
            virtual void ensureData(int size) {
            }

            DeserializableControlImpl() {
            }

            virtual ~DeserializableControlImpl() {
            }
        };

    }
}

static SerializableControl* flusher;
static DeserializableControl* control;
static ByteBuffer* buffer;

void serializationTest(PVField* field) {
    buffer->clear();

    // serialize
    field->serialize(buffer, flusher);

    buffer->flip();

    // create new instance and deserialize
    field->getField()->incReferenceCount();
    PVField* deserializedField = getPVDataCreate()->createPVField(NULL,
            field->getField());
    deserializedField->deserialize(buffer, control);

    // must equal
    assert((*field)==(*deserializedField));

    delete deserializedField; // clean up
}

void testScalar() {
    cout<<"Testing scalars...\n";
    PVDataCreate* factory = getPVDataCreate();
    assert(factory!=NULL);

    cout<<"\tPVBoolean\n";
    PVBoolean* pvBoolean = (PVBoolean*)factory->createPVScalar(NULL,
            "pvBoolean", epics::pvData::pvBoolean);
    pvBoolean->put(false);
    serializationTest(pvBoolean);
    pvBoolean->put(true);
    serializationTest(pvBoolean);
    delete pvBoolean;

    cout<<"\tPVByte\n";
    PVByte* pvByte = (PVByte*)factory->createPVScalar(NULL, "pvByte",
            epics::pvData::pvByte);
    pvByte->put(0);
    serializationTest(pvByte);
    pvByte->put(12);
    serializationTest(pvByte);
    pvByte->put(BYTE_MAX_VALUE);
    serializationTest(pvByte);
    pvByte->put(BYTE_MIN_VALUE);
    serializationTest(pvByte);
    delete pvByte;

    cout<<"\tPVShort\n";
    PVShort* pvShort = (PVShort*)factory->createPVScalar(NULL, "pvShort",
            epics::pvData::pvShort);
    pvShort->put(0);
    serializationTest(pvShort);
    pvShort->put(123);
    serializationTest(pvShort);
    pvShort->put(BYTE_MAX_VALUE);
    serializationTest(pvShort);
    pvShort->put(BYTE_MIN_VALUE);
    serializationTest(pvShort);
    pvShort->put(SHORT_MAX_VALUE);
    serializationTest(pvShort);
    pvShort->put(SHORT_MIN_VALUE);
    serializationTest(pvShort);
    delete pvShort;

    cout<<"\tPVInt\n";
    PVInt* pvInt = (PVInt*)factory->createPVScalar(NULL, "pvInt",
            epics::pvData::pvInt);
    pvInt->put(0);
    serializationTest(pvInt);
    pvInt->put(123456);
    serializationTest(pvInt);
    pvInt->put(BYTE_MAX_VALUE);
    serializationTest(pvInt);
    pvInt->put(BYTE_MIN_VALUE);
    serializationTest(pvInt);
    pvInt->put(SHORT_MAX_VALUE);
    serializationTest(pvInt);
    pvInt->put(SHORT_MIN_VALUE);
    serializationTest(pvInt);
    pvInt->put(INT_MAX_VALUE);
    serializationTest(pvInt);
    pvInt->put(INT_MIN_VALUE);
    serializationTest(pvInt);
    delete pvInt;

    cout<<"\tPVLong\n";
    PVLong* pvLong = (PVLong*)factory->createPVScalar(NULL, "pvLong",
            epics::pvData::pvLong);
    pvLong->put(0);
    serializationTest(pvLong);
    pvLong->put(12345678901LL);
    serializationTest(pvLong);
    pvLong->put(BYTE_MAX_VALUE);
    serializationTest(pvLong);
    pvLong->put(BYTE_MIN_VALUE);
    serializationTest(pvLong);
    pvLong->put(SHORT_MAX_VALUE);
    serializationTest(pvLong);
    pvLong->put(SHORT_MIN_VALUE);
    serializationTest(pvLong);
    pvLong->put(INT_MAX_VALUE);
    serializationTest(pvLong);
    pvLong->put(INT_MIN_VALUE);
    serializationTest(pvLong);
    pvLong->put(LONG_MAX_VALUE);
    serializationTest(pvLong);
    pvLong->put(LONG_MIN_VALUE);
    serializationTest(pvLong);
    delete pvLong;

    cout<<"\tPVFloat\n";
    PVFloat* pvFloat = (PVFloat*)factory->createPVScalar(NULL, "pvFloat",
            epics::pvData::pvFloat);
    pvFloat->put(0);
    serializationTest(pvFloat);
    pvFloat->put(12.345);
    serializationTest(pvFloat);
    pvFloat->put(FLOAT_MAX_VALUE);
    serializationTest(pvFloat);
    pvFloat->put(FLOAT_MIN_VALUE);
    serializationTest(pvFloat);
    delete pvFloat;

    cout<<"\tPVDouble\n";
    PVDouble* pvDouble = (PVDouble*)factory->createPVScalar(NULL, "pvDouble",
            epics::pvData::pvDouble);
    pvDouble->put(0);
    serializationTest(pvDouble);
    pvDouble->put(12.345);
    serializationTest(pvDouble);
    pvDouble->put(DOUBLE_MAX_VALUE);
    serializationTest(pvDouble);
    pvDouble->put(DOUBLE_MIN_VALUE);
    serializationTest(pvDouble);
    delete pvDouble;

    cout<<"\tPVString\n";
    PVString* pvString = (PVString*)factory->createPVScalar(NULL, "pvString",
            epics::pvData::pvString);
    pvString->put("");
    serializationTest(pvString);
    pvString->put("s");
    serializationTest(pvString);
    pvString->put("string");
    serializationTest(pvString);
    pvString->put("string with spaces");
    serializationTest(pvString);
    pvString->put("string with spaces and special characters\f\n");
    serializationTest(pvString);

    // huge string test
    pvString->put(String(10000, 'a'));
    serializationTest(pvString);

    delete pvString;

    cout<<"!!!  PASSED\n\n";
}

void testArray() {
    cout<<"Testing arrays...\n";

    PVDataCreate* factory = getPVDataCreate();
    assert(factory!=NULL);

    cout<<"\tPVBooleanArray\n";
    bool boolEmpty[] = { };
    bool bv[] = { false, true, false, true, true };
    PVBooleanArray* pvBoolean = (PVBooleanArray*)factory->createPVScalarArray(
            NULL, "pvBooleanArray", epics::pvData::pvBoolean);
    pvBoolean->put(0, 0, boolEmpty, 0);
    serializationTest(pvBoolean);
    pvBoolean->put(0, 5, bv, 0);
    serializationTest(pvBoolean);
    delete pvBoolean;

    cout<<"\tPVByteArray\n";
    int8 byteEmpty[] = { };
    int8 byv[] = { 0, 1, 2, -1, BYTE_MAX_VALUE, BYTE_MAX_VALUE-1,
            BYTE_MIN_VALUE+1, BYTE_MIN_VALUE };
    PVByteArray* pvByte = (PVByteArray*)factory->createPVScalarArray(NULL,
            "pvByteArray", epics::pvData::pvByte);
    pvByte->put(0, 0, byteEmpty, 0);
    serializationTest(pvByte);
    pvByte->put(0, 8, byv, 0);
    serializationTest(pvByte);
    delete pvByte;

    cout<<"\tPVShortArray\n";
    int16 shortEmpty[] = { };
    int16 sv[] = { 0, 1, 2, -1, SHORT_MAX_VALUE, SHORT_MAX_VALUE-1,
            SHORT_MIN_VALUE+1, SHORT_MIN_VALUE };
    PVShortArray* pvShort = (PVShortArray*)factory->createPVScalarArray(NULL,
            "pvShortArray", epics::pvData::pvShort);
    pvShort->put(0, 0, shortEmpty, 0);
    serializationTest(pvShort);
    pvShort->put(0, 8, sv, 0);
    serializationTest(pvShort);
    delete pvShort;

    cout<<"\tPVIntArray\n";
    int32 intEmpty[] = { };
    int32 iv[] = { 0, 1, 2, -1, INT_MAX_VALUE, INT_MAX_VALUE-1,
            INT_MIN_VALUE+1, INT_MIN_VALUE };
    PVIntArray* pvInt = (PVIntArray*)factory->createPVScalarArray(NULL,
            "pvIntArray", epics::pvData::pvInt);
    pvInt->put(0, 0, intEmpty, 0);
    serializationTest(pvInt);
    pvInt->put(0, 8, iv, 0);
    serializationTest(pvInt);
    delete pvInt;

    cout<<"\tPVLongArray\n";
    int64 longEmpty[] = { };
    int64 lv[] = { 0, 1, 2, -1, LONG_MAX_VALUE, LONG_MAX_VALUE-1,
            LONG_MIN_VALUE+1, LONG_MIN_VALUE };
    PVLongArray* pvLong = (PVLongArray*)factory->createPVScalarArray(NULL,
            "pvLongArray", epics::pvData::pvLong);
    pvLong->put(0, 0, longEmpty, 0);
    serializationTest(pvLong);
    pvLong->put(0, 8, lv, 0);
    serializationTest(pvLong);
    delete pvLong;

    cout<<"\tPVFloatArray\n";
    float floatEmpty[] = { };
    float fv[] = { (float)0.0, (float)1.1, (float)2.3, (float)-1.4,
            FLOAT_MAX_VALUE, FLOAT_MAX_VALUE-(float)123456.789, FLOAT_MIN_VALUE
                    +(float)1.1, FLOAT_MIN_VALUE };
    PVFloatArray* pvFloat = (PVFloatArray*)factory->createPVScalarArray(NULL,
            "pvFloatArray", epics::pvData::pvFloat);
    pvFloat->put(0, 0, floatEmpty, 0);
    serializationTest(pvFloat);
    pvFloat->put(0, 8, fv, 0);
    serializationTest(pvFloat);
    delete pvFloat;

    cout<<"\tPVDoubleArray\n";
    double doubleEmpty[] = { };
    double dv[] = { (double)0.0, (double)1.1, (double)2.3, (double)-1.4,
            DOUBLE_MAX_VALUE, DOUBLE_MAX_VALUE-(double)123456.789,
            DOUBLE_MIN_VALUE+(double)1.1, DOUBLE_MIN_VALUE };
    PVDoubleArray* pvDouble = (PVDoubleArray*)factory->createPVScalarArray(
            NULL, "pvDoubleArray", epics::pvData::pvDouble);
    pvDouble->put(0, 0, doubleEmpty, 0);
    serializationTest(pvDouble);
    pvDouble->put(0, 8, dv, 0);
    serializationTest(pvDouble);
    delete pvDouble;

    cout<<"\tPVStringArray\n";
    String stringEmpty[] = { };
    String
            strv[] =
                    {
                            "",
                            "a",
                            "a b",
                            " ",
                            "test",
                            "smile",
                            "this is a little longer string... maybe a little but longer... this makes test better" };
    PVStringArray* pvString = (PVStringArray*)factory->createPVScalarArray(
            NULL, "pvStringArray", epics::pvData::pvString);
    pvString->put(0, 0, stringEmpty, 0);
    serializationTest(pvString);
    pvString->put(0, 7, strv, 0);
    serializationTest(pvString);
    delete pvString;

    cout<<"!!!  PASSED\n\n";
}

void testScalarEquals() {
    cout<<"Testing scalar equals...\n";
    PVDataCreate* factory = getPVDataCreate();
    assert(factory!=NULL);

    PVScalar *scalar1, *scalar2;

    scalar1 = factory->createPVScalar(NULL, "pvBoolean",
            epics::pvData::pvBoolean);
    scalar2 = factory->createPVScalar(NULL, "pvBoolean",
            epics::pvData::pvBoolean);
    assert((*scalar1)==(*scalar2));
    delete scalar1;
    delete scalar2;

    scalar1 = factory->createPVScalar(NULL, "pvByte", epics::pvData::pvByte);
    scalar2 = factory->createPVScalar(NULL, "pvByte", epics::pvData::pvByte);
    assert((*scalar1)==(*scalar2));
    delete scalar1;
    delete scalar2;

    scalar1 = factory->createPVScalar(NULL, "pvShort", epics::pvData::pvShort);
    scalar2 = factory->createPVScalar(NULL, "pvShort", epics::pvData::pvShort);
    assert((*scalar1)==(*scalar2));
    delete scalar1;
    delete scalar2;

    scalar1 = factory->createPVScalar(NULL, "pvInt", epics::pvData::pvInt);
    scalar2 = factory->createPVScalar(NULL, "pvInt", epics::pvData::pvInt);
    assert((*scalar1)==(*scalar2));
    delete scalar1;
    delete scalar2;

    scalar1 = factory->createPVScalar(NULL, "pvLong", epics::pvData::pvLong);
    scalar2 = factory->createPVScalar(NULL, "pvLong", epics::pvData::pvLong);
    assert((*scalar1)==(*scalar2));
    delete scalar1;
    delete scalar2;

    scalar1 = factory->createPVScalar(NULL, "pvFloat", epics::pvData::pvFloat);
    scalar2 = factory->createPVScalar(NULL, "pvFloat", epics::pvData::pvFloat);
    assert((*scalar1)==(*scalar2));
    delete scalar1;
    delete scalar2;

    scalar1
            = factory->createPVScalar(NULL, "pvDouble", epics::pvData::pvDouble);
    scalar2
            = factory->createPVScalar(NULL, "pvDouble", epics::pvData::pvDouble);
    assert((*scalar1)==(*scalar2));
    delete scalar1;
    delete scalar2;

    scalar1
            = factory->createPVScalar(NULL, "pvString", epics::pvData::pvString);
    scalar2
            = factory->createPVScalar(NULL, "pvString", epics::pvData::pvString);
    assert((*scalar1)==(*scalar2));
    delete scalar1;
    delete scalar2;

    FieldCreate* fieldCreate = getFieldCreate();
    FieldConstPtrArray fields = new FieldConstPtr[2];
    fields[0] = fieldCreate->createScalar("secondsSinceEpoch",
            epics::pvData::pvLong);
    fields[1] = fieldCreate->createScalar("nanoSeconds", epics::pvData::pvInt);
    StructureConstPtr structure = fieldCreate->createStructure("timeStamp", 2,
            fields);

    PVStructure* pvStruct1 = factory->createPVStructure(NULL, structure);
    structure->incReferenceCount();
    PVStructure* pvStruct2 = factory->createPVStructure(NULL, structure);
    assert((*pvStruct1)==(*pvStruct2));
    delete pvStruct2;
    delete pvStruct1;
    // 'structure' and 'fields' are deleted implicitly

    cout<<"!!!  PASSED\n\n";
}

void testScalarNonInitialized() {
    cout<<"Testing scalar non-initialized...\n";
    PVDataCreate* factory = getPVDataCreate();
    assert(factory!=NULL);

    PVScalar* scalar;

    scalar = factory->createPVScalar(NULL, "pvBoolean",
            epics::pvData::pvBoolean);
    serializationTest(scalar);
    delete scalar;

    scalar = factory->createPVScalar(NULL, "pvByte", epics::pvData::pvByte);
    serializationTest(scalar);
    delete scalar;

    scalar = factory->createPVScalar(NULL, "pvShort", epics::pvData::pvShort);
    serializationTest(scalar);
    delete scalar;

    scalar = factory->createPVScalar(NULL, "pvInt", epics::pvData::pvInt);
    serializationTest(scalar);
    delete scalar;

    scalar = factory->createPVScalar(NULL, "pvLong", epics::pvData::pvLong);
    serializationTest(scalar);
    delete scalar;

    scalar = factory->createPVScalar(NULL, "pvFloat", epics::pvData::pvFloat);
    serializationTest(scalar);
    delete scalar;

    scalar = factory->createPVScalar(NULL, "pvDouble", epics::pvData::pvDouble);
    serializationTest(scalar);
    delete scalar;

    scalar = factory->createPVScalar(NULL, "pvString", epics::pvData::pvString);
    serializationTest(scalar);
    delete scalar;

    FieldCreate* fieldCreate = getFieldCreate();
    FieldConstPtrArray fields = new FieldConstPtr[2];
    fields[0] = fieldCreate->createScalar("secondsSinceEpoch",
            epics::pvData::pvLong);
    fields[1] = fieldCreate->createScalar("nanoSeconds", epics::pvData::pvInt);
    StructureConstPtr structure = fieldCreate->createStructure("timeStamp", 2,
            fields);

    PVStructure* pvStruct = factory->createPVStructure(NULL, structure);
    serializationTest(pvStruct);
    delete pvStruct; // 'structure' and 'fields' are deleted implicitly

    cout<<"!!!  PASSED\n\n";
}

void testArrayNonInitialized() {
    cout<<"Testing array non-initialized...\n";
    PVDataCreate* factory = getPVDataCreate();
    assert(factory!=NULL);

    PVArray* array;

    array = factory->createPVScalarArray(NULL, "pvBooleanArray",
            epics::pvData::pvBoolean);
    serializationTest(array);
    delete array;

    array = factory->createPVScalarArray(NULL, "pvByteArray",
            epics::pvData::pvByte);
    serializationTest(array);
    delete array;

    array = factory->createPVScalarArray(NULL, "pvShortArray",
            epics::pvData::pvShort);
    serializationTest(array);
    delete array;

    array = factory->createPVScalarArray(NULL, "pvIntArray",
            epics::pvData::pvInt);
    serializationTest(array);
    delete array;

    array = factory->createPVScalarArray(NULL, "pvLongArray",
            epics::pvData::pvLong);
    serializationTest(array);
    delete array;

    array = factory->createPVScalarArray(NULL, "pvFloatArray",
            epics::pvData::pvFloat);
    serializationTest(array);
    delete array;

    array = factory->createPVScalarArray(NULL, "pvDoubleArray",
            epics::pvData::pvDouble);
    serializationTest(array);
    delete array;

    array = factory->createPVScalarArray(NULL, "pvStringArray",
            epics::pvData::pvString);
    serializationTest(array);
    delete array;

    FieldCreate* fieldCreate = getFieldCreate();
    FieldConstPtrArray fields = new FieldConstPtr[2];
    fields[0] = fieldCreate->createScalar("secondsSinceEpoch",
            epics::pvData::pvLong);
    fields[1] = fieldCreate->createScalar("nanoSeconds", epics::pvData::pvInt);
    StructureConstPtr structure = fieldCreate->createStructure("timeStamp", 2,
            fields);

    StructureArrayConstPtr structureArray = fieldCreate->createStructureArray(
            "timeStampArray", structure);
    PVStructureArray* pvStructArray = factory->createPVStructureArray(NULL,
            structureArray);
    serializationTest(pvStructArray);
    delete pvStructArray; // also deletes 'structureArray',
    //'structureArray' also deletes 'structure'
    //'structure' also deletes 'fields'

    cout<<"!!!  PASSED\n\n";
}

void testStructure() {
    cout<<"Testing structure...\n";

    FieldCreate* fieldCreate = getFieldCreate();
    PVDataCreate* pvDataCreate = getPVDataCreate();
    assert(fieldCreate!=NULL);
    assert(pvDataCreate!=NULL);

    cout<<"\tSimple structure serialization\n";
    FieldConstPtrArray fields = new FieldConstPtr[2];
    fields[0] = fieldCreate->createScalar("secondsSinceEpoch",
            epics::pvData::pvLong);
    fields[1] = fieldCreate->createScalar("nanoSeconds", epics::pvData::pvInt);
    PVStructure* pvStructure = pvDataCreate->createPVStructure(NULL,
            "timestamp", 2, fields);
    pvStructure->getLongField(fields[0]->getFieldName())->put(123);
    pvStructure->getIntField(fields[1]->getFieldName())->put(456);

    serializationTest(pvStructure);
    //serializationTest(pvStructure->getStructure());
    delete pvStructure;

    cout<<"\tComplex structure serialization\n";
    // and more complex :)
    FieldConstPtrArray fields2 = new FieldConstPtr[4];
    fields2[0] = fieldCreate->createScalar("longVal", epics::pvData::pvLong);
    fields2[1] = fieldCreate->createScalar("intVal", epics::pvData::pvInt);
    fields2[2] = fieldCreate->createScalarArray("values", epics::pvData::pvDouble);
    FieldConstPtrArray fields3 = new FieldConstPtr[2];
    fields3[0] = fieldCreate->createScalar("secondsSinceEpoch", epics::pvData::pvLong);
    fields3[1] = fieldCreate->createScalar("nanoSeconds", epics::pvData::pvInt);
    fields2[3] = fieldCreate->createStructure("timeStamp", 2, fields3);
    PVStructure* pvStructure2 = pvDataCreate->createPVStructure(NULL,
            "complexStructure", 4, fields2);
    pvStructure2->getLongField(fields2[0]->getFieldName())->put(1234);
    pvStructure2->getIntField(fields2[1]->getFieldName())->put(4567);
    PVDoubleArray* da = (PVDoubleArray*)pvStructure2->getScalarArrayField(
            fields2[2]->getFieldName(), epics::pvData::pvDouble);
    double dd[] = { 1.2, 3.4, 4.5 };
    da->put(0, 3, dd, 0);

    PVStructure* ps = pvStructure2->getStructureField(
            fields2[3]->getFieldName());
    ps->getLongField(fields3[0]->getFieldName())->put(789);
    ps->getIntField(fields3[1]->getFieldName())->put(1011);

    serializationTest(pvStructure2);
    //serializationTest(pvStructure2->getStructure());
    delete pvStructure2;


    cout<<"!!!  PASSED\n\n";
}

/*
 void testIntrospectionSerialization() {
 cout<<"Testing introspection serialization...\n";
 FieldCreate* factory = getFieldCreate();
 assert(factory!=NULL);

 ScalarConstPtr scalar = factory->createScalar("scalar", epics::pvData::pvDouble);
 serializatioTest(scalar);
 delete scalar;

 ScalarArrayConstPtr array = factory->createScalarArray("array", epics::pvData::pvDouble);
 serializatioTest(array);
 delete array;

 cout<<"!!!  PASSED\n\n";
 }
 */

int main(int argc, char *argv[]) {
    flusher = new SerializableControlImpl();
    control = new DeserializableControlImpl();
    buffer = new ByteBuffer(1<<16);

    testScalarEquals();
    testScalar();
    testArray();
    testScalarNonInitialized();
    testArrayNonInitialized();
    testStructure();
    //testIntrospectionSerialization();

    delete buffer;
    delete control;
    delete flusher;

    epicsExitCallAtExits();
    CDRMonitor::get().show(stdout);
    cout<<"\nDone!\n";

    return (0);
}

