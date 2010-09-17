/*factory.h*/
#ifndef FACTORY_H
#define FACTORY_H

namespace epics { namespace pvData {

    enum DebugLevel{noDebug,lowDebug,highDebug};

    static DebugLevel debugLevel = highDebug;

    static void newLine(StringPtr buffer,int indentLevel) {
        *buffer += "\n";
        for(int i=0; i<indentLevel; i++) *buffer += "    ";
    }

}}
#endif /*FACTORY_H */
