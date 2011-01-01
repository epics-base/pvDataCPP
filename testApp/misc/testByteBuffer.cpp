/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/*
 * testByteBuffer.cpp
 *
 *  Created on: Oct 20, 2010
 *      Author: Miha Vitorovic
 */

#include <iostream>
#include <cstring>

#include <epicsAssert.h>
#include <epicsEndian.h>

#include "byteBuffer.h"
#include "pvIntrospect.h"

using namespace epics::pvData;
using std::cout;

void testBasicOperations() {
    cout<<"Basic operation tests...\n";

    ByteBuffer* buff = new ByteBuffer();
    assert(buff->getSize()==32);
    assert(buff->getByteOrder()==EPICS_BYTE_ORDER);
    assert(buff->getPosition()==0);
    assert(buff->getLimit()==32);
    assert(buff->getRemaining()==32);

    buff->putBoolean(true);
    assert(buff->getPosition()==1);
    assert(buff->getRemaining()==31);

    buff->putByte(-12);
    assert(buff->getPosition()==2);
    assert(buff->getRemaining()==30);

    buff->putShort(10516);
    assert(buff->getPosition()==4);
    assert(buff->getRemaining()==28);

    buff->putInt(0x1937628B);
    assert(buff->getPosition()==8);
    assert(buff->getRemaining()==24);

    buff->putLong(2345678123LL);
    assert(buff->getPosition()==16);
    assert(buff->getRemaining()==16);

    float testFloat = 34.67;
    buff->putFloat(testFloat);
    assert(buff->getPosition()==20);
    assert(buff->getRemaining()==12);

    double testDouble = -512.23974;
    buff->putDouble(testDouble);
    assert(buff->getPosition()==28);
    assert(buff->getRemaining()==4);

    // testing direct reads
    assert(buff->getBoolean(0)==true);
    assert(buff->getByte(1)==-12);
    assert(buff->getShort(2)==10516);
    assert(buff->getInt(4)==0x1937628B);
    assert(buff->getLong(8)==2345678123LL);
    assert(buff->getFloat(16)==testFloat);
    assert(buff->getDouble(20)==testDouble);

    buff->flip();
    assert(buff->getLimit()==28);
    assert(buff->getPosition()==0);
    assert(buff->getRemaining()==28);

    assert(buff->getBoolean()==true);
    assert(buff->getPosition()==1);

    assert(buff->getByte()==-12);
    assert(buff->getPosition()==2);

    assert(buff->getShort()==10516);
    assert(buff->getPosition()==4);

    assert(buff->getInt()==0x1937628B);
    assert(buff->getPosition()==8);

    assert(buff->getLong()==2345678123LL);
    assert(buff->getPosition()==16);

    assert(buff->getFloat()==testFloat);
    assert(buff->getPosition()==20);

    assert(buff->getDouble()==testDouble);
    assert(buff->getPosition()==28);

    buff->clear();
    assert(buff->getPosition()==0);
    assert(buff->getLimit()==32);
    assert(buff->getRemaining()==32);

    buff->setPosition(4);
    assert(buff->getPosition()==4);
    assert(buff->getLimit()==32);
    assert(buff->getRemaining()==(32-4));

    buff->setPosition(13);
    assert(buff->getPosition()==13);
    assert(buff->getLimit()==32);
    assert(buff->getRemaining()==(32-13));

    // testing absolute puts
    buff->clear();
    buff->setPosition(28);
    buff->putBoolean(0, true);
    buff->putByte(1, -12);
    buff->putShort(2, 10516);
    buff->putInt(4, 0x1937628B);
    buff->putLong(8, 2345678123LL);
    buff->putFloat(16, testFloat);
    buff->putDouble(20, testDouble);

    buff->flip();
    assert(buff->getLimit()==28);
    assert(buff->getPosition()==0);
    assert(buff->getRemaining()==28);

    assert(buff->getBoolean()==true);
    assert(buff->getPosition()==1);

    assert(buff->getByte()==-12);
    assert(buff->getPosition()==2);

    assert(buff->getShort()==10516);
    assert(buff->getPosition()==4);

    assert(buff->getInt()==0x1937628B);
    assert(buff->getPosition()==8);

    assert(buff->getLong()==2345678123LL);
    assert(buff->getPosition()==16);

    assert(buff->getFloat()==testFloat);
    assert(buff->getPosition()==20);

    assert(buff->getDouble()==testDouble);
    assert(buff->getPosition()==28);

    buff->clear();
    assert(buff->getPosition()==0);
    assert(buff->getLimit()==32);
    assert(buff->getRemaining()==32);

    char src[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
            'm' };
    char dst[] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
            ' ' };

    buff->put(src, 2, 6);
    assert(buff->getPosition()==6);
    assert(strncmp(buff->getArray(),&src[2],6)==0);

    buff->flip();
    assert(buff->getLimit()==6);
    assert(buff->getPosition()==0);
    assert(buff->getRemaining()==6);

    buff->get(dst, 2, 6);
    assert(buff->getLimit()==6);
    assert(buff->getPosition()==6);
    assert(strncmp(&src[2],&dst[2],6)==0);

    cout<<"    First 10 characters of destination: >>"<<String(dst, 10)<<"<<\n";

    delete buff;

    cout<<"!!!  PASSED\n";
}

void testInverseEndianness() {
    cout<<"Testing inverse endianness...\n";

#if EPICS_BYTE_ORDER==EPICS_ENDIAN_BIG
    ByteBuffer* buff = new ByteBuffer(32,EPICS_ENDIAN_LITTLE);
    char refBuffer[] =
    {   (char)0x02, (char)0x01, (char)0x0D, (char)0x0C, (char)0x0B, (char)0x0A};
#else
    ByteBuffer* buff = new ByteBuffer(32, EPICS_ENDIAN_BIG);
    char refBuffer[] = { (char)0x01, (char)0x02, (char)0x0A, (char)0x0B,
            (char)0x0C, (char)0x0D };
#endif

    buff->putShort(0x0102);
    buff->putInt(0x0A0B0C0D);

    assert(strncmp(buff->getArray(),refBuffer,6)==0);

    buff->flip();

    assert(buff->getShort()==0x0102);
    assert(buff->getInt()==0x0A0B0C0D);

    delete buff;
    cout<<"!!!  PASSED\n";
}

int main(int argc, char *argv[]) {
    testBasicOperations();
    testInverseEndianness();
    return (0);
}
