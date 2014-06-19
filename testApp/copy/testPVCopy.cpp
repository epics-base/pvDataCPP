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

static bool debug = true;

static void testPVScalar(
    string const & valueNameMaster,
    string const & valueNameCopy,
    PVStructurePtr const & pvMaster,
    PVCopyPtr const & pvCopy)
{
    PVStructurePtr pvStructureCopy;
    PVFieldPtr pvField;
    PVScalarPtr pvValueMaster;
    PVScalarPtr pvValueCopy;
    BitSetPtr bitSet;
    size_t offset;
    ConvertPtr convert = getConvert();

    pvField = pvMaster->getSubField(valueNameMaster);
    pvValueMaster = static_pointer_cast<PVScalar>(pvField);
    convert->fromDouble(pvValueMaster,.04);
    StructureConstPtr structure = pvCopy->getStructure();
    if(debug) { cout << "structure from copy" << endl << *structure << endl; }
    pvStructureCopy = pvCopy->createPVStructure();
    pvField = pvStructureCopy->getSubField(valueNameCopy);
    pvValueCopy = static_pointer_cast<PVScalar>(pvField);
    bitSet = BitSetPtr(new BitSet(pvStructureCopy->getNumberFields()));
    pvCopy->initCopy(pvStructureCopy, bitSet);
    if(debug) { cout << "after initCopy pvValueCopy " << convert->toDouble(pvValueCopy); }
    if(debug) { cout << endl; }
    convert->fromDouble(pvValueMaster,.06);
    testOk1(convert->toDouble(pvValueCopy)==.04);
    pvCopy->updateCopySetBitSet(pvStructureCopy,bitSet);
    testOk1(convert->toDouble(pvValueCopy)==.06);
    testOk1(bitSet->get(pvValueCopy->getFieldOffset()));
    if(debug) { cout << "after put(.06) pvValueCopy " << convert->toDouble(pvValueCopy); }
    if(debug) { cout << " bitSet " << *bitSet; }
    if(debug) { cout << endl; }
    offset = pvCopy->getCopyOffset(pvValueMaster);
    if(debug) { cout << "getCopyOffset() " << offset; }
    if(debug) { cout << " pvValueCopy->getOffset() " << pvValueCopy->getFieldOffset(); }
    if(debug) { cout << " pvValueMaster->getOffset() " << pvValueMaster->getFieldOffset(); }
    if(debug) { cout << " bitSet " << *bitSet; }
    if(debug) { cout << endl; }
    bitSet->clear();
    convert->fromDouble(pvValueMaster,1.0);
    if(debug) { cout << "before updateCopyFromBitSet"; }
    if(debug) { cout << " masterValue " << convert->toDouble(pvValueMaster); }
    if(debug) { cout << " copyValue " << convert->toDouble(pvValueCopy); }
    if(debug) { cout << " bitSet " << *bitSet; }
    if(debug) { cout << endl; }
    bitSet->set(0);
    testOk1(convert->toDouble(pvValueCopy)==0.06);
    pvCopy->updateCopyFromBitSet(pvStructureCopy,bitSet);
    testOk1(convert->toDouble(pvValueCopy)==1.0);
    if(debug) { cout << "after updateCopyFromBitSet"; }
    if(debug) { cout << " masterValue " << convert->toDouble(pvValueMaster); }
    if(debug) { cout << " copyValue " << convert->toDouble(pvValueCopy); }
    if(debug) { cout << " bitSet " << *bitSet; }
    if(debug) { cout << endl; }
    convert->fromDouble(pvValueCopy,2.0);
    bitSet->set(0);
    if(debug) { cout << "before updateMaster"; }
    if(debug) { cout << " masterValue " << convert->toDouble(pvValueMaster); }
    if(debug) { cout << " copyValue " << convert->toDouble(pvValueCopy); }
    if(debug) { cout << " bitSet " << *bitSet; }
    if(debug) { cout << endl; }
    testOk1(convert->toDouble(pvValueMaster)==1.0);
    pvCopy->updateMaster(pvStructureCopy,bitSet);
    testOk1(convert->toDouble(pvValueMaster)==2.0);
    if(debug) { cout << "after updateMaster"; }
    if(debug) { cout << " masterValue " << convert->toDouble(pvValueMaster); }
    if(debug) { cout << " copyValue " << convert->toDouble(pvValueCopy); }
    if(debug) { cout << " bitSet " << *bitSet; }
    if(debug) { cout << endl; }
}

static void testPVScalarArray(
    ScalarType scalarType,
    string const & valueNameMaster,
    string const & valueNameCopy,
    PVStructurePtr const & pvMaster,
    PVCopyPtr const & pvCopy)
{
    PVStructurePtr pvStructureCopy;
    PVScalarArrayPtr pvValueMaster;
    PVScalarArrayPtr pvValueCopy;
    BitSetPtr bitSet;
    size_t offset;
    size_t n = 5;
    shared_vector<double> values(n);
    shared_vector<const double> cvalues;

    pvValueMaster = pvMaster->getScalarArrayField(valueNameMaster,scalarType);
    for(size_t i=0; i<n; i++) values[i] = i;
    const shared_vector<const double> xxx(freeze(values));
    pvValueMaster->putFrom(xxx);
    StructureConstPtr structure = pvCopy->getStructure();
    if(debug) { cout << "structure from copy" << endl << *structure << endl;}
    pvStructureCopy = pvCopy->createPVStructure();
    pvValueCopy = pvStructureCopy->getScalarArrayField(valueNameCopy,scalarType);
    bitSet = BitSetPtr(new BitSet(pvStructureCopy->getNumberFields()));
    pvCopy->initCopy(pvStructureCopy, bitSet);
    if(debug) { cout << "after initCopy pvValueCopy " << *pvValueCopy << endl; }
    if(debug) { cout << endl; }
    values.resize(n);
    for(size_t i=0; i<n; i++) values[i] = i + .06;
    const shared_vector<const double> yyy(freeze(values));
    pvValueMaster->putFrom(yyy);
    pvValueCopy->getAs(cvalues);
    testOk1(cvalues[0]==0.0);
    pvCopy->updateCopySetBitSet(pvStructureCopy,bitSet);
    pvValueCopy->getAs(cvalues);
    testOk1(cvalues[0]==0.06);
    if(debug) { cout << "after put(i+ .06) pvValueCopy " << *pvValueCopy << endl; }
    if(debug) { cout << " bitSet " << *bitSet; }
    if(debug) { cout << endl; }
    offset = pvCopy->getCopyOffset(pvValueMaster);
    if(debug) { cout << "getCopyOffset() " << offset; }
    if(debug) { cout << " pvValueCopy->getOffset() " << pvValueCopy->getFieldOffset(); }
    if(debug) { cout << " pvValueMaster->getOffset() " << pvValueMaster->getFieldOffset(); }
    if(debug) { cout << " bitSet " << *bitSet; }
    if(debug) { cout << endl; }
    bitSet->clear();
    values.resize(n);
    for(size_t i=0; i<n; i++) values[i] = i + 1.0;
    const shared_vector<const double> zzz(freeze(values));
    pvValueMaster->putFrom(zzz);
    if(debug) { cout << "before updateCopyFromBitSet"; }
    if(debug) { cout << " masterValue " << *pvValueMaster << endl; }
    if(debug) { cout << " copyValue " << *pvValueCopy << endl; }
    if(debug) { cout << " bitSet " << *bitSet; }
    if(debug) { cout << endl; }
    bitSet->set(0);
    pvValueCopy->getAs(cvalues);
    testOk1(cvalues[0]==0.06);
    pvCopy->updateCopyFromBitSet(pvStructureCopy,bitSet);
    pvValueCopy->getAs(cvalues);
    testOk1(cvalues[0]==1.0);
    if(debug) { cout << "after updateCopyFromBitSet"; }
    if(debug) { cout << " masterValue " << *pvValueMaster << endl; }
    if(debug) { cout << " copyValue " << *pvValueCopy << endl; }
    if(debug) { cout << " bitSet " << *bitSet; }
    if(debug) { cout << endl; }
    values.resize(n);
    for(size_t i=0; i<n; i++) values[i] = i + 2.0;
    const shared_vector<const double> ttt(freeze(values));
    pvValueMaster->putFrom(ttt);
    bitSet->set(0);
    if(debug) { cout << "before updateMaster"; }
    if(debug) { cout << " masterValue " << *pvValueMaster << endl; }
    if(debug) { cout << " copyValue " << *pvValueCopy << endl; }
    if(debug) { cout << " bitSet " << *bitSet; }
    if(debug) { cout << endl; }
    pvValueMaster->getAs(cvalues);
    testOk1(cvalues[0]==2.0);
    pvCopy->updateMaster(pvStructureCopy,bitSet);
    pvValueMaster->getAs(cvalues);
    testOk1(cvalues[0]==1.0);
    if(debug) { cout << "before updateMaster"; }
    if(debug) { cout << " masterValue " << *pvValueMaster << endl; }
    if(debug) { cout << " copyValue " << *pvValueCopy << endl; }
    if(debug) { cout << " bitSet " << *bitSet; }
    if(debug) { cout << endl; }
}
    
static void scalarTest()
{
    if(debug) { cout << endl << endl << "****scalarTest****" << endl; }
    PVStructurePtr pvMaster;
    string request;
    PVStructurePtr pvRequest;
    PVFieldPtr pvMasterField;
    PVCopyPtr pvCopy;
    string valueNameMaster;
    string valueNameCopy;

    StandardPVFieldPtr standardPVField = getStandardPVField();
    pvMaster = standardPVField->scalar(pvDouble,"alarm,timeStamp,display");
    
    valueNameMaster = request = "value";
    CreateRequest::shared_pointer createRequest = CreateRequest::create();
    pvRequest = createRequest->createRequest(request);
    if(debug) { cout << "request " << request << endl; }
    if(debug) { cout << "pvRequest\n" << pvRequest->dumpValue(cout) << endl; }
    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "value";
    testPVScalar(valueNameMaster,valueNameCopy,pvMaster,pvCopy);
    request = "";
    valueNameMaster = "value";
    pvRequest = createRequest->createRequest(request);
    if(debug) { cout << "request " << request << endl; }
    if(debug) { cout << "pvRequest\n" << pvRequest->dumpValue(cout) << endl; }
    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "value";
    testPVScalar(valueNameMaster,valueNameCopy,pvMaster,pvCopy);
    request = "alarm,timeStamp,value";
    valueNameMaster = "value";
    pvRequest = createRequest->createRequest(request);
    if(debug) { cout << "request " << request << endl; }
    if(debug) { cout << "pvRequest\n" << pvRequest->dumpValue(cout) << endl; }
    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "value";
    testPVScalar(valueNameMaster,valueNameCopy,pvMaster,pvCopy);
}

static void arrayTest()
{
    if(debug) { cout << endl << endl << "****arrayTest****" << endl; }
    PVStructurePtr pvMaster;
    string request;
    PVStructurePtr pvRequest;
    PVFieldPtr pvMasterField;
    PVCopyPtr pvCopy;
    string valueNameMaster;
    string valueNameCopy;

    CreateRequest::shared_pointer createRequest = CreateRequest::create();
    StandardPVFieldPtr standardPVField = getStandardPVField();
    pvMaster = standardPVField->scalarArray(pvDouble,"alarm,timeStamp");
    valueNameMaster = request = "value";
    pvRequest = createRequest->createRequest(request);
    if(debug) { cout << "request " << request << endl; }
    if(debug) { cout << "pvRequest\n" << pvRequest->dumpValue(cout) << endl; }
    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "value";
    testPVScalarArray(pvDouble,valueNameMaster,valueNameCopy,pvMaster,pvCopy);
    request = "";
    valueNameMaster = "value";
    pvRequest = createRequest->createRequest(request);
    if(debug) { cout << "request " << request << endl; }
    if(debug) { cout << "pvRequest\n" << pvRequest->dumpValue(cout) << endl; }
    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "value";
    testPVScalarArray(pvDouble,valueNameMaster,valueNameCopy,pvMaster,pvCopy);
    request = "alarm,timeStamp,value";
    valueNameMaster = "value";
    pvRequest = createRequest->createRequest(request);
    if(debug) { cout << "request " << request << endl; }
    if(debug) { cout << "pvRequest\n" << pvRequest->dumpValue(cout) << endl; }
    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "value";
    testPVScalarArray(pvDouble,valueNameMaster,valueNameCopy,pvMaster,pvCopy);
}

static PVStructurePtr createPowerSupply()
{
    FieldCreatePtr fieldCreate = getFieldCreate();
    StandardFieldPtr standardField = getStandardField();
    PVDataCreatePtr pvDataCreate = getPVDataCreate();

    size_t nfields = 5;
    StringArray names;
    names.reserve(nfields);
    FieldConstPtrArray powerSupply;
    powerSupply.reserve(nfields);
    names.push_back("alarm");
    powerSupply.push_back(standardField->alarm());
    names.push_back("timeStamp");
    powerSupply.push_back(standardField->timeStamp());
    string properties("alarm,display");
    names.push_back("voltage");
    powerSupply.push_back(standardField->scalar(pvDouble,properties));
    names.push_back("power");
    powerSupply.push_back(standardField->scalar(pvDouble,properties));
    names.push_back("current");
    powerSupply.push_back(standardField->scalar(pvDouble,properties));
    return pvDataCreate->createPVStructure(
            fieldCreate->createStructure(names,powerSupply));
}



static void powerSupplyTest()
{
    if(debug) { cout << endl << endl << "****powerSupplyTest****" << endl; }
    PVStructurePtr pvMaster;
    string request;
    PVStructurePtr pvRequest;
    PVFieldPtr pvMasterField;
    PVCopyPtr pvCopy;
    string builder;
    string valueNameMaster;
    string valueNameCopy;

    CreateRequest::shared_pointer createRequest = CreateRequest::create();
    pvMaster = createPowerSupply();
    valueNameMaster = request = "power.value";
    pvRequest = createRequest->createRequest(request);
    if(debug) { cout << "request " << request << endl; }
    if(debug) { cout << "pvRequest\n" << pvRequest->dumpValue(cout) << endl; }
    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "power.value";
    testPVScalar(valueNameMaster,valueNameCopy,pvMaster,pvCopy);
    request = "";
    valueNameMaster = "power.value";
    pvRequest = createRequest->createRequest(request);
    if(debug) { cout << "request " << request << endl; }
    if(debug) { cout << "pvRequest\n" << pvRequest->dumpValue(cout) << endl; }
    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "power.value";
    testPVScalar(valueNameMaster,valueNameCopy,pvMaster,pvCopy);
    request = "alarm,timeStamp,voltage.value,power.value,current.value";
    valueNameMaster = "power.value";
    pvRequest = createRequest->createRequest(request);
    if(debug) { cout << "request " << request << endl; }
    if(debug) { cout << "pvRequest\n" << pvRequest->dumpValue(cout) << endl; }
    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "power.value";
    testPVScalar(valueNameMaster,valueNameCopy,pvMaster,pvCopy);
    request = "alarm,timeStamp,voltage{value,alarm},power{value,alarm,display},current.value";
    valueNameMaster = "power.value";
    pvRequest = createRequest->createRequest(request);
    if(debug) { cout << "request " << request << endl; }
    if(debug) { cout << "pvRequest\n" << pvRequest->dumpValue(cout) << endl; }
    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "power.value";
    testPVScalar(valueNameMaster,valueNameCopy,pvMaster,pvCopy);
}

MAIN(testPVCopy)
{
    testPlan(67);
    scalarTest();
    arrayTest();
    powerSupplyTest();
    return testDone();
}

