/*testPVCopyMain.cpp */
/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
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

#include <compilerDependencies.h>
#undef EPICS_DEPRECATED
#define EPICS_DEPRECATED

#include <pv/standardField.h>
#include <pv/standardPVField.h>
#include <pv/convert.h>
#include <pv/pvCopy.h>
#include <pv/createRequest.h>


using namespace std;
using std::tr1::static_pointer_cast;
using namespace epics::pvData;

static void testPVScalar(
    string const & valueNameMaster,
    string const & valueNameCopy,
    PVStructurePtr const & pvMaster,
    PVCopyPtr const & pvCopy)
{
    PVStructurePtr pvStructureCopy;
    PVScalarPtr pvValueMaster;
    PVScalarPtr pvValueCopy;
    BitSetPtr bitSet;
    ConvertPtr convert = getConvert();

    pvValueMaster = pvMaster->getSubField<PVScalar>(valueNameMaster);
    convert->fromDouble(pvValueMaster,.04);
    StructureConstPtr structure = pvCopy->getStructure();
    pvStructureCopy = pvCopy->createPVStructure();
    pvValueCopy = pvStructureCopy->getSubField<PVScalar>(valueNameCopy);
    bitSet = BitSetPtr(new BitSet(pvStructureCopy->getNumberFields()));
    pvCopy->initCopy(pvStructureCopy, bitSet);

    convert->fromDouble(pvValueMaster,.06);
    testOk1(convert->toDouble(pvValueCopy)==.04);
    pvCopy->updateCopySetBitSet(pvStructureCopy,bitSet);
    testOk1(convert->toDouble(pvValueCopy)==.06);
    testOk1(bitSet->get(pvValueCopy->getFieldOffset()));

    pvCopy->getCopyOffset(pvValueMaster);

    bitSet->clear();
    convert->fromDouble(pvValueMaster,1.0);

    bitSet->set(0);
    testOk1(convert->toDouble(pvValueCopy)==0.06);
    pvCopy->updateCopyFromBitSet(pvStructureCopy,bitSet);
    testOk1(convert->toDouble(pvValueCopy)==1.0);

    convert->fromDouble(pvValueCopy,2.0);
    bitSet->set(0);

    testOk1(convert->toDouble(pvValueMaster)==1.0);
    pvCopy->updateMaster(pvStructureCopy,bitSet);
    testOk1(convert->toDouble(pvValueMaster)==2.0);

}

static void testPVScalarArray(
    string const & valueNameMaster,
    string const & valueNameCopy,
    PVStructurePtr const & pvMaster,
    PVCopyPtr const & pvCopy)
{
    PVStructurePtr pvStructureCopy;
    PVScalarArrayPtr pvValueMaster;
    PVScalarArrayPtr pvValueCopy;
    BitSetPtr bitSet;
    size_t n = 5;
    shared_vector<double> values(n);
    shared_vector<const double> cvalues;

    pvValueMaster = pvMaster->getSubField<PVScalarArray>(valueNameMaster);
    for(size_t i=0; i<n; i++) values[i] = i;
    const shared_vector<const double> xxx(freeze(values));
    pvValueMaster->putFrom(xxx);
    StructureConstPtr structure = pvCopy->getStructure();

    pvStructureCopy = pvCopy->createPVStructure();
    pvValueCopy = pvStructureCopy->getSubField<PVScalarArray>(valueNameCopy);
    bitSet = BitSetPtr(new BitSet(pvStructureCopy->getNumberFields()));
    pvCopy->initCopy(pvStructureCopy, bitSet);

    values.resize(n);
    for(size_t i=0; i<n; i++) values[i] = i + .06;
    const shared_vector<const double> yyy(freeze(values));
    pvValueMaster->putFrom(yyy);
    pvValueCopy->getAs(cvalues);
    testOk1(cvalues[0]==0.0);
    pvCopy->updateCopySetBitSet(pvStructureCopy,bitSet);
    pvValueCopy->getAs(cvalues);
    testOk1(cvalues[0]==0.06);

    pvCopy->getCopyOffset(pvValueMaster);

    bitSet->clear();
    values.resize(n);
    for(size_t i=0; i<n; i++) values[i] = i + 1.0;
    const shared_vector<const double> zzz(freeze(values));
    pvValueMaster->putFrom(zzz);

    bitSet->set(0);
    pvValueCopy->getAs(cvalues);
    testOk1(cvalues[0]==0.06);
    pvCopy->updateCopyFromBitSet(pvStructureCopy,bitSet);
    pvValueCopy->getAs(cvalues);
    testOk1(cvalues[0]==1.0);

    values.resize(n);
    for(size_t i=0; i<n; i++) values[i] = i + 2.0;
    const shared_vector<const double> ttt(freeze(values));
    pvValueMaster->putFrom(ttt);
    bitSet->set(0);

    pvValueMaster->getAs(cvalues);
    testOk1(cvalues[0]==2.0);
    pvCopy->updateMaster(pvStructureCopy,bitSet);
    pvValueMaster->getAs(cvalues);
    testOk1(cvalues[0]==1.0);
}
    
static void scalarTest()
{
    testDiag("scalarTest()");
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
    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "value";
    testPVScalar(valueNameMaster,valueNameCopy,pvMaster,pvCopy);
    request = "";
    valueNameMaster = "value";
    pvRequest = createRequest->createRequest(request);

    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "value";
    testPVScalar(valueNameMaster,valueNameCopy,pvMaster,pvCopy);
    request = "alarm,timeStamp,value";
    valueNameMaster = "value";
    pvRequest = createRequest->createRequest(request);

    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "value";
    testPVScalar(valueNameMaster,valueNameCopy,pvMaster,pvCopy);
}

static void arrayTest()
{
    testDiag("arrayTest");

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

    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "value";
    testPVScalarArray(valueNameMaster,valueNameCopy,pvMaster,pvCopy);
    request = "";
    valueNameMaster = "value";
    pvRequest = createRequest->createRequest(request);

    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "value";
    testPVScalarArray(valueNameMaster,valueNameCopy,pvMaster,pvCopy);
    request = "alarm,timeStamp,value";
    valueNameMaster = "value";
    pvRequest = createRequest->createRequest(request);

    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "value";
    testPVScalarArray(valueNameMaster,valueNameCopy,pvMaster,pvCopy);
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
    testDiag("powerSupplyTest");

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

    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "power.value";
    testPVScalar(valueNameMaster,valueNameCopy,pvMaster,pvCopy);
    request = "";
    valueNameMaster = "power.value";
    pvRequest = createRequest->createRequest(request);

    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "power.value";
    testPVScalar(valueNameMaster,valueNameCopy,pvMaster,pvCopy);
    request = "alarm,timeStamp,voltage.value,power.value,current.value";
    valueNameMaster = "power.value";
    pvRequest = createRequest->createRequest(request);

    pvCopy = PVCopy::create(pvMaster,pvRequest,"");
    valueNameCopy = "power.value";
    testPVScalar(valueNameMaster,valueNameCopy,pvMaster,pvCopy);
    request = "alarm,timeStamp,voltage{value,alarm},power{value,alarm,display},current.value";
    valueNameMaster = "power.value";
    pvRequest = createRequest->createRequest(request);

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

