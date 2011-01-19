/* showConstructDestruct.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
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

typedef int64 (*getTotalFunc)();
typedef void (*deleteStaticFunc)();

class ConstructDestructCallback : private NoDefaultMethods {
public:
    String getConstructName();
    int64 getTotalConstruct();
    int64 getTotalDestruct();
    int64 getTotalReferenceCount();
private:
    ConstructDestructCallback(
        String name,
        getTotalFunc construct,
        getTotalFunc destruct,
        getTotalFunc reference,
        deleteStaticFunc deleteFunc);
    ~ConstructDestructCallback();
    void deleteStatic();
    String name;
    getTotalFunc construct;
    getTotalFunc destruct;
    getTotalFunc reference;
    deleteStaticFunc deleteFunc;
    friend class ShowConstructDestruct;
};

class ShowConstructDestruct : private NoDefaultMethods {
public:
    static void registerCallback(
        String name,
        getTotalFunc construct,
        getTotalFunc destruct,
        getTotalFunc reference,
        deleteStaticFunc deleteFunc);
    ConstructDestructCallback* getConstructDestructCallback(String name);
    void constuctDestructTotals(FILE *fd);
    static void showDeleteStaticExit(FILE *fd);
private:
    ShowConstructDestruct();
    ~ShowConstructDestruct();
    friend ShowConstructDestruct* getShowConstructDestruct();
};

extern ShowConstructDestruct* getShowConstructDestruct();



/* convenience macros - no getTotalReferenceCount() support */

#define PVDATA_REFCOUNT_MONITOR_DEFINE(NAME) \
    static volatile int64 NAME ## _totalConstruct = 0; \
    static volatile int64 NAME ## _totalDestruct = 0; \
    static Mutex NAME ## _globalMutex; \
    \
    static bool NAME ## _notInited = true; \
    static int64 NAME ## _processTotalConstruct() \
    { \
        Lock xx(&NAME ## _globalMutex); \
        return NAME ## _totalConstruct; \
    } \
    \
    static int64 NAME ## _processTotalDestruct() \
    { \
        Lock xx(&NAME ## _globalMutex); \
        return NAME ## _totalDestruct; \
    } \
    \
    static void NAME ## _init() \
    { \
         Lock xx(&NAME ## _globalMutex); \
         if(NAME ## _notInited) { \
            NAME ## _notInited = false; \
            ShowConstructDestruct::registerCallback( \
                String(#NAME), \
                NAME ## _processTotalConstruct,NAME ## _processTotalDestruct,0,0); \
         } \
    }

#define PVDATA_REFCOUNT_MONITOR_DESTRUCT(NAME) \
    Lock xx(&NAME ## _globalMutex); \
    NAME ## _totalDestruct++;

#define PVDATA_REFCOUNT_MONITOR_CONSTRUCT(NAME) \
    NAME ## _init(); \
    Lock xx(&NAME ## _globalMutex); \
    NAME ## _totalConstruct++;


}}
#endif  /* SHOWCONSTRUCTDESTRUCT_H */
