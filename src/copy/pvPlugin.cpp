/* pvPlugin.cpp */
/*
 * The License for this software can be found in the file LICENSE that is included with the distribution.
 */
#include <pv/pvPlugin.h>

namespace epics { namespace pvData{ 

typedef std::map<std::string,PVPluginPtr> PVPluginMap;

static PVPluginMap pluginMap;
static Mutex mutex;

void PVPluginRegistry::registerPlugin(const std::string & name,const PVPluginPtr & pvPlugin)
{
    Lock xx(mutex);
    PVPluginMap::iterator iter = pluginMap.find(name);
    if(iter!=pluginMap.end()) throw std::logic_error("plugin already registered");
    pluginMap.insert(PVPluginMap::value_type(name,pvPlugin));
}

PVPluginPtr PVPluginRegistry::find(const std::string & name)
{
    Lock xx(mutex);
    PVPluginMap::iterator iter = pluginMap.find(name);
    if(iter!=pluginMap.end()) return (*iter).second;
    return PVPluginPtr();
}

}}

