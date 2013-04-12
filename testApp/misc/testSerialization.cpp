/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/*
 * testSerialization.cpp
 *
 *  Created on: Oct 25, 2010
 *      Author: Miha Vitorovic
 */
#include <iostream>
#include <fstream>

#include <epicsAssert.h>

#include <epicsExit.h>
#include <pv/pvIntrospect.h>
#include <pv/pvData.h>
#include <pv/serialize.h>
#include <pv/noDefaultMethods.h>
#include <pv/byteBuffer.h>
#include <pv/convert.h>

#include <pv/standardField.h>

#include <limits>

#define BYTE_MAX_VALUE std::numeric_limits<int8>::max()
#define BYTE_MIN_VALUE std::numeric_limits<int8>::min()
#define UBYTE_MAX_VALUE std::numeric_limits<uint8>::max()
#define SHORT_MAX_VALUE std::numeric_limits<int16>::max()
#define SHORT_MIN_VALUE std::numeric_limits<int16>::min()
#define USHORT_MAX_VALUE std::numeric_limits<uint16>::max()
#define INT_MAX_VALUE std::numeric_limits<int32>::max()
#define INT_MIN_VALUE std::numeric_limits<int32>::min()
#define UINT_MAX_VALUE std::numeric_limits<uint32>::max()
#define LONG_MAX_VALUE std::numeric_limits<int64>::max()
#define LONG_MIN_VALUE std::numeric_limits<int64>::min()
#define ULONG_MAX_VALUE std::numeric_limits<uint64>::max()
#define FLOAT_MAX_VALUE std::numeric_limits<float>::max()
#define FLOAT_MIN_VALUE std::numeric_limits<float>::min()
#define DOUBLE_MAX_VALUE std::numeric_limits<double>::max()
#define DOUBLE_MIN_VALUE std::numeric_limits<double>::min()

using namespace epics::pvData;

static SerializableControl* flusher;
static DeserializableControl* control;
static ByteBuffer* buffer;


class SerializableControlImpl : public SerializableControl,
        public NoDefaultMethods {
public:
    virtual void flushSerializeBuffer() {
    }

    virtual void ensureBuffer(std::size_t /*size*/) {
    }

    virtual void alignBuffer(std::size_t alignment) {
        buffer->align(alignment);
    }
    
    virtual bool directSerialize(ByteBuffer */*existingBuffer*/, const char* /*toSerialize*/,
                                 std::size_t /*elementCount*/, std::size_t /*elementSize*/)
    {
        return false;
    }

    virtual void cachedSerialize(std::tr1::shared_ptr<const Field> const & field, ByteBuffer* buffer)
    {
        field->serialize(buffer, this);
    }

    SerializableControlImpl() {
    }

    virtual ~SerializableControlImpl() {
    }
};

class DeserializableControlImpl : public DeserializableControl,
        public NoDefaultMethods {
public:
    virtual void ensureData(size_t /*size*/) {
    }

    virtual void alignData(size_t alignment) {
        buffer->align(alignment);
    }

    virtual bool directDeserialize(ByteBuffer */*existingBuffer*/, char* /*deserializeTo*/,
                                   std::size_t /*elementCount*/, std::size_t /*elementSize*/)
    {
        return false;
    }

    virtual std::tr1::shared_ptr<const Field> cachedDeserialize(ByteBuffer* buffer)
    {
        return getFieldCreate()->deserialize(buffer, this);
    }

    DeserializableControlImpl() {
    }

    virtual ~DeserializableControlImpl() {
    }
};

void serializationTest(PVFieldPtr const & field) {
    buffer->clear();

    // serialize
    field->serialize(buffer, flusher);

    buffer->flip();

    // create new instance and deserialize
    PVFieldPtr deserializedField = getPVDataCreate()->createPVField(field->getField());
    deserializedField->deserialize(buffer, control);

    // must equal
    bool isEqual = getConvert()->equals(*field,*deserializedField);
    assert(isEqual);
}

void testEquals(std::ostream& ofile) {
    ofile<<"Testing equals...\n";
    PVDataCreatePtr factory = getPVDataCreate();
    assert(factory.get()!=NULL);

	 // be sure all is covered
	 for (int i = pvBoolean; i < pvString; i++)
	 {
		 ScalarType scalarType = static_cast<ScalarType>(i);

		 PVScalarPtr scalar1 = factory->createPVScalar(scalarType);
		 PVScalarPtr scalar2 = factory->createPVScalar(scalarType);
		 assert((*scalar1)==(*scalar2));

		 PVScalarArrayPtr array1 = factory->createPVScalarArray(scalarType);
		 PVScalarArrayPtr array2 = factory->createPVScalarArray(scalarType);
		 assert((*array1)==(*array2));
	}

	// and a structure
    PVStructurePtr structure1 = factory->createPVStructure(getStandardField()->timeStamp());
    PVStructurePtr structure2 = factory->createPVStructure(getStandardField()->timeStamp());
	assert((*structure1)==(*structure2));

    // and a structure array
    PVStructureArrayPtr structureArray1 = factory->createPVStructureArray(getFieldCreate()->createStructureArray(structure1->getStructure()));
    PVStructureArrayPtr structureArray2 = factory->createPVStructureArray(getFieldCreate()->createStructureArray(structure2->getStructure()));
	assert((*structureArray1)==(*structureArray2));

    ofile<<"!!!  PASSED\n\n";
}

void testScalar(std::ostream& ofile) {
    ofile<<"Testing scalars...\n";
    PVDataCreatePtr factory = getPVDataCreate();
    assert(factory.get()!=NULL);

    ofile<<"\tPVBoolean\n";
    PVBooleanPtr pvBoolean =
    		std::tr1::static_pointer_cast<PVBoolean>(factory->createPVScalar(epics::pvData::pvBoolean));
    pvBoolean->put(false);
    serializationTest(pvBoolean);
    pvBoolean->put(true);
    serializationTest(pvBoolean);

    ofile<<"\tPVByte\n";
    PVBytePtr pvByte =
    		std::tr1::static_pointer_cast<PVByte>(factory->createPVScalar(epics::pvData::pvByte));
    pvByte->put(0);
    serializationTest(pvByte);
    pvByte->put(12);
    serializationTest(pvByte);
    pvByte->put(BYTE_MAX_VALUE);
    serializationTest(pvByte);
    pvByte->put(BYTE_MIN_VALUE);
    serializationTest(pvByte);

    ofile<<"\tPVShort\n";
    PVShortPtr pvShort =
    		std::tr1::static_pointer_cast<PVShort>(factory->createPVScalar(epics::pvData::pvShort));
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

    ofile<<"\tPVInt\n";
    PVIntPtr pvInt =
    		std::tr1::static_pointer_cast<PVInt>(factory->createPVScalar(epics::pvData::pvInt));
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

    ofile<<"\tPVLong\n";
    PVLongPtr pvLong =
    		std::tr1::static_pointer_cast<PVLong>(factory->createPVScalar(epics::pvData::pvLong));
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


    ofile<<"\tPVUByte\n";
    PVUBytePtr pvUByte =
    		std::tr1::static_pointer_cast<PVUByte>(factory->createPVScalar(epics::pvData::pvUByte));
    pvUByte->put(0);
    serializationTest(pvUByte);
    pvUByte->put(12);
    serializationTest(pvUByte);
    pvUByte->put(UBYTE_MAX_VALUE);
    serializationTest(pvUByte);
    pvUByte->put(BYTE_MAX_VALUE);
    serializationTest(pvUByte);
    pvUByte->put(BYTE_MIN_VALUE);
    serializationTest(pvUByte);

    ofile<<"\tPVUShort\n";
    PVUShortPtr pvUShort =
    		std::tr1::static_pointer_cast<PVUShort>(factory->createPVScalar(epics::pvData::pvUShort));
    pvUShort->put(0);
    serializationTest(pvUShort);
    pvUShort->put(1234);
    serializationTest(pvUShort);
    pvUShort->put(BYTE_MAX_VALUE);
    serializationTest(pvUShort);
    pvUShort->put(BYTE_MIN_VALUE);
    serializationTest(pvUShort);
    pvUShort->put(UBYTE_MAX_VALUE);
    serializationTest(pvUShort);
    pvUShort->put(SHORT_MAX_VALUE);
    serializationTest(pvUShort);
    pvUShort->put(SHORT_MIN_VALUE);
    serializationTest(pvUShort);
    pvUShort->put(USHORT_MAX_VALUE);
    serializationTest(pvUShort);

    ofile<<"\tPVInt\n";
    PVIntPtr pvUInt =
    		std::tr1::static_pointer_cast<PVInt>(factory->createPVScalar(epics::pvData::pvUInt));
    pvUInt->put(0);
    serializationTest(pvUInt);
    pvUInt->put(123456);
    serializationTest(pvUInt);
    pvUInt->put(BYTE_MAX_VALUE);
    serializationTest(pvUInt);
    pvUInt->put(BYTE_MIN_VALUE);
    serializationTest(pvUInt);
    pvUInt->put(UBYTE_MAX_VALUE);
    serializationTest(pvUInt);
    pvUInt->put(SHORT_MAX_VALUE);
    serializationTest(pvUInt);
    pvUInt->put(SHORT_MIN_VALUE);
    serializationTest(pvUInt);
    pvUInt->put(USHORT_MAX_VALUE);
    serializationTest(pvUInt);
    pvUInt->put(INT_MAX_VALUE);
    serializationTest(pvUInt);
    pvUInt->put(INT_MIN_VALUE);
    serializationTest(pvUInt);
    pvUInt->put(UINT_MAX_VALUE);
    serializationTest(pvUInt);

    ofile<<"\tPVLong\n";
    PVLongPtr pvULong =
    		std::tr1::static_pointer_cast<PVLong>(factory->createPVScalar(epics::pvData::pvULong));
    pvULong->put(0);
    serializationTest(pvULong);
    pvULong->put(12345678901LL);
    serializationTest(pvULong);
    pvULong->put(BYTE_MAX_VALUE);
    serializationTest(pvULong);
    pvULong->put(BYTE_MIN_VALUE);
    serializationTest(pvULong);
    pvULong->put(UBYTE_MAX_VALUE);
    serializationTest(pvULong);
    pvULong->put(SHORT_MAX_VALUE);
    serializationTest(pvULong);
    pvULong->put(SHORT_MIN_VALUE);
    serializationTest(pvULong);
    pvULong->put(USHORT_MAX_VALUE);
    serializationTest(pvULong);
    pvULong->put(INT_MAX_VALUE);
    serializationTest(pvULong);
    pvULong->put(INT_MIN_VALUE);
    serializationTest(pvULong);
    pvULong->put(UINT_MAX_VALUE);
    serializationTest(pvULong);
    pvULong->put(LONG_MAX_VALUE);
    serializationTest(pvULong);
    pvULong->put(LONG_MIN_VALUE);
    serializationTest(pvULong);
    pvULong->put(ULONG_MAX_VALUE);
    serializationTest(pvULong);

    ofile<<"\tPVFloat\n";
    PVFloatPtr pvFloat =
    		std::tr1::static_pointer_cast<PVFloat>(factory->createPVScalar(epics::pvData::pvFloat));
    pvFloat->put(0);
    serializationTest(pvFloat);
    pvFloat->put(12.345);
    serializationTest(pvFloat);
    pvFloat->put(FLOAT_MAX_VALUE);
    serializationTest(pvFloat);
    pvFloat->put(FLOAT_MIN_VALUE);
    serializationTest(pvFloat);

    ofile<<"\tPVDouble\n";
    PVDoublePtr pvDouble =
    		std::tr1::static_pointer_cast<PVDouble>(factory->createPVScalar(epics::pvData::pvDouble));
    pvDouble->put(0);
    serializationTest(pvDouble);
    pvDouble->put(12.345);
    serializationTest(pvDouble);
    pvDouble->put(DOUBLE_MAX_VALUE);
    serializationTest(pvDouble);
    pvDouble->put(DOUBLE_MIN_VALUE);
    serializationTest(pvDouble);

    ofile<<"\tPVString\n";
    PVStringPtr pvString =
    		std::tr1::static_pointer_cast<PVString>(factory->createPVScalar(epics::pvData::pvString));
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

    // TODO unsigned test

    ofile<<"!!!  PASSED\n\n";
}

void testArray(std::ostream& ofile) {
    ofile<<"Testing arrays...\n";

    PVDataCreatePtr factory = getPVDataCreate();
    assert(factory.get()!=NULL);

    ofile<<"\tPVBooleanArray\n";
    //bool boolEmpty[] = { false };
    //bool bv[] = { false, true, false, true, true };
    PVBooleanArrayPtr pvBoolean =
    		std::tr1::static_pointer_cast<PVBooleanArray>(factory->createPVScalarArray(epics::pvData::pvBoolean));
    //pvBoolean->put(0, 0, boolEmpty, 0);
    serializationTest(pvBoolean);
    //pvBoolean->put(0, 5, bv, 0);
    serializationTest(pvBoolean);

    ofile<<"\tPVByteArray\n";
    int8 byteEmpty[] = { 0 };
    int8 byv[] = { 0, 1, 2, -1, BYTE_MAX_VALUE, BYTE_MAX_VALUE-1,
            BYTE_MIN_VALUE+1, BYTE_MIN_VALUE };
    PVByteArrayPtr pvByte =
    		std::tr1::static_pointer_cast<PVByteArray>(factory->createPVScalarArray(epics::pvData::pvByte));
    pvByte->put(0, 0, byteEmpty, 0);
    serializationTest(pvByte);
    pvByte->put(0, 8, byv, 0);
    serializationTest(pvByte);

    ofile<<"\tPVShortArray\n";
    int16 shortEmpty[] = { 0 };
    int16 sv[] = { 0, 1, 2, -1, SHORT_MAX_VALUE, SHORT_MAX_VALUE-1,
            SHORT_MIN_VALUE+1, SHORT_MIN_VALUE };
    PVShortArrayPtr pvShort =
    		std::tr1::static_pointer_cast<PVShortArray>(factory->createPVScalarArray(epics::pvData::pvShort));
    pvShort->put(0, 0, shortEmpty, 0);
    serializationTest(pvShort);
    pvShort->put(0, 8, sv, 0);
    serializationTest(pvShort);

    ofile<<"\tPVIntArray\n";
    int32 intEmpty[] = { 0 };
    int32 iv[] = { 0, 1, 2, -1, INT_MAX_VALUE, INT_MAX_VALUE-1,
            INT_MIN_VALUE+1, INT_MIN_VALUE };
    PVIntArrayPtr pvInt =
    		std::tr1::static_pointer_cast<PVIntArray>(factory->createPVScalarArray(epics::pvData::pvInt));
    pvInt->put(0, 0, intEmpty, 0);
    serializationTest(pvInt);
    pvInt->put(0, 8, iv, 0);
    serializationTest(pvInt);

    ofile<<"\tPVLongArray\n";
    int64 longEmpty[] = { 0 };
    int64 lv[] = { 0, 1, 2, -1, LONG_MAX_VALUE, LONG_MAX_VALUE-1,
            LONG_MIN_VALUE+1, LONG_MIN_VALUE };
    PVLongArrayPtr pvLong =
    		std::tr1::static_pointer_cast<PVLongArray>(factory->createPVScalarArray(epics::pvData::pvLong));
    pvLong->put(0, 0, longEmpty, 0);
    serializationTest(pvLong);
    pvLong->put(0, 8, lv, 0);
    serializationTest(pvLong);

    ofile<<"\tPVUByteArray\n";
    uint8 ubyteEmpty[] = { 0 };
    uint8 ubyv[] = { 0, 1, 2, -1, BYTE_MAX_VALUE, BYTE_MAX_VALUE-1,
            BYTE_MIN_VALUE+1, BYTE_MIN_VALUE, UBYTE_MAX_VALUE };
    PVUByteArrayPtr pvUByte =
    		std::tr1::static_pointer_cast<PVUByteArray>(factory->createPVScalarArray(epics::pvData::pvUByte));
    pvUByte->put(0, 0, ubyteEmpty, 0);
    serializationTest(pvUByte);
    pvUByte->put(0, 9, ubyv, 0);
    serializationTest(pvUByte);

    ofile<<"\tPVUShortArray\n";
    uint16 ushortEmpty[] = { 0 };
    uint16 usv[] = { 0, 1, 2, -1, SHORT_MAX_VALUE, SHORT_MAX_VALUE-1,
            SHORT_MIN_VALUE+1, SHORT_MIN_VALUE, USHORT_MAX_VALUE };
    PVUShortArrayPtr pvUShort =
    		std::tr1::static_pointer_cast<PVUShortArray>(factory->createPVScalarArray(epics::pvData::pvUShort));
    pvUShort->put(0, 0, ushortEmpty, 0);
    serializationTest(pvUShort);
    pvUShort->put(0, 8, usv, 0);
    serializationTest(pvUShort);

    ofile<<"\tPVUIntArray\n";
    uint32 uintEmpty[] = { 0 };
    uint32 uiv[] = { 0, 1, 2, -1, INT_MAX_VALUE, INT_MAX_VALUE-1,
            INT_MIN_VALUE+1, INT_MIN_VALUE, UINT_MAX_VALUE };
    PVUIntArrayPtr pvUInt =
    		std::tr1::static_pointer_cast<PVUIntArray>(factory->createPVScalarArray(epics::pvData::pvUInt));
    pvUInt->put(0, 0, uintEmpty, 0);
    serializationTest(pvUInt);
    pvUInt->put(0, 9, uiv, 0);
    serializationTest(pvUInt);

    ofile<<"\tPVULongArray\n";
    uint64 ulongEmpty[] = { 0 };
    uint64 ulv[] = { 0, 1, 2, -1, LONG_MAX_VALUE, LONG_MAX_VALUE-1,
            LONG_MIN_VALUE+1, LONG_MIN_VALUE, ULONG_MAX_VALUE };
    PVULongArrayPtr pvULong =
    		std::tr1::static_pointer_cast<PVULongArray>(factory->createPVScalarArray(epics::pvData::pvULong));
    pvULong->put(0, 0, ulongEmpty, 0);
    serializationTest(pvULong);
    pvULong->put(0, 9, ulv, 0);
    serializationTest(pvULong);

    ofile<<"\tPVFloatArray\n";
    float floatEmpty[] = { (float)0.0 };
    float fv[] = { (float)0.0, (float)1.1, (float)2.3, (float)-1.4,
            FLOAT_MAX_VALUE, FLOAT_MAX_VALUE-(float)123456.789, FLOAT_MIN_VALUE
                    +(float)1.1, FLOAT_MIN_VALUE };
    PVFloatArrayPtr pvFloat =
    		std::tr1::static_pointer_cast<PVFloatArray>(factory->createPVScalarArray(epics::pvData::pvFloat));
    pvFloat->put(0, 0, floatEmpty, 0);
    serializationTest(pvFloat);
    pvFloat->put(0, 8, fv, 0);
    serializationTest(pvFloat);

    ofile<<"\tPVDoubleArray\n";
    double doubleEmpty[] = { (double)0.0 };
    double dv[] = { (double)0.0, (double)1.1, (double)2.3, (double)-1.4,
            DOUBLE_MAX_VALUE, DOUBLE_MAX_VALUE-(double)123456.789,
            DOUBLE_MIN_VALUE+(double)1.1, DOUBLE_MIN_VALUE };
    PVDoubleArrayPtr pvDouble =
    		std::tr1::static_pointer_cast<PVDoubleArray>(factory->createPVScalarArray(epics::pvData::pvDouble));
    pvDouble->put(0, 0, doubleEmpty, 0);
    serializationTest(pvDouble);
    pvDouble->put(0, 8, dv, 0);
    serializationTest(pvDouble);

    ofile<<"\tPVStringArray\n";
    String stringEmpty[] = { "" };
    String
            strv[] =
                    {
                            "",
                            "a",
                            "a b",
                            " ",
                            "test",
                            "smile",
                            "this is a little longer string... maybe a little but longer... this makes test better",
                            String(10000, 'b') };
    PVStringArrayPtr pvString =
    		std::tr1::static_pointer_cast<PVStringArray>(factory->createPVScalarArray(epics::pvData::pvString));
    pvString->put(0, 0, stringEmpty, 0);
    serializationTest(pvString);
    pvString->put(0, 8, strv, 0);
    serializationTest(pvString);

    ofile<<"!!!  PASSED\n\n";
}

void testNonInitialized(std::ostream& ofile) {
    ofile<<"Testing non-initialized...\n";
    PVDataCreatePtr factory = getPVDataCreate();
    assert(factory.get()!=NULL);

	 // be sure all is covered
	 for (int i = pvBoolean; i < pvString; i++)
	 {
		 ScalarType scalarType = static_cast<ScalarType>(i);

		 PVScalarPtr scalar = factory->createPVScalar(scalarType);
		 serializationTest(scalar);

		 PVScalarArrayPtr array = factory->createPVScalarArray(scalarType);
		 serializationTest(array);
	}

	// and a structure
	PVStructurePtr structure = factory->createPVStructure(getStandardField()->timeStamp());
	serializationTest(structure);

	// and a structure array
	PVStructureArrayPtr structureArray = factory->createPVStructureArray(getFieldCreate()->createStructureArray(structure->getStructure()));
	serializationTest(structureArray);

	ofile<<"!!!  PASSED\n\n";
}

void testStructure(std::ostream& ofile) {
    ofile<<"Testing structure...\n";

    PVDataCreatePtr factory = getPVDataCreate();
    assert(factory.get()!=NULL);

    ofile<<"\tSimple structure serialization\n";
	PVStructurePtr pvStructure = factory->createPVStructure(getStandardField()->timeStamp());
    pvStructure->getLongField("secondsPastEpoch")->put(123);
    pvStructure->getIntField("nanoSeconds")->put(456);
    pvStructure->getIntField("userTag")->put(789);

    serializationTest(pvStructure);

    ofile<<"\tComplex structure serialization\n";
	pvStructure = factory->createPVStructure(
			getStandardField()->structureArray(
					getStandardField()->timeStamp(), "alarm,control,display,timeStamp")
			);
	// TODO fill with data
    serializationTest(pvStructure);

    ofile<<"!!!  PASSED\n\n";
}



void testStructureId(std::ostream& ofile) {
    ofile<<"Testing structureID...\n";

    FieldCreatePtr fieldCreate = getFieldCreate();

    StringArray fieldNames;
    fieldNames.push_back("longField");
    fieldNames.push_back("intField");

    FieldConstPtrArray fields;
    fields.push_back(fieldCreate->createScalar(pvLong));
    fields.push_back(fieldCreate->createScalar(pvInt));

    StructureConstPtr structureWithNoId = fieldCreate->createStructure(fieldNames, fields);
    StructureConstPtr structure1 = fieldCreate->createStructure("id1", fieldNames, fields);
    StructureConstPtr structure2 = fieldCreate->createStructure("id2", fieldNames, fields);


    assert(structureWithNoId!=structure1);
    assert(structure1!=structure2);

    //serializationTest(structure1);

    PVStructurePtr pvStructure = getPVDataCreate()->createPVStructure(structure1);
    serializationTest(pvStructure);

    ofile<<"!!!  PASSED\n\n";
}

void serializatioTest(FieldConstPtr const & field)
{
	buffer->clear();

	// serialize
	field->serialize(buffer, flusher);

	// deserialize
	buffer->flip();

	FieldConstPtr deserializedField = getFieldCreate()->deserialize(buffer, control);

	// must equal
	bool isEqual = *field == *deserializedField;
	assert(isEqual);
	//assertEquals("field " + field.toString() + " serialization broken", field, deserializedField);
}

void testIntrospectionSerialization(std::ostream& ofile)
{
	 ofile<<"Testing introspection serialization...\n";

	 FieldCreatePtr factory = getFieldCreate();
	 assert(factory.get()!=NULL);

	 // be sure all is covered
	 for (int i = pvBoolean; i < pvString; i++)
	 {
		 ScalarType scalarType = static_cast<ScalarType>(i);

		 ScalarConstPtr scalar = factory->createScalar(scalarType);
		 serializatioTest(scalar);

		 ScalarArrayConstPtr array = factory->createScalarArray(scalarType);
		 serializatioTest(array);
	}

     // and a structure
     StructureConstPtr structure = getStandardField()->timeStamp();
     serializatioTest(structure);

     // and a structure array
     StructureArrayConstPtr structureArray = factory->createStructureArray(structure);
     serializatioTest(structureArray);

	 ofile<<"!!!  PASSED\n\n";
 }

void testStringCopy(std::ostream& ofile) {
    String s1 = "abc";
    String s2 = s1;
    if (s1.c_str() != s2.c_str())
        ofile << "\n!!! implementation of epics::pvData::String assignment operator does not share content !!!\n\n";
}

int main(int argc, char *argv[]) {
    std::ofstream outfile;
    std::ostream *out=NULL;
    if(argc>1) {
        outfile.open(argv[1]);
        if(outfile.is_open()){
            out=&outfile;
        }else{
            fprintf(stderr, "Failed to open test output file\n");
        }
    }
    if(!out) out=&std::cout;
    flusher = new SerializableControlImpl();
    control = new DeserializableControlImpl();
    buffer = new ByteBuffer(1<<16);

    testStringCopy(*out);

    testIntrospectionSerialization(*out);
    testEquals(*out);
    testNonInitialized(*out);

    testScalar(*out);
    testArray(*out);
    testStructure(*out);


    delete buffer;
    delete control;
    delete flusher;

    epicsExitCallAtExits();
    return 0;
}

