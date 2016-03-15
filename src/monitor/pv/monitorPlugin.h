/* monitorPlugin.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#ifndef MONITORPLUGIN_H
#define MONITORPLUGIN_H

#include <list>
#include <pv/pvData.h>
#include <pv/sharedPtr.h>
#include <pv/bitSet.h>
#include <pv/monitor.h>

#include <shareLib.h>

#if !defined(IN_MONITORPLUGIN_CPP)
#warning monitorPlugin.h is deprecated
#endif

#if !defined(IN_MONITORPLUGIN_CPP) && defined(__GNUC__) && !(defined(__vxworks) && !defined(_WRS_VXWORKS_MAJOR))
#define USAGE_DEPRECATED __attribute__((deprecated))
#else
#define USAGE_DEPRECATED
#endif

namespace epics { namespace pvData { 

class MonitorPlugin;
typedef std::tr1::shared_ptr<MonitorPlugin> MonitorPluginPtr;


class MonitorPluginCreator;
typedef std::tr1::shared_ptr<MonitorPluginCreator> MonitorPluginCreatorPtr;


class MonitorPluginManager;
typedef std::tr1::shared_ptr<MonitorPluginManager> MonitorPluginManagerPtr;
        

/**
 * @brief A plugin for raising monitors;
 *
 * This is for use by pvAccess servers that support monitors.
 * Since the interface has only a dependence on pvData it
 * can be used for other purposes.
 * A monitor is assumed to be associated with a field of a top-level
 * structure.
 */
class epicsShareClass USAGE_DEPRECATED MonitorPlugin
{
public:
    virtual ~MonitorPlugin(){}
    /**
     * getName
     * @returns The name of the plugin
     */
    virtual std::string const & getName() = 0;
    /**
     * Should a monitor be raised?
     * @param pvField The field being monitored.
     * @param pvTop The top-level structure in which the field resides.
     * @param monitorElement The client data and bitsets.
     * @returns true or false.
     * True is returned if the change to this field should cause a monitor.
     * False is returned in a change only to this field should not cause a
     * monitor.
     */
    virtual bool causeMonitor(
        PVFieldPtr const &pvField,
        PVStructurePtr const &pvTop,
        MonitorElementPtr const &monitorElement) = 0;
    /**
     * A monitor will be sent to the client.
     * @param monitorElement The data for the client.
     * The plugin is allowed to change the data values.
     */
    virtual void monitorDone(
        MonitorElementPtr const &monitorElement)
        {}
    /**
     * Begin monitoring
     */
    virtual void startMonitoring(){}
    /**
     * Stop monitoring
     */
    virtual void stopMonitoring(){}
    /**
     * Begin a set of puts.
     */
    virtual void beginGroupPut() {};
    /**
     * End a set of puts.
     */
    virtual void endGroupPut() {};
};

/**
 * @brief  A class that creates a plugin.
 *
 * Normlly a plugin is created for a single client.
 */
class epicsShareClass USAGE_DEPRECATED MonitorPluginCreator
{
public:
    virtual ~MonitorPluginCreator() {}
    /**
     * Create a monitor plugin.
     * @param field The introspection interface for the field monitored.
     * @param top The introspection interface for the client structure.
     * @param pvFieldOptions The options the client requested.
     * The structure has a set of PVString subfields.
     * The options are a set of name,value pairs.
     * The subfield name is the name and the subfield value is the value.
     * @returns shared pointer to a MonitorPluginCreator.
     */
    virtual MonitorPluginPtr create(
        FieldConstPtr const &field,
        StructureConstPtr const &top,
        PVStructurePtr const &pvFieldOptions) = 0;
    /**
     * getName
     * @returns The name of the plugin
     */
    virtual std::string const & getName() = 0;
};
    

/**
 * @brief Manager for plugins.
 *
 * This manages a set of monitor plugins.
 * @author mrk
 */
class epicsShareClass USAGE_DEPRECATED MonitorPluginManager
{
public:
    POINTER_DEFINITIONS(MonitorPluginManager);
    /**
     * Factory to get the  manager.
     * @return shared pointer to manager.
     */
    static MonitorPluginManagerPtr get();
    /** destructor
     */
    ~MonitorPluginManager(){}
    /* add plugin
     * @param pluginName The name of the plugin.
     * @param creator The creator.
     * @returns true or false
     * false is returned if a plugin with that name is already present
     */
     bool addPlugin(
         std::string const &pluginName,
         MonitorPluginCreatorPtr const &creator);
    /* find plugin
     * 
     * @param plugin name
     * @returns share pointer to plugin creator.
     * If a plugin with that name is not found NULL is returned.
     */
     MonitorPluginCreatorPtr findPlugin(std::string const &pluginName);
    /* showNames
     * 
     */
    void showNames();
private:
     MonitorPluginManager(){}
     std::list<MonitorPluginCreatorPtr> monitorPluginList;
     epics::pvData::Mutex mutex;
};

#undef USAGE_DEPRECATED

}}
#endif  /* MONITORPLUGIN_H */
