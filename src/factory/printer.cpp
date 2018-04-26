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
    std::size_t spaces = static_cast<std::size_t>(il) * 4;
    return os << string(spaces, ' ');
}

array_at_internal operator<<(std::ostream& str, array_at const& manip)
{
    return array_at_internal(manip.index, str);
}
};

}}
