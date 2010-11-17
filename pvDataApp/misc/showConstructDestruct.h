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

class ConstructDestructCallback {
public:
    virtual String getConstructName() = 0;
    virtual int64 getTotalConstruct() = 0;
    virtual int64 getTotalDestruct() = 0;
    virtual int64 getTotalReferenceCount() = 0;
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
