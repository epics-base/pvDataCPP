/* pvEnumerated.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <string>
#include "pvType.h"
#include "pvData.h"
#ifndef PVENUMERATED_H
#define PVENUMERATED_H
namespace epics { namespace pvData { 

class PVEnumerated {
public:
    PVEnumerated() : pvIndex(0), pvChoices(0) {}
    //default constructors and destructor are OK
    //This class should not be extended
    //returns (false,true) if pvField(isNot, is valid enumerated structure
    //An automatic detach is issued if already attached.
    bool attach(PVField *pvField);
    void detach();
    bool isAttached();
    // each of the following throws logic_error is not attached to PVField
    // a set returns false if field is immutable
    bool setIndex(int32 index);
    int32 getIndex();
    String getChoice();
    bool choicesMutable();
    StringArray getChoices();
    int32 getNumberChoices();
    bool setChoices(StringArray choices,int32 numberChoices);
private:
    PVInt *pvIndex;
    PVStringArray *pvChoices;
};
    
}}
#endif  /* PVENUMERATED_H */
