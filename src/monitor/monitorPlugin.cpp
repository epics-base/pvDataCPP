/* monitorPlugin.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */

#define epicsExportSharedSymbols

#include <pv/monitorPlugin.h>

using std::string;
using std::cout;
using std::endl;

namespace epics { namespace pvData { 

MonitorPluginManagerPtr MonitorPluginManager::get()
{
    static MonitorPluginManagerPtr pluginManager;
    static Mutex mutex;
    Lock xx(mutex);
    if(pluginManager==NULL) {
        pluginManager = MonitorPluginManagerPtr(new MonitorPluginManager());
    }
    return pluginManager;
}

bool MonitorPluginManager::addPlugin(
    string const &pluginName,
    MonitorPluginCreatorPtr const &creator)
{
    mutex.lock();
    std::list<MonitorPluginCreatorPtr>::iterator iter;
    for (iter = monitorPluginList.begin();iter!=monitorPluginList.end();iter++)
    {
        if(*iter==creator)
        {
            mutex.unlock();
            return false;
        }
        if(((*iter)->getName().compare(pluginName))==0) 
        {
            mutex.unlock();
            return false;
        }
    }
    monitorPluginList.push_back(creator);
    mutex.unlock();
    return true;
}


MonitorPluginCreatorPtr MonitorPluginManager::findPlugin(
    string const &pluginName)
{
    mutex.lock();
    std::list<MonitorPluginCreatorPtr>::iterator iter;
    for (iter = monitorPluginList.begin();iter!=monitorPluginList.end();++iter)
    {
        if(((*iter)->getName().compare(pluginName))==0) 
        {
            mutex.unlock();
            return *iter;
        }
    }
    mutex.unlock();
    return MonitorPluginCreatorPtr();
}

void MonitorPluginManager::showNames()
{
    mutex.lock();
    std::list<MonitorPluginCreatorPtr>::iterator iter;
    for (iter = monitorPluginList.begin();iter!=monitorPluginList.end();++iter)
    {
        std::cout << (*iter)->getName() << std::endl;
    }
    mutex.unlock();
}


}}
