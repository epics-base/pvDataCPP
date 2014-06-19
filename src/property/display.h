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
    : description(std::string("")),format(std::string("")),units(std::string("")),
        low(0.0),high(0.0) {}
    //default constructors and destructor are OK
    double getLow() const {return low;}
    double getHigh() const{ return high;}
    void setLow(double value){low = value;}
    void setHigh(double value){high = value;}
    std::string getDescription() const {return description;}
    void setDescription(std::string const & value) {description = value;}
    std::string getFormat() const {return format;}
    void setFormat(std::string const & value) {format = value;}
    std::string getUnits() const {return units;}
    void setUnits(std::string const & value) {units = value;}
private:
    std::string description;
    std::string format;
    std::string units;
    double low;
    double high;
};
    
}}
#endif  /* DISPLAY_H */
