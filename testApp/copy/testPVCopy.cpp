/*testPVCopyMain.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 * @author mrk
 */

/* Author: Marty Kraimer */

#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>
#include <memory>
#include <iostream>

#include <epicsUnitTest.h>
#include <testMain.h>

#include <pv/standardField.h>
#include <pv/standardPVField.h>
#include <pv/convert.h>
#include <pv/pvCopy.h>
#include <pv/createRequest.h>


using namespace std;
using std::tr1::static_pointer_cast;
using namespace epics::pvData;

//static bool debug = true;

static void testPVScalar(
    String const & valueNameMaster,
    String const & valueNameCopy,
    PVStructurePtr const & pvMaster,
    PVCopyPtr const & pvCopy)
{
    PVStructurePtr pvStructureCopy;
    PVFieldPtr pvField;
    PVScalarPtr pvValueMaster;
    PVScalarPtr pvValueCopy;
    BitSetPtr bitSet;
    String builder;
    size_t offset;
    ConvertPtr convert = getConvert();

    cout << endl;
    pvField = pvMaster->getSubField(valueNameMaster);
    pvValueMaster = static_pointer_cast<PVScalar>(pvField);
    convert->fromDouble(pvValueMaster,.04);
    StructureConstPtr structure = pvCopy->getStructure();
    builder.clear(); structure->toString(&builder);
    cout << "structure from copy" << endl << builder << endl;
    pvStructureCopy = pvCopy->createPVStructure();
    pvField = pvStructureCopy->getSubField(valueNameCopy);
    pvValueCopy = static_pointer_cast<PVScalar>(pvField);
    bitSet = BitSetPtr(new BitSet(pvStructureCopy->getNumberFields()));
    pvCopy->initCopy(pvStructureCopy, bitSet);
    cout << "after initCopy pvValueCopy " << convert->toDouble(pvValueCopy);
    cout << endl;
    convert->fromDouble(pvValueMaster,.06);
    pvCopy->updateCopySetBitSet(pvStructureCopy,bitSet);
    cout << "after put(.06) pvValueCopy " << convert->toDouble(pvValueCopy);
    builder.clear();
    bitSet->toString(&builder);
    cout << " bitSet " << builder;
    cout << endl;
    offset = pvCopy->getCopyOffset(pvValueMaster);
    cout << "getCopyOffset() " << offset;
    cout << " pvValueCopy->getOffset() " << pvValueCopy->getFieldOffset();
    cout << " pvValueMaster->getOffset() " << pvValueMaster->getFieldOffset();
    cout << " bitSet " << builder;
    cout << endl;
    bitSet->clear();
    convert->fromDouble(pvValueMaster,1.0);
    builder.clear();
    bitSet->toString(&builder);
    cout << "before updateCopyFromBitSet";
    cout << " masterValue " << convert->toDouble(pvValueMaster);
    cout << " copyValue " << convert->toDouble(pvValueCopy);
    cout << " bitSet " << builder;
    cout << endl;
    bitSet->set(0);
    pvCopy->updateCopyFromBitSet(pvStructureCopy,bitSet);
    cout << "after updateCopyFromBitSet";
    cout << " masterValue " << convert->toDouble(pvValueMaster);
    cout << " copyValue " << convert->toDouble(pvValueCopy);
    cout << " bitSet " << builder;
    cout << endl;
    convert->fromDouble(pvValueCopy,2.0);
    bitSet->set(0);
    cout << "before updateMaster";
    cout << " masterValue " << convert->toDouble(pvValueMaster);
    cout << " copyValue " << convert->toDouble(pvValueCopy);
    cout << " bitSet " << builder;
    cout << endl;
    pvCopy->updateMaster(pvStructureCopy,bitSet);
    cout << "after updateMaster";
    cout << " masterValue " << convert->toDouble(pvValueMaster);
    cout << " copyValue " << convert->toDouble(pvValueCopy);
    cout << " bitSet " << builder;
    cout << endl;
}

static void testPVScalarArray(
    ScalarType scalarType,
    String const & valueNameMaster,
    String const & valueNameCopy,
    PVStructurePtr const & pvMaster,
    PVCopyPtr const & pvCopy)
{
    PVStructurePtr pvStructureCopy;
    PVScalarArrayPtr pvValueMaster;
    PVScalarArrayPtr pvValueCopy;
    BitSetPtr bitSet;
    String builder;
    size_t offset;
    size_t n = 5;
    shared_vector<double> values(n);
    cout << endl;
    pvValueMaster = pvMaster->getScalarArrayField(valueNameMaster,scalarType);
    for(size_t i=0; i<n; i++) values[i] = i;
    const shared_vector<const double> xxx(freeze(values));
    pvValueMaster->putFrom(xxx);
    StructureConstPtr structure = pvCopy->getStructure();
    builder.clear(); structure->toString(&builder);
    cout << "structure from copy" << endl << builder << endl;
    pvStructureCopy = pvCopy->createPVStructure();
    pvValueCopy = pvStructureCopy->getScalarArrayField(valueNameCopy,scalarType);
    bitSet = BitSetPtr(new BitSet(pvStructureCopy->getNumberFields()));
    pvCopy->initCopy(pvStructureCopy, bitSet);
    builder.clear(); pvValueCopy->toString(&builder);
    cout << "after initCopy pvValueCopy " << builder << endl;
    cout << endl;
    values.resize(n);
    for(size_t i=0; i<n; i++) values[i] = i + .06;
    const shared_vector<const double> yyy(freeze(values));
    pvValueMaster->putFrom(yyy);
    pvCopy->updateCopySetBitSet(pvStructureCopy,bitSet);
    builder.clear(); pvValueCopy->toString(&builder);
    cout << "after put(i+ .06) pvValueCopy " << builder << endl;
    builder.clear();
    bitSet->toString(&builder);
    cout << " bitSet " << builder;
    cout << endl;
    offset = pvCopy->getCopyOffset(pvValueMaster);
    cout << "getCopyOffset() " << offset;
    cout << " pvValueCopy->getOffset() " << pvValueCopy->getFieldOffset();
    cout << " pvValueMaster->getOffset() " << pvValueMaster->getFieldOffset();
    builder.clear();
    bitSet->toString(&builder);
    cout << " bitSet " << builder;
    cout << endl;
    bitSet->clear();
    values.resize(n);
    for(size_t i=0; i<n; i++) values[i] = i + 1.0;
    const shared_vector<const double> zzz(freeze(values));
    pvValueMaster->putFrom(zzz);
    builder.clear();
    bitSet->toString(&builder);
    cout << "before updateCopyFromBitSet";
    builder.clear(); pvValueMaster->toString(&builder);
    cout << " masterValue " << builder << endl;
    builder.clear(); pvValueCopy->toString(&builder);
    cout << " copyValue " << builder << endl;
    cout << " bitSet " << builder;
    builder.clear();
    bitSet->toString(&builder);
    cout << endl;
    bitSet->set(0);
    pvCopy->updateCopyFromBitSet(pvStructureCopy,bitSet);
    cout << "after updateCopyFromBitSet";
    builder.clear(); pvValueMaster->toString(&builder);
    cout << " masterValue " << builder << endl;
    builder.clear(); pvValueCopy->toString(&builder);
    cout << " copyValue " << builder << endl;
    builder.clear();
    bitSet->toString(&builder);
    cout << " bitSet " << builder;
    cout << endl;
    values.resize(n);
    for(size_t i=0; i<n; i++) values[i] = i + 2.0;
    const shared_vector<const double> ttt(freeze(values));
    pvValueMaster->putFrom(ttt);
    bitSet->set(0);
    cout << "before updateMaster";
    builder.clear(); pvValueMaster->toString(&builder);
    cout << " masterValue " << builder << endl;
    builder.clear(); pvValueCopy->toString(&builder);
    cout << " copyValue " << builder << endl;
    builder.clear();
    bitSet->toString(&builder);
    cout << " bitSet " << builder;
    cout << endl;
    pvCopy->updateMaster(pvStructureCopy,bitSet);
    cout << "after updateMaster";
    builder.clear(); pvValueMaster->toString(&builder);
    cout << " masterValue " << builder << endl;
    builder.clear(); pvValueCopy->toString(&builder);
    cout << " copyValue " << builder << endl;
    builder.clear();
    bitSet->toString(&builder);
    cout << " bitSet " << builder;
    cout << endl;
}
    
static void scalarTest()
{
    cout << endl << endl << "****scalarTest****" << endl;
    PVStructurePtr pvMaster;
    String request;
    PVStructurePtr pvRequest;
    PVFieldPtr pvMasterField;
    PVCopyPtr pvCopy;
    String builder;
    String valueNameMaster;
    String valueNameCopy;

    StandardPVFieldPtr standardPVField = getStandardPVField();
    pvMaster = standardPVField->scalar(pvDouble,"alarm,timeStamp,display");
    
    valueNameMaster = request = "value";
    CreateRequest::shared_pointer createRequest = CreateRequest::create();
    pvRequest = createRequest->createRequest(request);
    builder.clear(); pvRequest->toString(&builder);
    cout << "request " << request << endl;
    cout << "pvRequest" << endl << builder;
    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "value";
    testPVScalar(valueNameMaster,valueNameCopy,pvMaster,pvCopy);
    request = "";
    valueNameMaster = "value";
    pvRequest = createRequest->createRequest(request);
    builder.clear(); pvRequest->toString(&builder);
    cout << "request " << request << endl << "pvRequest" << endl << builder << endl;
    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "value";
    testPVScalar(valueNameMaster,valueNameCopy,pvMaster,pvCopy);
    request = "alarm,timeStamp,value";
    valueNameMaster = "value";
    pvRequest = createRequest->createRequest(request);
    builder.clear(); pvRequest->toString(&builder);
    cout << "request " << request << endl << "pvRequest" << endl << builder << endl;
    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "value";
    testPVScalar(valueNameMaster,valueNameCopy,pvMaster,pvCopy);
}

static void arrayTest()
{
    cout << endl << endl << "****arrayTest****" << endl;
    PVStructurePtr pvMaster;
    String request;
    PVStructurePtr pvRequest;
    PVFieldPtr pvMasterField;
    PVCopyPtr pvCopy;
    String builder;
    String valueNameMaster;
    String valueNameCopy;

    CreateRequest::shared_pointer createRequest = CreateRequest::create();
    StandardPVFieldPtr standardPVField = getStandardPVField();
    pvMaster = standardPVField->scalarArray(pvDouble,"alarm,timeStamp");
    valueNameMaster = request = "value";
    pvRequest = createRequest->createRequest(request);
    builder.clear(); pvRequest->toString(&builder);
    cout << "request " << request << endl;
    cout << "pvRequest" << endl << builder;
    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "value";
    testPVScalarArray(pvDouble,valueNameMaster,valueNameCopy,pvMaster,pvCopy);
    request = "";
    valueNameMaster = "value";
    pvRequest = createRequest->createRequest(request);
    builder.clear(); pvRequest->toString(&builder);
    cout << "request " << request << endl << "pvRequest" << endl << builder << endl;
    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "value";
    testPVScalarArray(pvDouble,valueNameMaster,valueNameCopy,pvMaster,pvCopy);
    request = "alarm,timeStamp,value";
    valueNameMaster = "value";
    pvRequest = createRequest->createRequest(request);
    builder.clear(); pvRequest->toString(&builder);
    cout << "request " << request << endl << "pvRequest" << endl << builder << endl;
    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "value";
    testPVScalarArray(pvDouble,valueNameMaster,valueNameCopy,pvMaster,pvCopy);
}

#ifdef XXXXXX
static void powerSupplyTest()
{
    cout << endl << endl << "****powerSupplyTest****" << endl;
    RequesterPtr requester(new MyRequester("exampleTest"));
    PowerSupplyPtr pvMaster;
    String request;
    PVStructurePtr pvRequest;
    PVFieldPtr pvMasterField;
    PVCopyPtr pvCopy;
    String builder;
    String valueNameMaster;
    String valueNameCopy;

    CreateRequest::shared_pointer createRequest = CreateRequest::create();
    PVStructurePtr pv = createPowerSupply();
    pvMaster = PowerSupply::create("powerSupply",pv);
    valueNameMaster = request = "power.value";
    pvRequest = createRequest->createRequest(request);
    builder.clear(); pvRequest->toString(&builder);
    cout << "request " << request << endl;
    cout << "pvRequest" << endl << builder;
    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "power.value";
    testPVScalar(valueNameMaster,valueNameCopy,pvMaster,pvCopy);
    request = "";
    valueNameMaster = "power.value";
    pvRequest = createRequest->createRequest(request);
    builder.clear(); pvRequest->toString(&builder);
    cout << "request " << request << endl << "pvRequest" << endl << builder << endl;
    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "power.value";
    testPVScalar(valueNameMaster,valueNameCopy,pvMaster,pvCopy);
    request = "alarm,timeStamp,voltage.value,power.value,current.value";
    valueNameMaster = "power.value";
    pvRequest = createRequest->createRequest(request);
    builder.clear(); pvRequest->toString(&builder);
    cout << "request " << request << endl << "pvRequest" << endl << builder << endl;
    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "power.value";
    testPVScalar(valueNameMaster,valueNameCopy,pvMaster,pvCopy);
    request = "alarm,timeStamp,voltage{value,alarm},power{value,alarm,display},current.value";
    valueNameMaster = "power.value";
    pvRequest = createRequest->createRequest(request);
    builder.clear(); pvRequest->toString(&builder);
    cout << "request " << request << endl << "pvRequest" << endl << builder << endl;
    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "power.value";
    testPVScalar(valueNameMaster,valueNameCopy,pvMaster,pvCopy);
    pvMaster->destroy();
}
#endif

MAIN(testPVCopy)
{
    testPlan(0);
    scalarTest();
    arrayTest();
//    powerSupplyTest();
    return testDone();
}

