/*factory.h*/
#ifndef FACTORY_H
#define FACTORY_H

namespace epics { namespace pvData {

    enum DebugLevel{noDebug,lowDebug,highDebug};

    static DebugLevel debugLevel = highDebug;

}}
#endif /*FACTORY_H */
