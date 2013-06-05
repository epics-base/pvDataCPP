/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/* Author:  Michael Davidsaver */

#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include <vector>

#include <epicsUnitTest.h>
#include <testMain.h>

#include "pv/sharedVector.h"

static void testEmpty()
{
    testDiag("Test empty vector");
    epics::pvData::shared_vector<int> empty, empty2;

    testOk1(empty.size()==0);
    testOk1(empty.empty());
    testOk1(empty.begin()==empty.end());
    testOk1(empty.unique());

    testOk1(empty.data()==NULL);

    testOk1(empty==empty);
    testOk1(!(empty!=empty));
    testOk1(empty==empty2);
    testOk1(!(empty!=empty2));
}

static void testInternalAlloc()
{
    testDiag("Test vector alloc w/ new[]");

    epics::pvData::shared_vector<int> internal(5);

    testOk1(internal.size()==5);
    testOk1(!internal.empty());
    testOk1(internal.unique());
    testOk1(internal.data()!=NULL);

    testOk1(internal.begin()+5==internal.end());
    testOk1(internal.begin()==internal.end()-5);
    testOk1(internal.begin()+2==internal.end()-3);
    testOk1(internal.end()-internal.begin()==5);

    internal[2] = 42;
    testOk1(internal[2]==42);

    epics::pvData::shared_vector<int> internal2(15, 500);

    testOk1(internal2.size()==15);
    testOk1(internal2[1]==500);

    internal.swap(internal2);

    testOk1(internal2.size()==5);
    testOk1(internal.size()==15);

    internal.clear();

    testOk1(internal.size()==0);
    testOk1(internal.empty());
    testOk1(internal.unique());
    testOk1(internal.data()==NULL);
}

namespace {
    //Note: STL shared_ptr requires that deletors be copy constructable
    template<typename E>
    struct callCounter {
        std::tr1::shared_ptr<int> count;
        callCounter():count(new int){*count=0;}
        callCounter(const callCounter& o):count(o.count) {};
        callCounter& operator=(const callCounter& o){count=o.count;}
        void operator()(E){*count=1;}
    };
}

static void testExternalAlloc()
{
    testDiag("Test vector external alloc");

    // Simulate a failed malloc() or similar
    int *oops=0;
    epics::pvData::shared_vector<int> nullPtr(oops, 42, 100);

    testOk1(nullPtr.size()==0);
    testOk1(nullPtr.empty());
    testOk1(nullPtr.begin()==nullPtr.end());
    testOk1(nullPtr.unique());

    testOk1(nullPtr.data()==NULL);

    int *raw=new int[5];
    epics::pvData::shared_vector<int> newData(raw, 1, 4);

    testOk1(newData.size()==4);
    testOk1(!newData.empty());

    // check that offset is used
    raw[1]=14;
    testOk1(newData[0]==14);

    // Check use of custom deleter
    int localVar[4] = {1,2,3,4};
    callCounter<int*> tracker;
    testOk1(*tracker.count==0);

    epics::pvData::shared_vector<int> locvar(localVar,
                                             tracker,
                                             0, 4);

    testOk1(locvar[1]==2);
    testOk1(*tracker.count==0);

    newData.swap(locvar);

    testOk1(*tracker.count==0);

    newData.clear();

    testOk1(*tracker.count==1);
}

static void testShare()
{
    testDiag("Test vector Sharing");

    epics::pvData::shared_vector<int> one, two(15);
    epics::pvData::shared_vector<int> three(two);

    testOk1(one.unique());
    testOk1(!two.unique());
    testOk1(!three.unique());

    testOk1(two.data() == three.data());

    one = two;

    testOk1(!one.unique());

    testOk1(two.data() == one.data());

    one = one; // no-op

    testOk1(two.data() == one.data());

    one[1] = 43;
    testOk1(two[1]==43);
    testOk1(three[1]==43);

    one.make_unique();

    testOk1(one[1]==43);
    one[1] = 143;
    testOk1(two[1]==43);
    testOk1(three[1]==43);

    two.resize(two.size());

    testOk1(two[1]==43);
    two[1] = 243;
    testOk1(one[1]==143);
    testOk1(three[1]==43);

    testOk1(one.unique());
    testOk1(two.unique());
    testOk1(three.unique());

    one.resize(2);

    testOk1(one.size()==2);
    testOk1(one[1]==143);
    testOk1(two.size()==15);
    testOk1(three.size()==15);

    two.resize(20, 5000);

    testOk1(two[1]==243);
    testOk1(one.size()==2);
    testOk1(two.size()==20);
    testOk1(three.size()==15);

    testOk1(two[19]==5000);
}

static void testConst()
{
    testDiag("Test constant vector");

    epics::pvData::shared_vector<int> writable(15, 100);

    epics::pvData::shared_vector<int>::reference wr = writable[0];
    epics::pvData::shared_vector<int>::const_reference ror = writable[0];

    testOk1(wr==ror);

    // can re-target container, but data is R/O
    epics::pvData::shared_vector<const int> rodata(writable);

    epics::pvData::shared_vector<const int>::reference wcr = rodata[0];
    epics::pvData::shared_vector<const int>::const_reference rocr = rodata[0];

    testOk1(wcr==rocr);

    // Data is R/W, but container can't be re-targeted
    const epics::pvData::shared_vector<int> roref(writable);

    // Can't change anything.
    const epics::pvData::shared_vector<const int> fixed(writable);

    testOk1(rodata[1]==100);

    // intentionally modify shared data!
    // safe since this thread owns all references.
    writable[1]=200;

    testOk1(rodata[1]==200);

    epics::pvData::shared_vector<int>::const_iterator a;
    epics::pvData::shared_vector<const int>::iterator b;
    epics::pvData::shared_vector<const int>::const_iterator c;

    a = writable.begin();
    b = rodata.begin();
    c = rodata.end();

    testOk1(std::equal(b, c, a));

    a = writable.cbegin();
    c = rodata.cend();

    epics::pvData::shared_vector<int>::const_reference x = rodata[1];

    testOk1(x==200);
}

static void testSlice()
{
    testDiag("Test vector slicing");

    epics::pvData::shared_vector<int> original(10, 100);

    epics::pvData::shared_vector<int> half1(original), half2(original), half2a(original);

    half1.slice(0, 5);
    half2.slice(5, 5);
    half2a.slice(5);

    testOk1(half1.dataOffset()==0);
    testOk1(half2.dataOffset()==5);
    testOk1(half2a.dataOffset()==5);

    testOk1(half2.size() == half2a.size());

    testOk1(original.data() == half1.data());
    testOk1(half2.data() == half2a.data());

    half1.slice(100000);
    half2.slice(1);
    half2a.slice(1,1);

    testOk1(half1.size()==0);
    testOk1(half2.size()==4);
    testOk1(half2a.size()==1);

    testOk1(half1.dataOffset()==10);
    testOk1(half2.dataOffset()==6);
    testOk1(half2a.dataOffset()==6);

    half2.clear();
    testOk1(half2.dataOffset()==0);
    testOk1(half2.dataCount()==0);
    testOk1(half2.dataTotal()==0);
    testOk1(half2.data()==NULL);
}

static void testCapacity()
{
    testDiag("Test vector capacity");

    epics::pvData::shared_vector<int> vect(10, 100);

    int *peek = vect.dataPtr().get();

    vect.slice(0, 5);

    testOk1(vect.size()==5);
    testOk1(vect.dataTotal()==10);
    testOk1(vect.dataPtr().get() == peek);

    vect.resize(6);

    testOk1(vect.dataPtr().get() == peek);
    testOk1(vect.size()==6);
    testOk1(vect.dataTotal()==10);

    vect.resize(10);

    testOk1(vect.dataPtr().get() == peek);
    testOk1(vect.size()==10);
    testOk1(vect.dataTotal()==10);

    vect.resize(11);

    testOk1(vect.dataPtr().get() != peek);
    testOk1(vect.size()==11);
    testOk1(vect.dataTotal()>=11);

    vect[1] = 124;

    vect.reserve(15);

    testOk1(vect.size()==11);
    testOk1(vect.dataTotal()>=15);

    testOk1(vect[1]==124);
}

static void testVoid()
{
    testDiag("Test vecter cast to/from void");

    epics::pvData::shared_vector<int> typed(4);
    epics::pvData::shared_vector<void> untyped;
    epics::pvData::shared_vector<const void> constuntyped;

    untyped = epics::pvData::const_shared_vector_cast<void>(constuntyped);

    epics::pvData::shared_vector<void> untyped2(epics::pvData::static_shared_vector_cast<void>(typed));

    testOk1(typed.dataPtr().get()==untyped2.dataPtr().get());
    testOk1(typed.size()*sizeof(int)==untyped2.size());

    untyped2.slice(sizeof(int), 2*sizeof(int));

    typed = epics::pvData::static_shared_vector_cast<int>(untyped2);

    testOk1(typed.dataOffset()==1);
    testOk1(typed.size()==2);
}

struct dummyStruct {};

static void testNonPOD()
{
    testDiag("Test vector of non-POD types");

    epics::pvData::shared_vector<std::string> strings(6);
    epics::pvData::shared_vector<std::tr1::shared_ptr<dummyStruct> > structs(5);

    testOk1(strings[0].empty());
    testOk1(structs[0].get()==NULL);

    structs[1].reset(new dummyStruct);
    dummyStruct *temp = structs[1].get();

    epics::pvData::shared_vector<std::tr1::shared_ptr<dummyStruct> > structs2(structs);

    testOk1(!structs.unique());
    testOk1(structs[1].unique());

    testOk1(structs2[1].get()==temp);

    structs2.make_unique();

    testOk1(structs.unique());
    testOk1(!structs[1].unique());

    testOk1(structs2[1].get()==temp);
}

MAIN(testSharedVector)
{
    testPlan(115);
    testDiag("Tests for shared_vector");

    testDiag("sizeof(shared_vector<int>)=%lu",
             (unsigned long)sizeof(epics::pvData::shared_vector<int>));

    testEmpty();
    testInternalAlloc();
    testExternalAlloc();
    testCapacity();
    testShare();
    testConst();
    testSlice();
    testVoid();
    testNonPOD();
    return testDone();
}
