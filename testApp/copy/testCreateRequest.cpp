/* testCreateRequest.cpp */
/* Author:  Matej Sekoranja Date: 2010.12.27 */

#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <iostream>

#include <epicsUnitTest.h>
#include <testMain.h>

#include <pv/createRequest.h>

using namespace epics::pvData;
using std::cout;
using std::endl;

static bool debug = false;

void testCreateRequest() {
    printf("testCreateRequest... \n");
    CreateRequest::shared_pointer  createRequest = CreateRequest::create();

    String out;
    String request = "";
    if(debug) { cout  << "request " << request <<endl;}
    PVStructurePtr pvRequest = createRequest->createRequest(request);
    if(debug) { cout<< createRequest->getMessage() << endl;}
    testOk1(pvRequest!=NULL);
    if(debug) { cout << pvRequest->dumpValue(cout) << endl;}
    testOk1(pvRequest->getStructure()->getNumberFields()==0);
    testPass("request %s",request.c_str());


    request = "record[a=b,x=y]field(a) putField(a),getField(a)";
    pvRequest = createRequest->createRequest(request);
    if(pvRequest==NULL) {
        cout << createRequest->getMessage() << endl;
    }
    if(debug) { cout  << "request " << request <<endl;}
    testOk1(pvRequest!=NULL);
    if(debug) { cout << pvRequest->dumpValue(cout) << endl;}
    PVStringPtr pvString = pvRequest->getSubField<PVString>("record.a");
    String sval = pvString->get();
    testOk(sval.compare("b")==0,"record.a = b");
    pvString = pvRequest->getSubField<PVString>("record.x");
    sval = pvString->get();
    testOk(sval.compare("y")==0,"record.x = y");
    testOk1(pvRequest->getSubField("field.a")!=NULL);
    testOk1(pvRequest->getSubField("putField.a")!=NULL);
    testOk1(pvRequest->getSubField("getField.a")!=NULL);
    testPass("request %s",request.c_str());

    request = "field(a.b[x=y])";
    pvRequest = createRequest->createRequest(request);
    if(pvRequest==NULL) {
        cout << createRequest->getMessage() << endl;
    }
    if(debug) { cout  << "request " << request <<endl;}
    testOk1(pvRequest!=NULL);
    if(debug) { cout << pvRequest->dumpValue(cout) << endl;}
    pvString = pvRequest->getSubField<PVString>("field.a.b._options.x");
    sval = pvString->get();
    testOk(sval.compare("y")==0,"field.a.b._options.x = y");
    testPass("request %s",request.c_str());

    request = "field(a.b{c.d})";
    pvRequest = createRequest->createRequest(request);
    if(debug) { cout  << "request " << request <<endl;}
    if(pvRequest==NULL) {
        cout << createRequest->getMessage() << endl;
    }
    testOk1(pvRequest!=NULL);
    testOk1(pvRequest->getSubField("field.a.b.c.d")!=NULL);
    if(debug) { cout << pvRequest->dumpValue(cout) << endl;}
    testPass("request %s",request.c_str());

    request = "field(a.b[x=y]{c.d})";
    pvRequest = createRequest->createRequest(request);
    if(debug) { cout  << "request " << request <<endl;}
    if(pvRequest==NULL) {
        cout << createRequest->getMessage() << endl;
    }
    testOk1(pvRequest!=NULL);
    pvString = pvRequest->getSubField<PVString>("field.a.b._options.x");
    sval = pvString->get();
    testOk(sval.compare("y")==0,"field.a.b._options.x = y");
    testOk1(pvRequest->getSubField("field.a.b.c.d")!=NULL);
    if(debug) { cout << pvRequest->dumpValue(cout) << endl;}
    testPass("request %s",request.c_str());

    request = "field(a.b[x=y]{c.d[x=y]})";
    pvRequest = createRequest->createRequest(request);
    if(debug) { cout  << "request " << request <<endl;}
    if(pvRequest==NULL) {
        cout << createRequest->getMessage() << endl;
    }
    testOk1(pvRequest!=NULL);
    pvString = pvRequest->getSubField<PVString>("field.a.b._options.x");
    sval = pvString->get();
    testOk(sval.compare("y")==0,"field.a.b._options.x = y");
    pvString = pvRequest->getSubField<PVString>("field.a.b.c.d._options.x");
    sval = pvString->get();
    testOk(sval.compare("y")==0,"field.a.b.c.d._options.x = y");
    if(debug) { cout << pvRequest->dumpValue(cout) << endl;}
    testPass("request %s",request.c_str());

    request = "record[a=b,c=d] field(a.a[a=b]{a.a[a=b]},b.a[a=b]{a,b})";
    pvRequest = createRequest->createRequest(request);
    if(debug) { cout  << "request " << request <<endl;}
    if(pvRequest==NULL) {
        cout << createRequest->getMessage() << endl;
    }
    testOk1(pvRequest!=NULL);
    pvString = pvRequest->getSubField<PVString>("field.a.a._options.a");
    sval = pvString->get();
    testOk(sval.compare("b")==0,"field.a.a._options.a = b");
    pvString = pvRequest->getSubField<PVString>("field.a.a.a.a._options.a");
    sval = pvString->get();
    testOk(sval.compare("b")==0,"field.a.a.a.a._options.a = b");
    pvString = pvRequest->getSubField<PVString>("field.b.a._options.a");
    sval = pvString->get();
    testOk(sval.compare("b")==0,"field.b.a._options.a = b");
    testOk1(pvRequest->getSubField("field.b.a.a")!=NULL);
    testOk1(pvRequest->getSubField("field.b.a.b")!=NULL);
    if(debug) { cout << pvRequest->dumpValue(cout) << endl;}
    testPass("request %s",request.c_str());


    request = "alarm,timeStamp,power.value";
    pvRequest = createRequest->createRequest(request);
    if(debug) { cout << endl << String("request") <<endl << request <<endl;}
    if(pvRequest==NULL) {
         cout << "reason " << createRequest->getMessage() << endl;
    }
    testOk1(pvRequest!=NULL);
    testOk1(pvRequest->getSubField("field.alarm")!=NULL);
    testOk1(pvRequest->getSubField("field.timeStamp")!=NULL);
    testOk1(pvRequest->getSubField("field.power.value")!=NULL);
    if(debug) { cout << pvRequest->dumpValue(cout) << endl;}
    testPass("request %s",request.c_str());

    request = "record[process=true]field(alarm,timeStamp,power.value)";
    pvRequest = createRequest->createRequest(request);
    if(debug) { cout << endl << String("request") <<endl << request <<endl;}
    if(pvRequest==NULL) {
        cout << "reason " << createRequest->getMessage() << endl;
    }
    testOk1(pvRequest!=NULL);
    pvString = pvRequest->getSubField<PVString>("record.process");
    sval = pvString->get();
    testOk(sval.compare("true")==0,"record.process = true");
    testOk1(pvRequest->getSubField("field.alarm")!=NULL);
    testOk1(pvRequest->getSubField("field.timeStamp")!=NULL);
    testOk1(pvRequest->getSubField("field.power.value")!=NULL);
    if(debug) {cout << pvRequest->dumpValue(cout) << endl;}
    testPass("request %s",request.c_str());

    request = "record[process=true]field(alarm,timeStamp[algorithm=onChange,causeMonitor=false],power{value,alarm})";
    pvRequest = createRequest->createRequest(request);
    if(debug) { cout << String("request") <<endl << request <<endl;}
    if(pvRequest==NULL) {
         cout << "reason " << createRequest->getMessage() << endl;
    }
    testOk1(pvRequest!=NULL);
    pvString = pvRequest->getSubField<PVString>("record.process");
    sval = pvString->get();
    testOk(sval.compare("true")==0,"record.process = true");
    testOk1(pvRequest->getSubField("field.alarm")!=NULL);
    testOk1(pvRequest->getSubField("field.timeStamp")!=NULL);
    pvString = pvRequest->getSubField<PVString>("field.timeStamp._options.algorithm");
    sval = pvString->get();
    testOk(sval.compare("onChange")==0,"field.timeStamp._options.algorithm = onChange");
    pvString = pvRequest->getSubField<PVString>("field.timeStamp._options.causeMonitor");
    sval = pvString->get();
    testOk(sval.compare("false")==0,"field.timeStamp._options.causeMonitor = false");
    testOk1(pvRequest->getSubField("field.power.value")!=NULL);
    testOk1(pvRequest->getSubField("field.power.alarm")!=NULL);
    if(debug) {cout << pvRequest->dumpValue(cout) << endl;}
    testPass("request %s",request.c_str());

    request = "record[int=2,float=3.14159]field(alarm,timeStamp[shareData=true],power.value)";
    pvRequest = createRequest->createRequest(request);
    if(debug) { cout << String("request") <<endl << request <<endl;}
    if(pvRequest==NULL) {
         cout << "reason " << createRequest->getMessage() << endl;
    }
    testOk1(pvRequest!=NULL);
    pvString = pvRequest->getSubField<PVString>("record.int");
    sval = pvString->get();
    testOk(sval.compare("2")==0,"record.int = 2");
    pvString = pvRequest->getSubField<PVString>("record.float");
    sval = pvString->get();
    testOk(sval.compare("3.14159")==0,"record.float = 3.14159");
    testOk1(pvRequest->getSubField("field.alarm")!=NULL);
    pvString = pvRequest->getSubField<PVString>("field.timeStamp._options.shareData");
    sval = pvString->get();
    testOk(sval.compare("true")==0,"field.timeStamp._options.shareData = true");
    testOk1(pvRequest->getSubField("field.power.value")!=NULL);
    if(debug) {cout << pvRequest->dumpValue(cout) << endl;}
    testPass("request %s",request.c_str());

    request = String("record[process=true,xxx=yyy]")
        + "putField(power.value)"
    	+ "getField(alarm,timeStamp,power{value,alarm},"
    	+ "current{value,alarm},voltage{value,alarm})";
    pvRequest = createRequest->createRequest(request);
    if(debug) { cout << String("request") <<endl << request <<endl;}
    if(pvRequest==NULL) {
        cout << "reason " << createRequest->getMessage() << endl;
    }
    testOk1(pvRequest!=NULL);
    testOk1(pvRequest->getSubField("putField.power.value")!=NULL);
    testOk1(pvRequest->getSubField("getField.alarm")!=NULL);
    testOk1(pvRequest->getSubField("getField.timeStamp")!=NULL);
    testOk1(pvRequest->getSubField("getField.power.value")!=NULL);
    testOk1(pvRequest->getSubField("getField.power.alarm")!=NULL);
    testOk1(pvRequest->getSubField("getField.current.value")!=NULL);
    testOk1(pvRequest->getSubField("getField.current.alarm")!=NULL);
    testOk1(pvRequest->getSubField("getField.voltage.value")!=NULL);
    testOk1(pvRequest->getSubField("getField.voltage.alarm")!=NULL);
    if(debug) {cout << pvRequest->dumpValue(cout) << endl;}
    testPass("request %s",request.c_str());

    request = String("field(alarm,timeStamp,supply{")
    	+ "0{voltage.value,current.value,power.value},"
        + "1{voltage.value,current.value,power.value}"
        + "})";
    pvRequest = createRequest->createRequest(request);
    if(debug) { cout << String("request") <<endl << request <<endl;}
    if(pvRequest==NULL) {
         cout << "reason " << createRequest->getMessage() << endl;
    }
    testOk1(pvRequest!=NULL);
    testOk1(pvRequest->getSubField("field.alarm")!=NULL);
    testOk1(pvRequest->getSubField("field.timeStamp")!=NULL);
    testOk1(pvRequest->getSubField("field.supply.0.voltage.value")!=NULL);
    testOk1(pvRequest->getSubField("field.supply.0.current.value")!=NULL);
    testOk1(pvRequest->getSubField("field.supply.0.power.value")!=NULL);
    testOk1(pvRequest->getSubField("field.supply.1.voltage.value")!=NULL);
    testOk1(pvRequest->getSubField("field.supply.1.current.value")!=NULL);
    testOk1(pvRequest->getSubField("field.supply.1.power.value")!=NULL);
    if(debug) {cout << pvRequest->dumpValue(cout) << endl;}
    testPass("request %s",request.c_str());

    request = String("record[process=true,xxx=yyy]")
    	+ "putField(power.value)"
    	+ "getField(alarm,timeStamp,power{value,alarm},"
    	+ "current{value,alarm},voltage{value,alarm},"
    	+ "ps0{alarm,timeStamp,power{value,alarm},current{value,alarm},voltage{value,alarm}},"
        + "ps1{alarm,timeStamp,power{value,alarm},current{value,alarm},voltage{value,alarm}}"
        + ")";
    pvRequest = createRequest->createRequest(request);
    if(debug) { cout << String("request") <<endl << request <<endl;}
    if(pvRequest==NULL) {
        cout << "reason " << createRequest->getMessage() << endl;
    }
    testOk1(pvRequest!=NULL);
    testOk1(pvRequest->getSubField("putField.power.value")!=NULL);
    testOk1(pvRequest->getSubField("getField.alarm")!=NULL);
    testOk1(pvRequest->getSubField("getField.timeStamp")!=NULL);
    testOk1(pvRequest->getSubField("getField.power.value")!=NULL);
    testOk1(pvRequest->getSubField("getField.power.alarm")!=NULL);
    testOk1(pvRequest->getSubField("getField.current.value")!=NULL);
    testOk1(pvRequest->getSubField("getField.current.alarm")!=NULL);
    testOk1(pvRequest->getSubField("getField.voltage.value")!=NULL);
    testOk1(pvRequest->getSubField("getField.voltage.alarm")!=NULL);
    testOk1(pvRequest->getSubField("getField.ps0.alarm")!=NULL);
    testOk1(pvRequest->getSubField("getField.ps0.timeStamp")!=NULL);
    testOk1(pvRequest->getSubField("getField.ps0.power.value")!=NULL);
    testOk1(pvRequest->getSubField("getField.ps0.power.alarm")!=NULL);
    testOk1(pvRequest->getSubField("getField.ps0.current.value")!=NULL);
    testOk1(pvRequest->getSubField("getField.ps0.current.alarm")!=NULL);
    testOk1(pvRequest->getSubField("getField.ps0.voltage.value")!=NULL);
    testOk1(pvRequest->getSubField("getField.ps0.voltage.alarm")!=NULL);
    testOk1(pvRequest->getSubField("getField.ps1.alarm")!=NULL);
    testOk1(pvRequest->getSubField("getField.ps1.timeStamp")!=NULL);
    testOk1(pvRequest->getSubField("getField.ps1.power.value")!=NULL);
    testOk1(pvRequest->getSubField("getField.ps1.power.alarm")!=NULL);
    testOk1(pvRequest->getSubField("getField.ps1.current.value")!=NULL);
    testOk1(pvRequest->getSubField("getField.ps1.current.alarm")!=NULL);
    testOk1(pvRequest->getSubField("getField.ps1.voltage.value")!=NULL);
    testOk1(pvRequest->getSubField("getField.ps1.voltage.alarm")!=NULL);
    if(debug) {cout << pvRequest->dumpValue(cout) << endl;}
    testPass("request %s",request.c_str());

    request = "a{b{c{d}}}";
    pvRequest = createRequest->createRequest(request);
    if(debug) { cout << String("request") <<endl << request <<endl;}
    testOk1(pvRequest!=NULL);
    testOk1(pvRequest->getSubField("field.a.b.c.d")!=NULL);
    if(debug) {cout << pvRequest->dumpValue(cout) << endl;}
    testPass("request %s",request.c_str());

    request = String("record[process=true,xxx=yyy]")
        + "putField(power.value)"
        + "getField(alarm,timeStamp,power{value,alarm},"
        + "current{value,alarm},voltage{value,alarm},"
        + "ps0{alarm,timeStamp,power{value,alarm},current{value,alarm},voltage{value,alarm}},"
        + "ps1{alarm,timeStamp,power{value,alarm},current{value,alarm},voltage{value,alarm}"
        + ")";
    if(debug) { cout << endl << "Error Expected for next call!!" << endl;}
    if(debug) { cout << String("request") <<endl << request <<endl;}
    pvRequest = createRequest->createRequest(request);
    assert(pvRequest.get()==NULL);
    if(debug) {cout << "reason " << createRequest->getMessage() << endl;}
    testPass("request %s",request.c_str());

    request = "record[process=true,power.value";
    if(debug) { cout << String("request") <<endl << request <<endl;}
    if(debug) { cout << endl << "Error Expected for next call!!" << endl;}
    pvRequest = createRequest->createRequest(request);
    assert(pvRequest.get()==NULL);
    if(debug) { cout << "reason " << createRequest->getMessage() << endl;}
    testPass("request %s",request.c_str());
}

MAIN(testCreateRequest)
{
    testPlan(111);
    testCreateRequest();
    return testDone();
}


