/* testBitSet.cpp */
/* Author:  Matej Sekoranja Date: 2010.10.18 */

#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "BitSet.h"


#include <epicsAssert.h>

using namespace epics::pvData;

void testGetSetClearFlip() {
    printf("testGetSetClearFlip... ");

    // empty
    BitSet* b1 = new BitSet();
    assert(b1->isEmpty());
    assert(b1->cardinality() == 0);
    // to string check
    std::string str; b1->toString(&str);
    assert(str == "{}");

    // one
    b1->set(3);
    assert(b1->get(3));
    assert(!b1->isEmpty());
    assert(b1->cardinality() == 1);
    // to string check
    str.clear(); b1->toString(&str);
    assert(str == "{3}");

    // grow
    b1->set(66);
    b1->set(67);
    b1->set(68);
    assert(b1->cardinality() == 4);
    str.clear(); b1->toString(&str);
    assert(str == "{3, 66, 67, 68}");

    // clear one
    b1->clear(67);
    assert(b1->cardinality() == 3);
    str.clear(); b1->toString(&str);
    assert(str == "{3, 66, 68}");

    // flip
    b1->flip(66);
    b1->flip(130);
    assert(b1->cardinality() == 3);
    str.clear(); b1->toString(&str);
    assert(str == "{3, 68, 130}");

    // flip
    b1->set(130, false);
    b1->set(4, true);
    assert(b1->cardinality() == 3);
    str.clear(); b1->toString(&str);
    assert(str == "{3, 4, 68}");

    // clear all
    b1->clear();
    assert(b1->isEmpty());
    assert(b1->cardinality() == 0);
    str.clear(); b1->toString(&str);
    assert(str == "{}");

    delete b1;
    printf("PASSED\n");

}

void testOperators() {
    printf("testOperators... ");

    BitSet b1;
    assert(b1 == b1);
    BitSet b2;
    assert(b1 == b2);

    b1.set(1);
    assert(!(b1 == b2));

    // different internal length, but the same
    b2.set(100);
    b2.set(1);
    b2.flip(100);
    assert(b1 == b2);

    // OR test
    b1.set(65);
    b1.set(106);
    b2.set(105);
    b1 |= b2;
    std::string str; b1.toString(&str);
    assert(str == "{1, 65, 105, 106}");

    // AND test
    b1.set(128);
    b1 &= b2;
    assert(b1 == b2);

    // XOR test
    b1.set(128);
    b1 ^= b2;
    assert(b1.cardinality() == 1 && b1.get(128) == true);

    // a AND (NOT b)
    b2 -= b1;
    str.clear(); b2.toString(&str);
    assert(str == "{1, 105}");
    b1.set(1);
    b2 -= b1;
    str.clear(); b2.toString(&str);
    assert(b2.cardinality() == 1 && b2.get(105) == true);

    // assign
    b1 = b2;
    assert(b1 == b2);

    // or_and
    b1.clear(); b1.set(2);
    b2.clear(); b2.set(66); b2.set(128);
    BitSet b3; b3.set(128); b3.set(520);
    b1.or_and(b2, b3);
    str.clear(); b1.toString(&str);
    assert(str == "{2, 128}");

    printf("PASSED\n");

}


int main(int argc,char *argv[])
{
    testGetSetClearFlip();
    testOperators();
    return(0);
}

