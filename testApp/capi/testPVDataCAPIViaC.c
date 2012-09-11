/* testPVDataCAPIViaC.c */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/* Author:  Marty Kraimer Date: 2010.11 */

#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <pv/pvDataCAPICreateTest.h>
#include <pv/pvDataCAPI.h>

static void testScalar()
{
     void *addrTop = pvCAPICreateScalar(10,"alarm,timeStamp");
     printf("testScalar\npvCAPIDumpPVStructureCAPI\n");
     pvCAPIDumpPVStructureCAPI(addrTop);
     int ijunk = pvCAPIGetSubfield(addrTop,"junk");
     printf("ijunk %d\n",ijunk);
     int ind = pvCAPIGetSubfield(addrTop,"value");
     int type = pvCAPIGetFieldType(addrTop,ind);
     const char *fieldName = pvCAPIGetFieldName(addrTop,ind);
     printf("value ind %d type %d fieldName %s\n",ind,type,fieldName);
     void * addrValue = pvCAPIGetPVScalarCAPI(addrTop,ind);
     int scalarType = pvCAPIScalarGetScalarType(addrValue);
     printf("type %d fieldName %s scalarType %d\n",type,fieldName,scalarType);
     double initValue = pvCAPIScalarGetDouble(addrValue);
     double value = 1.234;
     pvCAPIScalarPutDouble(addrValue,value);
     value = pvCAPIScalarGetDouble(addrValue);
     printf("initValue %f value %f\n",initValue,value);
     pvCAPIDestroyPVScalarCAPI(addrValue);
     ind = pvCAPIGetSubfield(addrTop,"alarm");
     void *addrAlarm = pvCAPIGetPVStructureCAPI(addrTop,ind);
     ind = pvCAPIGetSubfield(addrAlarm,"severity");
     void *addrSeverity = pvCAPIGetPVScalarCAPI(addrAlarm,ind);
     int severity = 3;
     pvCAPIScalarPutInt(addrSeverity,severity);
     pvCAPIDestroyPVScalarCAPI(addrSeverity);
     pvCAPIDumpPVStructureCAPI(addrAlarm);
     pvCAPIDestroyPVStructureCAPI(addrAlarm);
     pvCAPIDumpPVStructureCAPI(addrTop);
     pvCAPIDestroyPVStructureCAPI(addrTop);
}

static void testScalarArray()
{
     void *addrTop = pvCAPICreateScalarArray(10,"alarm,timeStamp");
     printf("testScalarArray\npvCAPIDumpPVStructureCAPI\n");
     pvCAPIDumpPVStructureCAPI(addrTop);
     int ijunk = pvCAPIGetSubfield(addrTop,"junk");
     printf("ijunk %d\n",ijunk);
     int ind = pvCAPIGetSubfield(addrTop,"value");
     int type = pvCAPIGetFieldType(addrTop,ind);
     const char *fieldName = pvCAPIGetFieldName(addrTop,ind);
     printf("value ind %d type %d fieldName %s\n",ind,type,fieldName);
     pvCAPIDestroyPVStructureCAPI(addrTop);
}


int main(int argc,char *argv[])
{
    testScalar();
    testScalarArray();
    return(0);
}

