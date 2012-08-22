/* control.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#ifndef CONTROL_H
#define CONTROL_H
namespace epics { namespace pvData { 

class Control {
public:
    Control() : low(0.0), high(0.0) {}
    //default constructors and destructor are OK
    double getLow() const {return low;}
    double getHigh() const {return high;}
    double getMinStep() const {return minStep;}
    void setLow(double value) {low = value;}
    void setHigh(double value) {high = value;}
    void setMinStep(double value) {minStep = value;}
private:
    double low;
    double high;
    double minStep;
};
    
}}
#endif  /* CONTROL_H */
