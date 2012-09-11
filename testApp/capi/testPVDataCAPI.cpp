/* testPVDataCAPI.cpp */
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

#include <pv/pvDataCAPI.h>
#include <pv/standardPVField.h>
#include <pv/convert.h>

using namespace epics::pvData;
using std::tr1::static_pointer_cast;

static StandardPVFieldPtr standardPVField = getStandardPVField();
static String alarmTimeStamp("alarm,timeStamp");

static bool debug = false;

static void dumpPVStructureCAPI(
    PVStructureCAPIPtr const &pvStructureCAPIPtr,String indent)
{
    String builder;
    builder += indent + "structure ";
    builder += pvStructureCAPIPtr->getPVStructurePtr()->getFieldName();
    printf("%s\n",builder.c_str());
    builder.clear();
    indent += "    ";
    PVFieldCAPIPtrArrayPtr pvFieldCAPIPtrArrayPtr
        = pvStructureCAPIPtr->getPVFieldCAPIPtrArrayPtr();
    PVFieldCAPIPtrArray *pvFieldCAPIPtrArray = pvFieldCAPIPtrArrayPtr.get();
    size_t n = pvFieldCAPIPtrArray->size();
    for(size_t i=0; i<n; i++) {
         PVFieldCAPIPtr pvFieldCAPIPtr = (*pvFieldCAPIPtrArray)[i];
         Type type = pvFieldCAPIPtr->getType();
         switch(type) {
         case scalar:
             {
                 PVScalarCAPIPtr py = static_pointer_cast<PVScalarCAPI>(pvFieldCAPIPtr);
                 PVScalarPtr pv = py->getPVScalarPtr();
                 builder.clear(); builder += indent;
                 pv->toString(&builder);
                 printf("%s\n",builder.c_str());
             }
             break;
         case scalarArray:
             {
                 PVScalarArrayCAPIPtr py = static_pointer_cast<PVScalarArrayCAPI>
                      (pvFieldCAPIPtr);
                 PVScalarArrayPtr pv = py->getPVScalarArrayPtr();
                 builder.clear(); builder += indent;
                 pv->toString(&builder);
                 printf("%s\n",builder.c_str());
             }
             break;
         case structureArray:
             {
                 PVStructureArrayCAPIPtr py = static_pointer_cast<PVStructureArrayCAPI>
                      (pvFieldCAPIPtr);
                 PVStructureArrayPtr pv = py->getPVStructureArrayPtr();
                 builder.clear(); builder += indent;
                 pv->toString(&builder);
                 printf("%s\n",builder.c_str());
             }
             break;
         case structure:
             {
                 PVStructureCAPIPtr py = static_pointer_cast<PVStructureCAPI>
                      (pvFieldCAPIPtr);
                 dumpPVStructureCAPI(py,indent);
             }
             break;
         }
    }
}

static void testScalar(FILE * fd)
{
     PVStructurePtr pv0 = standardPVField->scalar(
         pvDouble,alarmTimeStamp);
     String builder;
     pv0->toString(&builder);
     printf("testScalar\npv0\n%s\n",builder.c_str());
     void *top = PVStructureCAPI::create(pv0);
     PVStructureCAPIPtr pvStructureCAPIPtr = PVStructureCAPI::getPVStructureCAPIPtr(top);
     PVStructurePtr pvStructurePtr = pvStructureCAPIPtr->getPVStructurePtr();
     builder.clear();
     pvStructurePtr->toString(&builder);
     printf("pvStructurePtr\n%s\n",builder.c_str());
     printf("PVStructureCAPIPtr\n");
     dumpPVStructureCAPI(pvStructureCAPIPtr,String(""));
     PVDoublePtr pvDouble = pv0->getDoubleField("value");
     pvDouble->put(10.23);
     PVIntPtr pvInt = pv0->getIntField("alarm.severity");
     pvInt->put(2);
     PVStringPtr pvString = pv0->getStringField("alarm.message");
     pvString->put("test alarm message");
     PVLongPtr pvLong = pv0->getLongField("timeStamp.secondsPastEpoch");
     pvLong->put(123456789123456789LL);
     builder.clear();
     printf("PVStructureCAPIPtr\n");
     dumpPVStructureCAPI(pvStructureCAPIPtr,String(""));
     PVStructureCAPI::destroy(top);
}

static void testScalarArray(FILE * fd)
{
     PVStructurePtr pv0 = standardPVField->scalarArray(
         pvDouble,alarmTimeStamp);
     String builder;
     pv0->toString(&builder);
     printf("testScalarArray\npv0\n%s\n",builder.c_str());
     void *top = PVStructureCAPI::create(pv0);
     PVStructureCAPIPtr pvStructureCAPIPtr = PVStructureCAPI::getPVStructureCAPIPtr(top);
     PVStructurePtr pvStructurePtr = pvStructureCAPIPtr->getPVStructurePtr();
     builder.clear();
     pvStructurePtr->toString(&builder);
     printf("pvStructurePtr\n%s\n",builder.c_str());
     printf("PVStructureCAPIPtr\n");
     dumpPVStructureCAPI(pvStructureCAPIPtr,String(""));
     PVStructureCAPI::destroy(top);
}


int main(int argc,char *argv[])
{
    char *fileName = 0;
    if(argc>1) fileName = argv[1];
    FILE * fd = stdout;
    if(fileName!=0 && fileName[0]!=0) {
        fd = fopen(fileName,"w+");
    }
    testScalar(fd);
    testScalarArray(fd);
    return(0);
}

