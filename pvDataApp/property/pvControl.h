/* pvControl.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include "control.h"
#include "pvData.h"
#ifndef PVCONTROL_H
#define PVCONTROL_H
namespace epics { namespace pvData { 

class PVControl {
public:
    PVControl() : pvLow(0),pvHigh(0) {}
    //default constructors and destructor are OK
    //returns (false,true) if pvField(isNot, is valid enumerated structure
    //An automatic detach is issued if already attached.
    bool attach(PVField *pvField);
    void detach();
    // each of the following throws logic_error is not attached to PVField
    // set returns false if field is immutable
    Control get() const;
    bool set(Control control);
private:
    PVDouble *pvLow;
    PVDouble *pvHigh;
};
    
}}
#endif  /* PVCONTROL_H */
