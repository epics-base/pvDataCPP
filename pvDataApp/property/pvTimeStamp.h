/* pvTimeStamp.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <string>
#include <stdexcept>
#ifndef PVTIMESTAMP_H
#define PVTIMESTAMP_H
#include "pvTypes.h"
#include "timeStamp.h"
#include "pvData.h"
namespace epics { namespace pvData { 

class PVTimeStamp {
public:
    //default constructors and destructor are OK
    //This class should not be extended
    
    //returns (false,true) if pvField(isNot, is valid timeStamp structure
    bool attach(PVField *pvField);
    // throws logic_error is not attached to PVField
    TimeStamp &get();
    // throws logic_error is not attached to PVField
    void put (TimeStamp &timeStamp);
private:
    PVLong* pvSecs;
    PVInt* pvNano;
};
    
}}
#endif  /* PVTIMESTAMP_H */
