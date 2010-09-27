/*PVAuxInfo.cpp*/
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "noDefaultMethods.h"
#include "pvData.h"
#include "factory.h"

namespace epics { namespace pvData {

    static PVDataCreate *pvDataCreate =0;

    class PVAuxInfoPvt {
    public:
        PVAuxInfoPvt(PVField *pvField)
        :  pvField(pvField),
           theMap(std::map<StringConst, PVScalar * >())
        {} 
        PVField *pvField;
        std::map<StringConst, PVScalar * > theMap;
    };

    PVAuxInfo::PVAuxInfo(PVField *pvField)
    : pImpl(new PVAuxInfoPvt(pvField))
    { }

    PVAuxInfo::~PVAuxInfo() { delete pImpl;}

    void PVAuxInfo::init() {
        pvDataCreate = getPVDataCreate();
    }
    
    PVField * PVAuxInfo::getPVField() {
        return pImpl->pvField;
    }

    typedef std::map<StringConst,PVScalar * >::const_iterator map_iterator;

    PVScalar * PVAuxInfo::createInfo(StringConst key,ScalarType scalarType)
    {
        map_iterator i = pImpl->theMap.find(key);
        if(i!=pImpl->theMap.end()) {
            String message("AuxoInfo:create key ");
            message += key.c_str();
            message += " already exists with scalarType ";
            ScalarTypeFunc::toString(&message,scalarType);
            pImpl->pvField->message(message,errorMessage);
        }
        PVScalar *pvScalar = pvDataCreate->createPVScalar(0,key,scalarType);
        pImpl->theMap.insert(std::pair<StringConst,PVScalar * >(key, pvScalar));
        return pvScalar;

    }

    std::map<StringConst, PVScalar * > *PVAuxInfo::getInfos()
    {
        return &pImpl->theMap;
    }

    PVScalar * PVAuxInfo::getInfo(StringConst key)
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
             StringConst key = i->first;
             PVScalar *value = i->second;
             *buf += " ";
             *buf += key.c_str();
             *buf += " ";
             value->toString(buf);
        }
    }
}}
