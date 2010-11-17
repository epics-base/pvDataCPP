/* enumerated.h */
#include <string>
#include <stdexcept>
#ifndef ENUMERATED_H
#define ENUMERATED_H
#include "pvIntrospect.h"
#include "pvData.h"
#include "noDefaultMethods.h"
namespace epics { namespace pvData { 

class Enumerated : private NoDefaultMethods {
public:
    static Enumerated* create(PVField *pvField);
    ~Enumerated();
    PVInt *getIndex() { return pvIndex;}
    String getChoice();
    PVStringArray *getChoices() { return pvChoices;}
    PVStructure *getPV() { return pvStructure;}
private:
    Enumerated(PVStructure *pvStructure,
        PVInt *pvIndex,PVStringArray *pvChoices);
    PVStructure *pvStructure;
    PVInt *pvIndex;
    PVStringArray *pvChoices;
    StringArrayData stringArrayData;
};
    
}}
#endif  /* ENUMERATED_H */
