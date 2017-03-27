/* pvArrayPlugin.h */
/*
 * The License for this software can be found in the file LICENSE that is included with the distribution.
 */

#ifndef PVARRAYPLUGIN_H
#define PVARRAYPLUGIN_H

#if defined(_WIN32) && !defined(NOMINMAX)
#define NOMINMAX
#endif

#include <string>
#include <map>
#include <pv/lock.h>
#include <pv/pvData.h>
#include <pv/pvPlugin.h>

namespace epics { namespace pvData{

class PVArrayPlugin;
class PVArrayFilter;

typedef std::tr1::shared_ptr<PVArrayPlugin> PVArrayPluginPtr;
typedef std::tr1::shared_ptr<PVArrayFilter> PVArrayFilterPtr;


/**
 * A plugin for a filter that gets a sub array from a PVScalarArray.
 * @author mrk
 * @since date 2017.02.23
 */
class PVArrayPlugin : public PVPlugin
{
private:
    PVArrayPlugin();
public:
    POINTER_DEFINITIONS(PVArrayPlugin);
    virtual ~PVArrayPlugin();
    /**
     * Factory
     */
    static void create();
    /**
     * Create a PVFilter.
     * @param requestValue The value part of a name=value request option.
     * @param pvCopy The PVCopy to which the PVFilter will be attached.
     * @param master The field in the master PVStructure to which the PVFilter will be attached
     * @return The PVFilter.
     * Null is returned if master or requestValue is not appropriate for the plugin.
     */
    virtual PVFilterPtr create(
         const std::string & requestValue,
         const PVCopyPtr & pvCopy,
         const PVFieldPtr & master);
};

/**
 * A filter that gets a sub array from a PVScalarArray.
 */
class PVArrayFilter : public PVFilter
{
private:
    long start;
    long increment;
    long end;
    PVScalarArrayPtr masterArray;

    PVArrayFilter(long start,long increment,long end,const PVScalarArrayPtr & masterArray);
public:
    POINTER_DEFINITIONS(PVArrayFilter);
    virtual ~PVArrayFilter();
    /**
     * Create a PVArrayFilter.
     * @param requestValue The value part of a name=value request option.
     * @param master The field in the master PVStructure to which the PVFilter will be attached.
     * @return The PVFilter.
     * A null is returned if master or requestValue is not appropriate for the plugin.
     */
    static PVArrayFilterPtr create(const std::string & requestValue,const PVFieldPtr & master);
    /**
     * Perform a filter operation
     * @param pvCopy The field in the copy PVStructure.
     * @param bitSet A bitSet for copyPVStructure.
     * @param toCopy (true,false) means copy (from master to copy,from copy to master)
     * @return if filter (modified, did not modify) destination.
     * Null is returned if master or requestValue is not appropriate for the plugin.
     */
    bool filter(const PVFieldPtr & pvCopy,const BitSetPtr & bitSet,bool toCopy);
    /**
     * Get the filter name.
     * @return The name.
     */
    std::string getName();
};

}}
#endif  /* PVARRAYPLUGIN_H */

