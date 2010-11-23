/* enumerated.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <string>
#include "pvTypes.h"
#include "pvData.h"
#ifndef ENUMERATED_H
#define ENUMERATED_H
#include "noDefaultMethods.h"
namespace epics { namespace pvData { 

class Enumerated ;
public:
    //default constructors and destructor are OK
    //This class should not be extended
    
    //returns (false,true) if pvField(isNot, is valid enumerated structure
    bool attach(PVField *pvField);
    ~Enumerated();
    // each of the following throws logic_error is not attached to PVField
    void putIndex(int32 index);
    int32 getIndex();
    String getChoice();
    bool choicesMutable();
    StringArray getChoices();
    int32 getNumberChoices();
    // also throws logic_error of immutable
    void putChoices(StringArray choices,int32 numberChoices);
private:
    PVInt *pvIndex;
    PVStringArray *pvChoices;
};
    
}}
#endif  /* ENUMERATED_H */
