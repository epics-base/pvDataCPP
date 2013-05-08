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
#include <epicsUnitTest.h>
#include <testMain.h>

#include <pv/requester.h>
#include <pv/pvIntrospect.h>
#include <pv/pvData.h>
#include <pv/convert.h>
#include <pv/standardField.h>
#include <pv/standardPVField.h>

using namespace epics::pvData;
using std::tr1::static_pointer_cast;

namespace {

static void testFactory()
{
    testDiag("Check array creation");

    for(ScalarType e=pvBoolean; e<=pvString; e=(ScalarType)(1+(int)e))
    {
        testDiag("Check type %s", ScalarTypeFunc::name(e));
        PVScalarArrayPtr arr = getPVDataCreate()->createPVScalarArray(e);
        testOk1(arr.get()!=NULL);
        if(!arr.get())
            continue;
        testOk1(arr->getScalarArray()->getElementType()==e);
        testOk1(arr->getLength()==0);
        arr->setLength(10);
        testOk1(arr->getLength()==10);
        testOk1(arr->getCapacity()>=10);
        arr->setLength(0);
        testOk1(arr->getLength()==0);
    }
}

template<typename PVT>
struct basicTestData {
    static inline void fill(typename PVT::svector& data) {
        data.resize(100);
        for(typename PVT::value_type i=0;
            (size_t)i<data.size();
            i++)
        {
            data[i] = 10*i;
        }
    }
};


template<>
struct basicTestData<PVStringArray> {
    static inline void fill(PVStringArray::svector& data) {
        PVIntArray::svector idata;
        basicTestData<PVIntArray>::fill(idata);
        data.resize(idata.size());
        castUnsafeV(data.size(), pvString, data.data(), pvInt, idata.data());
    }
};

template<typename PVT>
static void testBasic()
{
    testDiag("Check basic array operations for %s", typeid(PVT).name());

    typename PVT::shared_pointer arr1 = static_pointer_cast<PVT>(getPVDataCreate()->createPVScalarArray(PVT::typeCode));
    typename PVT::shared_pointer arr2 = static_pointer_cast<PVT>(getPVDataCreate()->createPVScalarArray(PVT::typeCode));

    testOk1(*arr1==*arr2);
    testOk1(*arr1==*arr1);
    testOk1(*arr1->getScalarArray()==*arr2->getScalarArray());

    typename PVT::svector data;
    data.reserve(200);
    basicTestData<PVT>::fill(data);

    testOk1(data.unique());
    arr1->replace(data);
    testOk1(!data.unique());

    testOk1(arr1->getLength()==data.size());

    testOk1(*arr1!=*arr2);

    data.clear();

    testOk1(arr1->viewUnsafe().unique());

    arr2->assign(*arr1);

    testOk1(*arr1==*arr2);
    testOk1(!arr1->viewUnsafe().unique());

    arr2->swap(data);

    testOk1(arr2->getLength()==0);
    testOk1(data.size()==arr1->getLength());

    PVIntArray::svector idata;
    arr1->PVScalarArray::getAs<pvInt>(idata);

    testOk1(idata[1]==10);

    idata.make_unique();
    idata[1] = 42;

    arr1->PVScalarArray::putFrom<pvInt>(idata);

    testOk1(castUnsafe<PVIntArray::value_type>(arr1->viewUnsafe()[1])==42);
}

static void testShare()
{
    testDiag("Check array data sharing");

    PVIntArrayPtr iarr = static_pointer_cast<PVIntArray>(getPVDataCreate()->createPVScalarArray(pvInt));
    PVStringArrayPtr sarr = static_pointer_cast<PVStringArray>(getPVDataCreate()->createPVScalarArray(pvString));

    PVIntArray::svector idata(4, 1);

    sarr->PVScalarArray::putFrom<pvInt>(idata); // copy and convert

    testOk1(idata.unique());

    iarr->PVScalarArray::putFrom<pvInt>(idata); // take a reference

    testOk1(!idata.unique());

    idata.clear();

    sarr->PVScalarArray::getAs<pvInt>(idata); // copy and convert

    testOk1(idata.unique());

    iarr->PVScalarArray::getAs<pvInt>(idata); // take a reference

    testOk1(!idata.unique());
}

} // end namespace

MAIN(testPVScalarArray)
{
    testPlan(146);
    testFactory();
    testBasic<PVByteArray>();
    testBasic<PVUByteArray>();
    testBasic<PVIntArray>();
    testBasic<PVDoubleArray>();
    testBasic<PVStringArray>();
    testShare();
    return testDone();
}
