/* pvControl.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#ifndef PVCONTROL_H
#define PVCONTROL_H

#include <pv/pvData.h>
#include <pv/control.h>

#include <shareLib.h>

namespace epics { namespace pvData { 

class epicsShareClass PVControl {
public:
    PVControl(){}
    //default constructors and destructor are OK
    //returns (false,true) if pvField(isNot, is valid enumerated structure
    //An automatic detach is issued if already attached.
    bool attach(PVFieldPtr const & pvField);
    void detach();
    bool isAttached();
    // each of the following throws logic_error is not attached to PVField
    // set returns false if field is immutable
    void get(Control &) const;
    bool set(Control const & control);
private:
    PVDoublePtr pvLow;
    PVDoublePtr pvHigh;
    PVDoublePtr pvMinStep;
    static std::string noControlFound;
    static std::string notAttached;
};
    
}}
#endif  /* PVCONTROL_H */
