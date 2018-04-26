/* printer.cpp */
/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */

#include <deque>

#define epicsExportSharedSymbols
#include <pv/pvIntrospect.h>

using std::string;

namespace epics { namespace pvData {

namespace format
{
static int indent_index = std::ios_base::xalloc();

long& indent_value(std::ios_base& ios)
{
    return ios.iword(indent_index);
}

std::ostream& operator<<(std::ostream& os, indent_level const& indent)
{
    indent_value(os) = indent.level;
    return os;
}

std::ostream& operator<<(std::ostream& os, indent const&)
{
    long il = indent_value(os);
    for(long i=0, spaces = il * 4; i<spaces; i++)
        os.put(' ');
    return os;
}

array_at_internal operator<<(std::ostream& str, array_at const& manip)
{
    return array_at_internal(manip.index, str);
}
};

}}
