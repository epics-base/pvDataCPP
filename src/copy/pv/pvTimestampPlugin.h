/* pvTimeStampPlugin.h */
/*
 * The License for this software can be found in the file LICENSE that is included with the distribution.
 */

#ifndef PVTIMESTAMPPLUGIN_H
#define PVTIMESTAMPPLUGIN_H

#if defined(_WIN32) && !defined(NOMINMAX)
#define NOMINMAX
#endif

#include <string>
#include <map>
#include <pv/lock.h>
#include <pv/pvData.h>
#include <pv/pvPlugin.h>
#include <pv/pvTimeStamp.h>

namespace epics { namespace pvData{

class PVTimestampPlugin;
class PVTimestampFilter;

typedef std::tr1::shared_ptr<PVTimestampPlugin> PVTimestampPluginPtr;
typedef std::tr1::shared_ptr<PVTimestampFilter> PVTimestampFilterPtr;


/**
 * A plugin for a filter that sets a timeStamp to the current time.
 * @author mrk
 * @since date 2017.03.24
 */
class PVTimestampPlugin : public PVPlugin
{
private:
    PVTimestampPlugin();
public:
    POINTER_DEFINITIONS(PVTimestampPlugin);
    virtual ~PVTimestampPlugin();
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
 * A filter that sets a timeStamp to the current time.
 */
class PVTimestampFilter : public PVFilter
{
private:
    PVTimeStamp pvTimeStamp;
    TimeStamp timeStamp;
    bool current;
    bool copy;
    PVFieldPtr master;
    

    PVTimestampFilter(bool current,bool copy,PVFieldPtr const & pvField);
public:
    POINTER_DEFINITIONS(PVTimestampFilter);
    virtual ~PVTimestampFilter();
    /**
     * Create a PVTimestampFilter.
     * @param requestValue The value part of a name=value request option.
     * @param master The field in the master PVStructure to which the PVFilter will be attached.
     * @return The PVFilter.
     * A null is returned if master or requestValue is not appropriate for the plugin.
     */
    static PVTimestampFilterPtr create(const std::string & requestValue,const PVFieldPtr & master);
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
#endif  /* PVTIMESTAMPPLUGIN_H */

