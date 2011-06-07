/*PVAuxInfo.cpp*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <pv/noDefaultMethods.h>
#include <pv/pvData.h>
#include <pv/convert.h>
#include <pv/factory.h>
#include <pv/lock.h>
#include <pv/CDRMonitor.h>

namespace epics { namespace pvData {

PVDATA_REFCOUNT_MONITOR_DEFINE(pvAuxInfo);

PVAuxInfo::PVAuxInfo(PVField *pvField)
  :  pvField(pvField),lengthInfo(1),numberInfo(0),
     pvInfos(new PVScalar *[1])
{
    PVDATA_REFCOUNT_MONITOR_CONSTRUCT(pvAuxInfo);
} 

PVAuxInfo::~PVAuxInfo()
{
    PVDATA_REFCOUNT_MONITOR_DESTRUCT(pvAuxInfo);
    for(int i=0; i<lengthInfo; i++) delete pvInfos[i];
    delete[] pvInfos;
}


PVField * PVAuxInfo::getPVField() {
    return pvField;
}

PVScalar * PVAuxInfo::createInfo(String key,ScalarType scalarType)
{
    for(int i=0; i<numberInfo; i++) {
        PVScalar *pvScalar = pvInfos[i];
        if(key.compare(pvScalar->getField()->getFieldName())==0) {
            String message("AuxoInfo:create key ");
            message += key.c_str();
            message += " already exists with scalarType ";
            ScalarTypeFunc::toString(&message,scalarType);
            pvField->message(message,errorMessage);
            return 0;
        }
    }
    if(lengthInfo==numberInfo) {
        int newLength = lengthInfo+4;
        PVScalar ** newInfos = new PVScalar *[newLength];
        lengthInfo = newLength;
        for(int i=0; i<numberInfo; i++) newInfos[i] = pvInfos[i];
        for(int i= numberInfo; i<lengthInfo; i++) newInfos[i] = 0;
        delete[] pvInfos;
        pvInfos = newInfos;
    }
    PVScalar *pvScalar = getPVDataCreate()->createPVScalar(0,key,scalarType);
    pvInfos[numberInfo++] = pvScalar;
    return pvScalar;
}

PVScalar * PVAuxInfo::getInfo(String key)
{
    for(int i=0; i<numberInfo; i++) {
        PVScalar *pvScalar = pvInfos[i];
        if(key.compare(pvScalar->getField()->getFieldName())==0) return pvScalar;
    }
    return 0;
}

PVScalar * PVAuxInfo::getInfo(int index)
{
    if(index<0 || index>=numberInfo) return 0;
    return pvInfos[index];
}

int PVAuxInfo::getNumberInfo() { return numberInfo;}

void PVAuxInfo::toString(StringBuilder buf)
{
    PVAuxInfo::toString(buf,0);
}

void PVAuxInfo::toString(StringBuilder buf,int indentLevel)
{
    if(numberInfo==0) return;
    Convert *convert = getConvert();
    convert->newLine(buf,indentLevel);
    *buf += "auxInfo";
    for(int i=0; i<numberInfo; i++) {
         convert->newLine(buf,indentLevel+1);
         PVScalar *value = pvInfos[i];
         value->toString(buf,indentLevel + 1);
    }
}
}}
