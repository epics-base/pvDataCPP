/*PVAuxInfo.cpp*/
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "noDefaultMethods.h"
#include "pvData.h"
#include "factory.h"

namespace epics { namespace pvData {

    class PVAuxInfoPvt {
    public:
        PVAuxInfoPvt(PVField *pvField)
        :  pvField(pvField),
           theMap(std::map<String, PVScalar * >())
        {} 
        PVField *pvField;
        std::map<String, PVScalar * > theMap;
    };

    PVAuxInfo::PVAuxInfo(PVField *pvField)
    : pImpl(new PVAuxInfoPvt(pvField))
    { }

    PVAuxInfo::~PVAuxInfo() { delete pImpl;}
    
    PVField * PVAuxInfo::getPVField() {
        return pImpl->pvField;
    }

    typedef std::map<String,PVScalar * >::const_iterator map_iterator;

    PVScalar * PVAuxInfo::createInfo(String key,ScalarType scalarType)
    {
        map_iterator i = pImpl->theMap.find(key);
        if(i!=pImpl->theMap.end()) {
            String message("AuxoInfo:create key ");
            message += key.c_str();
            message += " already exists with scalarType ";
            ScalarTypeFunc::toString(&message,scalarType);
            pImpl->pvField->message(message,errorMessage);
        }
        PVScalar *pvScalar = getPVDataCreate()->createPVScalar(0,key,scalarType);
        pImpl->theMap.insert(std::pair<String,PVScalar * >(key, pvScalar));
        return pvScalar;

    }

    std::map<String, PVScalar * > *PVAuxInfo::getInfos()
    {
        return &pImpl->theMap;
    }

    PVScalar * PVAuxInfo::getInfo(String key)
    {
        map_iterator i = pImpl->theMap.find(key);
        if(i!=pImpl->theMap.end()) return i->second;
        return 0;
    }

    void PVAuxInfo::toString(StringBuilder buf)
    {
        return PVAuxInfo::toString(buf,0);
    }

    void PVAuxInfo::toString(StringBuilder buf,int indentLevel)
    {
        map_iterator i = pImpl->theMap.begin();
        while(i!=pImpl->theMap.end()) {
             String key = i->first;
             PVScalar *value = i->second;
             *buf += " ";
             *buf += key.c_str();
             *buf += " ";
             value->toString(buf);
             i++;
        }
    }
}}
