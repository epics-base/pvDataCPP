/* display.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>

#include <pv/pvType.h>
#include <pv/pvData.h>

#include <shareLib.h>

namespace epics { namespace pvData { 

class epicsShareClass Display {
public:
    Display()
    : description(String("")),format(String("")),units(String("")),
        low(0.0),high(0.0) {}
    //default constructors and destructor are OK
    double getLow() const {return low;}
    double getHigh() const{ return high;}
    void setLow(double value){low = value;}
    void setHigh(double value){high = value;}
    String getDescription() const {return description;}
    void setDescription(String const & value) {description = value;}
    String getFormat() const {return format;}
    void setFormat(String const & value) {format = value;}
    String getUnits() const {return units;}
    void setUnits(String const & value) {units = value;}
private:
    String description;
    String format;
    String units;
    double low;
    double high;
};
    
}}
#endif  /* DISPLAY_H */
