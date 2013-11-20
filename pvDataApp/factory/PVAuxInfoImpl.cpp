/*PVAuxInfo.cpp*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>

#define epicsExportSharedSymbols
#include <pv/noDefaultMethods.h>
#include <pv/pvData.h>
#include <pv/convert.h>
#include <pv/factory.h>
#include <pv/lock.h>

namespace epics { namespace pvData {


PVAuxInfo::PVAuxInfo(PVField * pvField)
  :  pvField(pvField),
     pvInfos(std::map<String,std::tr1::shared_ptr<PVScalar> > ())
{
} 

PVAuxInfo::~PVAuxInfo()
{
}


PVField * PVAuxInfo::getPVField() {
    return pvField;
}


PVScalarPtr PVAuxInfo::createInfo(String const & key,ScalarType scalarType)
{
    PVInfoIter iter = pvInfos.find(key);
    if(iter!=pvInfos.end()) {
        String message = key.c_str();
        message += " already exists  ";
        pvField->message(message,errorMessage);
        return nullPVScalar;
    }
    PVScalarPtr pvScalar = getPVDataCreate()->createPVScalar(scalarType);
    pvInfos.insert(PVInfoPair(key,pvScalar));
    return pvScalar;
}

PVScalarPtr  PVAuxInfo::getInfo(String const & key)
{
    PVInfoIter iter;
    iter = pvInfos.find(key);
    if(iter==pvInfos.end()) return nullPVScalar;
    return iter->second;
}

PVAuxInfo::PVInfoMap & PVAuxInfo::getInfoMap()
{
    return pvInfos;
}


void PVAuxInfo::toString(StringBuilder buf)
{
    PVAuxInfo::toString(buf,0);
}

void PVAuxInfo::toString(StringBuilder buf,int indentLevel)
{
    if(pvInfos.size()<=0) return;
    ConvertPtr convert = getConvert();
    convert->newLine(buf,indentLevel);
    *buf += "auxInfo";
    for(PVInfoIter iter = pvInfos.begin(); iter!= pvInfos.end(); ++iter) {
         convert->newLine(buf,indentLevel+1);
         PVFieldPtr value = iter->second;
         value->toString(buf,indentLevel + 1);
    }
}
}}
