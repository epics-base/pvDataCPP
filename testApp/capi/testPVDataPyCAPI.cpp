/* testPVDataPyCAPI.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/* Author:  Marty Kraimer Date: 2010.11 */

#include <pv/pvDataPyCreateTest.h>

using namespace epics::pvData;

static bool debug = false;

static void dumpPVStructurePy(
     void *addrPVStructurePyPtr,const char *fieldName,String indent)
{
    String builder;
    builder += indent + "structure ";
    builder += fieldName;
    printf("%s\n",builder.c_str());
    builder.clear();
    indent += "    ";
    size_t n = pvPyGetNumberFields(addrPVStructurePyPtr);
    for(size_t i=0; i<n; i++) {
         Type type = static_cast<Type>(pvPyGetFieldType(addrPVStructurePyPtr,i));
         switch(type) {
         case scalar:
             {
                 void * addrPVScalarPyPtr =
                     pvPyGetPVScalarPy(addrPVStructurePyPtr,i);
                 void * addrPVScalarPtr =
                     pvPyGetPVScalar(addrPVScalarPyPtr);
                 PVScalarPtr *pv = static_cast<PVScalarPtr *>(addrPVScalarPtr);
                 builder.clear(); builder += indent;
                 (*pv)->toString(&builder);
                 printf("%s\n",builder.c_str());
             }
             break;
         case scalarArray:
             {
                 void * addrPVScalarArrayPyPtr =
                     pvPyGetPVScalarArrayPy(addrPVStructurePyPtr,i);
                 void * addrPVScalarArrayPtr =
                     pvPyGetPVScalar(addrPVScalarArrayPyPtr);
                 PVScalarArrayPtr *pv =
                     static_cast<PVScalarArrayPtr *>(addrPVScalarArrayPtr);
                 builder.clear(); builder += indent;
                 (*pv)->toString(&builder);
                 printf("%s\n",builder.c_str());
             }
             break;
         case structureArray:
             {
                 void * addrPVStructureArrayPyPtr =
                     pvPyGetPVStructureArrayPy(addrPVStructurePyPtr,i);
                 void * addrPVStructureArrayPtr =
                     pvPyGetPVScalar(addrPVStructureArrayPyPtr);
                 PVStructureArrayPtr *pv =
                     static_cast<PVStructureArrayPtr *>(addrPVStructureArrayPtr);
                 builder.clear(); builder += indent;
                 (*pv)->toString(&builder);
                 printf("%s\n",builder.c_str());
             }
             break;
         case structure:
             {
                 void * xxx =
                     pvPyGetPVStructurePy(addrPVStructurePyPtr,i);
                 const char *fieldName =
                     pvPyGetFieldName(addrPVStructurePyPtr,i);
                 dumpPVStructurePy(xxx,fieldName,indent);
             }
             break;
         }
    }
}


static void testScalar(FILE * fd)
{
     void *addrPVTopPyPtr = pvPyCreateScalar(10,"alarm,timeStamp");
     void *top = pvPyGetTop(addrPVTopPyPtr);
     void *addrPVStructurePyPtr = pvPyGetPVStructurePyPtr(top);
     printf("testScalar\npvPyDumpPVStructurePy\n");
     pvPyDumpPVStructurePy(addrPVStructurePyPtr);
     printf("dumpPVStructurePy\n");
     dumpPVStructurePy(addrPVStructurePyPtr,"top","");
     int ijunk = pvPyGetSubfield(addrPVStructurePyPtr,"junk");
     printf("ijunk %d\n",ijunk);
     int ind = pvPyGetSubfield(addrPVStructurePyPtr,"value");
     int type = pvPyGetFieldType(addrPVStructurePyPtr,ind);
     const char *fieldName = pvPyGetFieldName(addrPVStructurePyPtr,ind);
     void * addrPVScalarPyPtr = pvPyGetPVScalarPy(addrPVStructurePyPtr,ind);
     void * addrPVScalarPtr = pvPyGetPVScalar(addrPVScalarPyPtr);
     int scalarType = pvPyScalarGetScalarType(addrPVScalarPtr);
     printf("type %d fieldName %s scalarType %d\n",type,fieldName,scalarType);
     double initValue = pvPyScalarGetDouble(addrPVScalarPtr);
     double value = 1.234;
     pvPyScalarPutDouble(addrPVScalarPtr,value);
     value = pvPyScalarGetDouble(addrPVScalarPtr);
     printf("initValue %f value %f\n",initValue,value);
     ind = pvPyGetSubfield(addrPVStructurePyPtr,"alarm");
     void *addrAlarm = pvPyGetPVStructurePy(addrPVStructurePyPtr,ind);
     ind = pvPyGetSubfield(addrAlarm,"severity");
     addrPVScalarPyPtr = pvPyGetPVScalarPy(addrAlarm,ind);
     addrPVScalarPtr = pvPyGetPVScalar(addrPVScalarPyPtr);
     int severity = 3;
     pvPyScalarPutInt(addrPVScalarPtr,severity);
     printf("dumpPVStructurePy\n");
     dumpPVStructurePy(addrPVStructurePyPtr,"top","");
     pvPyDeleteTop(addrPVTopPyPtr);
}

static void testScalarArray(FILE * fd)
{
     void *addrPVTopPyPtr = pvPyCreateScalarArray(10,"alarm,timeStamp");
     void *top = pvPyGetTop(addrPVTopPyPtr);
     void *addrPVStructurePyPtr = pvPyGetPVStructurePyPtr(top);
     printf("testScalarArray\npvPyDumpPVStructurePy\n");
     pvPyDumpPVStructurePy(addrPVStructurePyPtr);
     printf("dumpPVStructurePy\n");
     dumpPVStructurePy(addrPVStructurePyPtr,"top","");
     pvPyDeleteTop(addrPVTopPyPtr);
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

