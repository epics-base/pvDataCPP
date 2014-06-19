/* pvTimeStamp.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#ifndef PVTIMESTAMP_H
#define PVTIMESTAMP_H

#include <string>
#include <stdexcept>

#include <pv/pvType.h>
#include <pv/pvData.h>
#include <pv/timeStamp.h>

#include <shareLib.h>

namespace epics { namespace pvData { 

class epicsShareClass PVTimeStamp {
public:
    PVTimeStamp(){}
    //default constructors and destructor are OK
    //This class should not be extended
    
    //returns (false,true) if pvField(isNot, is valid timeStamp structure
    bool attach(PVFieldPtr const & pvField);
    void detach();
    bool isAttached();
    // following throw logic_error is not attached to PVField
    // a set returns false if field is immutable
    void get(TimeStamp &) const;
    bool set(TimeStamp const & timeStamp);
private:
    static std::string noTimeStamp;
    static std::string notAttached;
    PVLongPtr pvSecs;
    PVIntPtr pvUserTag;
    PVIntPtr pvNano;
};
    
}}
#endif  /* PVTIMESTAMP_H */
