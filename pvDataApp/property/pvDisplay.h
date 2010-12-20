/* pvDisplay.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <string>
#include "pvType.h"
#include "pvData.h"
#include "display.h"
#ifndef PVDISPLAY_H
#define PVDISPLAY_H
namespace epics { namespace pvData { 

class PVDisplay {
public:
    PVDisplay()
    : pvDescription(0),pvFormat(),pvUnits(),pvLow(),pvHigh() {}
    //default constructors and destructor are OK
    //An automatic detach is issued if already attached.
    bool attach(PVField *pvField);
    void detach();
    bool isAttached();
    // each of the following throws logic_error is not attached to PVField
    // a set returns false if field is immutable
    void get(Display &) const;
    bool set(Display const & display);
private:
    PVString *pvDescription;
    PVString *pvFormat;
    PVString *pvUnits;
    PVDouble *pvLow;
    PVDouble *pvHigh;
};
    
}}
#endif  /* PVDISPLAY_H */
