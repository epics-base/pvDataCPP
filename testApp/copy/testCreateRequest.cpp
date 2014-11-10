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
using std::string;
using std::cout;
using std::endl;

static bool debug = false;

static void testCreateRequestInternal() {
    printf("testCreateRequest... \n");
    CreateRequest::shared_pointer  createRequest = CreateRequest::create();
    PVStringPtr pvString;
    string sval;

    string out;
    string request = "";
    if(debug) { cout  << "request " << request <<endl;}
    PVStructurePtr pvRequest = createRequest->createRequest(request);
    if(pvRequest.get()==NULL) { cout<< createRequest->getMessage() << endl;}
    if(debug) { cout << *pvRequest << endl;}
    testOk1(pvRequest.get()!=NULL);
    testOk1(pvRequest->getStructure()->getNumberFields()==0);
    testPass("request %s",request.c_str());

    request = "record[]field()getField()putField()";
    if(debug) { cout  << "request " << request <<endl;}
    pvRequest = createRequest->createRequest(request);
    if(pvRequest.get()==NULL) { cout<< createRequest->getMessage() << endl;}
    if(debug) { cout << *pvRequest << endl;}
    testOk1(pvRequest.get()!=NULL);
    testOk1(pvRequest->getSubField("field").get()!=NULL);
    testOk1(pvRequest->getSubField("putField").get()!=NULL);
    testOk1(pvRequest->getSubField("getField").get()!=NULL);
    testPass("request %s",request.c_str());

    request = "record[a=b,x=y]field(a) putField(a),getField(a)";
    if(debug) { cout  << "request " << request <<endl;}
    pvRequest = createRequest->createRequest(request);
    if(pvRequest.get()==NULL) { cout<< createRequest->getMessage() << endl;}
    if(debug) { cout << *pvRequest << endl;}
    testOk1(pvRequest.get()!=NULL);
    pvString = pvRequest->getSubField<PVString>("record._options.a");
    sval = pvString->get();
    testOk(sval.compare("b")==0,"record.a = b");
    pvString = pvRequest->getSubField<PVString>("record._options.x");
    sval = pvString->get();
    testOk(sval.compare("y")==0,"record.x = y");
    testOk1(pvRequest->getSubField("field.a").get()!=NULL);
    testOk1(pvRequest->getSubField("putField.a").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.a").get()!=NULL);
    testPass("request %s",request.c_str());

    request = "field(a.b[x=y])";
    if(debug) { cout  << "request " << request <<endl;}
    pvRequest = createRequest->createRequest(request);
    if(pvRequest.get()==NULL) { cout<< createRequest->getMessage() << endl;}
    if(debug) { cout << *pvRequest << endl;}
    testOk1(pvRequest.get()!=NULL);
    pvString = pvRequest->getSubField<PVString>("field.a.b._options.x");
    sval = pvString->get();
    testOk(sval.compare("y")==0,"field.a.b._options.x = y");
    testPass("request %s",request.c_str());

    request = "field(a.b{c.d})";
    if(debug) { cout  << "request " << request <<endl;}
    pvRequest = createRequest->createRequest(request);
    if(pvRequest.get()==NULL) { cout<< createRequest->getMessage() << endl;}
    if(debug) { cout << *pvRequest << endl;}
    testOk1(pvRequest.get()!=NULL);
    testOk1(pvRequest->getSubField("field.a.b.c.d").get()!=NULL);
    testPass("request %s",request.c_str());

    request = "field(a.b[x=y]{c.d})";
    if(debug) { cout  << "request " << request <<endl;}
    pvRequest = createRequest->createRequest(request);
    if(pvRequest.get()==NULL) { cout<< createRequest->getMessage() << endl;}
    if(debug) { cout << *pvRequest << endl;}
    testOk1(pvRequest.get()!=NULL);
    pvString = pvRequest->getSubField<PVString>("field.a.b._options.x");
    sval = pvString->get();
    testOk(sval.compare("y")==0,"field.a.b._options.x = y");
    testOk1(pvRequest->getSubField("field.a.b.c.d").get()!=NULL);
    testPass("request %s",request.c_str());

    request = "field(a.b[x=y]{c.d[x=y]})";
    if(debug) { cout  << "request " << request <<endl;}
    pvRequest = createRequest->createRequest(request);
    if(pvRequest.get()==NULL) { cout<< createRequest->getMessage() << endl;}
    if(debug) { cout << *pvRequest << endl;}
    testOk1(pvRequest.get()!=NULL);
    pvString = pvRequest->getSubField<PVString>("field.a.b._options.x");
    sval = pvString->get();
    testOk(sval.compare("y")==0,"field.a.b._options.x = y");
    pvString = pvRequest->getSubField<PVString>("field.a.b.c.d._options.x");
    sval = pvString->get();
    testOk(sval.compare("y")==0,"field.a.b.c.d._options.x = y");
    testPass("request %s",request.c_str());

    request = "record[a=b,c=d] field(a.a[a=b]{a.a[a=b]},b.a[a=b]{a,b})";
    if(debug) { cout  << "request " << request <<endl;}
    pvRequest = createRequest->createRequest(request);
    if(pvRequest.get()==NULL) { cout<< createRequest->getMessage() << endl;}
    if(debug) { cout << *pvRequest << endl;}
    testOk1(pvRequest.get()!=NULL);
    pvString = pvRequest->getSubField<PVString>("field.a.a._options.a");
    sval = pvString->get();
    testOk(sval.compare("b")==0,"field.a.a._options.a = b");
    pvString = pvRequest->getSubField<PVString>("field.a.a.a.a._options.a");
    sval = pvString->get();
    testOk(sval.compare("b")==0,"field.a.a.a.a._options.a = b");
    pvString = pvRequest->getSubField<PVString>("field.b.a._options.a");
    sval = pvString->get();
    testOk(sval.compare("b")==0,"field.b.a._options.a = b");
    testOk1(pvRequest->getSubField("field.b.a.a").get()!=NULL);
    testOk1(pvRequest->getSubField("field.b.a.b").get()!=NULL);
    testPass("request %s",request.c_str());


    request = "alarm,timeStamp,power.value";
    if(debug) { cout  << "request " << request <<endl;}
    pvRequest = createRequest->createRequest(request);
    if(pvRequest.get()==NULL) { cout<< createRequest->getMessage() << endl;}
    if(debug) { cout << *pvRequest << endl;}
    testOk1(pvRequest.get()!=NULL);
    testOk1(pvRequest->getSubField("field.alarm").get()!=NULL);
    testOk1(pvRequest->getSubField("field.timeStamp").get()!=NULL);
    testOk1(pvRequest->getSubField("field.power.value").get()!=NULL);
    testPass("request %s",request.c_str());

    request = "record[process=true]field(alarm,timeStamp,power.value)";
    if(debug) { cout  << "request " << request <<endl;}
    pvRequest = createRequest->createRequest(request);
    if(pvRequest.get()==NULL) { cout<< createRequest->getMessage() << endl;}
    if(debug) { cout << *pvRequest << endl;}
    testOk1(pvRequest.get()!=NULL);
    pvString = pvRequest->getSubField<PVString>("record._options.process");
    sval = pvString->get();
    testOk(sval.compare("true")==0,"record._options.process = true");
    testOk1(pvRequest->getSubField("field.alarm").get()!=NULL);
    testOk1(pvRequest->getSubField("field.timeStamp").get()!=NULL);
    testOk1(pvRequest->getSubField("field.power.value").get()!=NULL);
    testPass("request %s",request.c_str());

    request = "record[process=true]field(alarm,timeStamp[algorithm=onChange,causeMonitor=false],power{value,alarm})";
    if(debug) { cout  << "request " << request <<endl;}
    pvRequest = createRequest->createRequest(request);
    if(pvRequest.get()==NULL) { cout<< createRequest->getMessage() << endl;}
    if(debug) { cout << *pvRequest << endl;}
    testOk1(pvRequest.get()!=NULL);
    pvString = pvRequest->getSubField<PVString>("record._options.process");
    sval = pvString->get();
    testOk(sval.compare("true")==0,"record._options.process = true");
    testOk1(pvRequest->getSubField("field.alarm").get()!=NULL);
    testOk1(pvRequest->getSubField("field.timeStamp").get()!=NULL);
    pvString = pvRequest->getSubField<PVString>("field.timeStamp._options.algorithm");
    sval = pvString->get();
    testOk(sval.compare("onChange")==0,"field.timeStamp._options.algorithm = onChange");
    pvString = pvRequest->getSubField<PVString>("field.timeStamp._options.causeMonitor");
    sval = pvString->get();
    testOk(sval.compare("false")==0,"field.timeStamp._options.causeMonitor = false");
    testOk1(pvRequest->getSubField("field.power.value").get()!=NULL);
    testOk1(pvRequest->getSubField("field.power.alarm").get()!=NULL);
    testPass("request %s",request.c_str());

    request = "record[int=2,float=3.14159]field(alarm,timeStamp[shareData=true],power.value)";
    if(debug) { cout  << "request " << request <<endl;}
    pvRequest = createRequest->createRequest(request);
    if(pvRequest.get()==NULL) { cout<< createRequest->getMessage() << endl;}
    if(debug) { cout << *pvRequest << endl;}
    testOk1(pvRequest.get()!=NULL);
    pvString = pvRequest->getSubField<PVString>("record._options.int");
    sval = pvString->get();
    testOk(sval.compare("2")==0,"record._options.int = 2");
    pvString = pvRequest->getSubField<PVString>("record._options.float");
    sval = pvString->get();
    testOk(sval.compare("3.14159")==0,"record._options.float = 3.14159");
    testOk1(pvRequest->getSubField("field.alarm").get()!=NULL);
    pvString = pvRequest->getSubField<PVString>("field.timeStamp._options.shareData");
    sval = pvString->get();
    testOk(sval.compare("true")==0,"field.timeStamp._options.shareData = true");
    testOk1(pvRequest->getSubField("field.power.value").get()!=NULL);
    testPass("request %s",request.c_str());

    request = string("record[process=true,xxx=yyy]")
        + "putField(power.value)"
    	+ "getField(alarm,timeStamp,power{value,alarm},"
    	+ "current{value,alarm},voltage{value,alarm})";
    if(debug) { cout  << "request " << request <<endl;}
    pvRequest = createRequest->createRequest(request);
    if(pvRequest.get()==NULL) { cout<< createRequest->getMessage() << endl;}
    if(debug) { cout << *pvRequest << endl;}
    testOk1(pvRequest.get()!=NULL);
    testOk1(pvRequest->getSubField("putField.power.value").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.alarm").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.timeStamp").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.power.value").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.power.alarm").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.current.value").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.current.alarm").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.voltage.value").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.voltage.alarm").get()!=NULL);
    testPass("request %s",request.c_str());

    request = string("field(alarm,timeStamp,supply{")
    	+ "0{voltage.value,current.value,power.value},"
        + "1{voltage.value,current.value,power.value}"
        + "})";
    if(debug) { cout  << "request " << request <<endl;}
    pvRequest = createRequest->createRequest(request);
    if(pvRequest.get()==NULL) { cout<< createRequest->getMessage() << endl;}
    if(debug) { cout << *pvRequest << endl;}
    testOk1(pvRequest.get()!=NULL);
    testOk1(pvRequest->getSubField("field.alarm").get()!=NULL);
    testOk1(pvRequest->getSubField("field.timeStamp").get()!=NULL);
    testOk1(pvRequest->getSubField("field.supply.0.voltage.value").get()!=NULL);
    testOk1(pvRequest->getSubField("field.supply.0.current.value").get()!=NULL);
    testOk1(pvRequest->getSubField("field.supply.0.power.value").get()!=NULL);
    testOk1(pvRequest->getSubField("field.supply.1.voltage.value").get()!=NULL);
    testOk1(pvRequest->getSubField("field.supply.1.current.value").get()!=NULL);
    testOk1(pvRequest->getSubField("field.supply.1.power.value").get()!=NULL);
    testPass("request %s",request.c_str());

    request = string("record[process=true,xxx=yyy]")
    	+ "putField(power.value)"
    	+ "getField(alarm,timeStamp,power{value,alarm},"
    	+ "current{value,alarm},voltage{value,alarm},"
    	+ "ps0{alarm,timeStamp,power{value,alarm},current{value,alarm},voltage{value,alarm}},"
        + "ps1{alarm,timeStamp,power{value,alarm},current{value,alarm},voltage{value,alarm}}"
        + ")";
    if(debug) { cout  << "request " << request <<endl;}
    pvRequest = createRequest->createRequest(request);
    if(pvRequest.get()==NULL) { cout<< createRequest->getMessage() << endl;}
    if(debug) { cout << *pvRequest << endl;}
    testOk1(pvRequest.get()!=NULL);
    testOk1(pvRequest->getSubField("putField.power.value").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.alarm").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.timeStamp").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.power.value").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.power.alarm").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.current.value").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.current.alarm").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.voltage.value").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.voltage.alarm").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.ps0.alarm").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.ps0.timeStamp").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.ps0.power.value").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.ps0.power.alarm").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.ps0.current.value").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.ps0.current.alarm").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.ps0.voltage.value").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.ps0.voltage.alarm").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.ps1.alarm").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.ps1.timeStamp").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.ps1.power.value").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.ps1.power.alarm").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.ps1.current.value").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.ps1.current.alarm").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.ps1.voltage.value").get()!=NULL);
    testOk1(pvRequest->getSubField("getField.ps1.voltage.alarm").get()!=NULL);
    testPass("request %s",request.c_str());

    request = "a{b{c{d}}}";
    if(debug) { cout  << "request " << request <<endl;}
    pvRequest = createRequest->createRequest(request);
    if(pvRequest.get()==NULL) { cout<< createRequest->getMessage() << endl;}
    if(debug) { cout << *pvRequest << endl;}
    testOk1(pvRequest.get()!=NULL);
    testOk1(pvRequest->getSubField("field.a.b.c.d").get()!=NULL);
    testPass("request %s",request.c_str());

    request = string("record[process=true,xxx=yyy]")
        + "putField(power.value)"
        + "getField(alarm,timeStamp,power{value,alarm},"
        + "current{value,alarm},voltage{value,alarm},"
        + "ps0{alarm,timeStamp,power{value,alarm},current{value,alarm},voltage{value,alarm}},"
        + "ps1{alarm,timeStamp,power{value,alarm},current{value,alarm},voltage{value,alarm}"
        + ")";
    if(debug) { cout  << "request " << request <<endl;}
    cout << endl << "Error Expected for next call!!" << endl;
    pvRequest = createRequest->createRequest(request);
    cout << "reason " << createRequest->getMessage() << endl;
    testPass("request %s",request.c_str());

    request = "record[process=true,power.value";
    if(debug) { cout  << "request " << request <<endl;}
    cout << endl << "Error Expected for next call!!" << endl;
    pvRequest = createRequest->createRequest(request);
    cout << "reason " << createRequest->getMessage() << endl;
    testOk1(pvRequest.get()==NULL);
    testPass("request %s",request.c_str());
}

MAIN(testCreateRequest)
{
    testPlan(117);
    testCreateRequestInternal();
    return testDone();
}


