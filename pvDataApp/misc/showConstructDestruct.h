/* showConstructDestruct.h */
#ifndef SHOWCONSTRUCTDESTRUCT_H
#define SHOWCONSTRUCTDESTRUCT_H
#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "noDefaultMethods.h"
#include "pvType.h"

namespace epics { namespace pvData { 

typedef int64 (*getTotal)();

class ConstructDestructCallback : private NoDefaultMethods {
public:
    ConstructDestructCallback(
        String name,
        getTotal construct,
        getTotal destruct,
        getTotal reference);
    String getConstructName();
    int64 getTotalConstruct();
    int64 getTotalDestruct();
    int64 getTotalReferenceCount();
private:
    ~ConstructDestructCallback();
    String name;
    getTotal construct;
    getTotal destruct;
    getTotal reference;
};

class ShowConstructDestruct : private NoDefaultMethods {
public:
    static void constuctDestructTotals(FILE *fd);
    static void registerCallback(ConstructDestructCallback *callback);
private:
    ShowConstructDestruct();
    friend ShowConstructDestruct* getShowConstructDestruct();
};

extern ShowConstructDestruct* getShowConstructDestruct();

}}
#endif  /* SHOWCONSTRUCTDESTRUCT_H */
