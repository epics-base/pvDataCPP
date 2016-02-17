/* testBitSet.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/* Author:  Matej Sekoranja Date: 2010.10.18 */

#include <iostream>
#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <sstream>

#include <pv/bitSet.h>

#include <epicsUnitTest.h>
#include <testMain.h>

using namespace epics::pvData;
using std::string;

static string toString(BitSet& bitSet)
{
    std::ostringstream oss;
    oss << bitSet;
    return oss.str();
}

static void testGetSetClearFlip()
{
    testDiag("testGetSetClearFlip... ");

    // empty
    BitSet* b1 = new BitSet();
    testOk1(b1->isEmpty());
    testOk1(b1->cardinality() == 0);
    // to string check
    string str = toString(*b1);
    testOk1(str == "{}");

    // one
    b1->set(3);
    testOk1(b1->get(3));
    testOk1(!b1->isEmpty());
    testOk1(b1->cardinality() == 1);
    // to string check
    str = toString(*b1);
    testOk1(str == "{3}");

    // grow
    b1->set(66);
    b1->set(67);
    b1->set(68);
    testOk1(b1->cardinality() == 4);
    str = toString(*b1);
    testOk1(str == "{3, 66, 67, 68}");

    // clear one
    b1->clear(67);
    testOk1(b1->cardinality() == 3);
    str = toString(*b1);
    testOk1(str == "{3, 66, 68}");

    // flip
    b1->flip(66);
    b1->flip(130);
    testOk1(b1->cardinality() == 3);
    str = toString(*b1);
    testOk1(str == "{3, 68, 130}");

    // flip
    b1->set(130, false);
    b1->set(4, true);
    testOk1(b1->cardinality() == 3);
    str = toString(*b1);
    testOk1(str == "{3, 4, 68}");

    // clear all
    b1->clear();
    testOk1(b1->isEmpty());
    testOk1(b1->cardinality() == 0);
    str = toString(*b1);
    testOk1(str == "{}");

    delete b1;
}

static void testOperators()
{
    testDiag("testOperators... ");

    BitSet b1;
    testOk1(b1 == b1);
    BitSet b2;
    testOk1(b1 == b2);

    b1.set(1);
    testOk1(!(b1 == b2));

    testDiag("different internal length, but the same");
    b2.set(100);
    b2.set(1);
    b2.flip(100);
    testOk1(b1 == b2);

    testDiag("OR test");
    b2.set(65);
    b2.set(106);
    b2.set(105);
    b1 |= b2;
    string str = toString(b1);
    testOk1(str == "{1, 65, 105, 106}");
    b1.clear();
    b1 |= b2;
    str = toString(b1);
    testOk1(str == "{1, 65, 105, 106}");

    testDiag("AND test");
    b1.set(128);
    b1 &= b2;
    testOk1(b1 == b2);

    testDiag("XOR test");
    b1.set(128);
    b1 ^= b2;
    testOk1((b1.cardinality() == 1 && b1.get(128) == true));
    b1.clear();
    b2.clear();
    b1.set(1);
    b2.set(256);
    b1 ^= b2;
    testOk1((b1.cardinality() == 2 && b1.get(1) == true && b1.get(256) == true));
    

    testDiag("assign");
    b1 = b2;
    testOk1(b1 == b2);

    testDiag("or_and");
    b1.clear(); b1.set(2);
    b2.clear(); b2.set(66); b2.set(128);
    BitSet b3; b3.set(128); b3.set(520);
    b1.or_and(b2, b3);
    str = toString(b1);
    testOk1(str == "{2, 128}");

    b1.clear(); b1.set(1);
    b2.clear();
    b3.clear(); b3.set(1);
    std::cout<<"# "<<toString(b3)<<" |= "<<toString(b1)<<" & "<<toString(b2)<<"\n";
    b3.or_and(b1, b2);
    testTodoBegin("Bug in or_and");
    testOk(toString(b3) == "{1}", "%s == {1}", toString(b3).c_str());
    testTodoEnd();
}


MAIN(testBitSet)
{
    testPlan(29);
    testGetSetClearFlip();
    testOperators();
    return testDone();
}

