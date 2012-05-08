/* pvDisplay.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <string>
#include <pv/pvType.h>
#include <pv/pvData.h>
#include <pv/display.h>
#ifndef PVDISPLAY_H
#define PVDISPLAY_H
namespace epics { namespace pvData { 

class PVDisplay {
public:
    PVDisplay() {}
    //default constructors and destructor are OK
    //An automatic detach is issued if already attached.
    bool attach(PVFieldPtr pvField);
    void detach();
    bool isAttached();
    // each of the following throws logic_error is not attached to PVField
    // a set returns false if field is immutable
    void get(Display &) const;
    bool set(Display const & display);
private:
    PVStringPtr pvDescription;
    PVStringPtr pvFormat;
    PVStringPtr pvUnits;
    PVDoublePtr pvLow;
    PVDoublePtr pvHigh;
};
    
}}
#endif  /* PVDISPLAY_H */
