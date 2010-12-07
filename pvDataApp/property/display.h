/* display.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <string>
#include "pvType.h"
#include "pvData.h"
#ifndef DISPLAY_H
#define DISPLAY_H
namespace epics { namespace pvData { 

class Display {
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
    void setDescription(String value) {description = value;}
    String getFormat() const {return format;}
    void setFormat(String value) {format = value;}
    String getUnits() const {return units;}
    void setUnits(String value) {units = value;}
private:
    String description;
    String format;
    String units;
    double low;
    double high;
};
    
}}
#endif  /* DISPLAY_H */
