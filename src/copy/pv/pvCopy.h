/* pvCopy.h */
/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */
/**
 * @author Marty Kraimer
 * @date 2013.04
 */
#ifndef PVCOPY_H
#define PVCOPY_H
#include <string>
#include <stdexcept>
#include <memory>

#include <compilerDependencies.h>
#include <shareLib.h>

#include <pv/pvData.h>
#include <pv/bitSet.h>

namespace epics { namespace pvData{ 

class PVCopyTraverseMasterCallback;
typedef std::tr1::shared_ptr<PVCopyTraverseMasterCallback> PVCopyTraverseMasterCallbackPtr;

/**
 * @brief Callback for traversing master structure
 *
 * Must be implemented by code that creates pvCopy.
 */
class epicsShareClass PVCopyTraverseMasterCallback
{
public:
    POINTER_DEFINITIONS(PVCopyTraverseMasterCallback);
    virtual ~PVCopyTraverseMasterCallback() {}
    /**
     * Called once for each field in master.
     * @param pvField The field in master.
     */
    virtual void nextMasterPVField(epics::pvData::PVFieldPtr const &pvField) = 0;
};


class PVCopy;
typedef std::tr1::shared_ptr<PVCopy> PVCopyPtr;

struct CopyNode;
typedef std::tr1::shared_ptr<CopyNode> CopyNodePtr;
struct CopyMasterNode;
typedef std::tr1::shared_ptr<CopyMasterNode> CopyMasterNodePtr;
struct CopyStructureNode;
typedef std::tr1::shared_ptr<CopyStructureNode> CopyStructureNodePtr;


/**
 * @brief Support for subset of fields in a pvStructure.
 *
 * Class that manages one or more PVStructures that holds an arbitrary subset of the fields
 * in another PVStructure called master.
 */
class epicsShareClass EPICS_DEPRECATED PVCopy :
    public std::tr1::enable_shared_from_this<PVCopy>
{
public:
    POINTER_DEFINITIONS(PVCopy);
    /**
     * Create a new pvCopy
     * @param pvMaster The top-level structure for which a copy of
     * an arbitrary subset of the fields in master will be created and managed.
     * @param pvRequest Selects the set of subfields desired and options for each field.
     * @param structureName The name for the top level of any PVStructure created.
     */
    static PVCopyPtr create(
        PVStructurePtr const &pvMaster,
        PVStructurePtr const &pvRequest,
        std::string const & structureName);
    virtual ~PVCopy(){}
    void destroy();
    /**
     * Get the top-level structure of master
     * @returns The master top-level structure.
     * This should not be modified.
     */
    PVStructurePtr getPVMaster();
    /**
     * Traverse all the fields in master.
     * @param callback This is called for each field on master.
     */
    void traverseMaster(PVCopyTraverseMasterCallbackPtr const & callback)
    {
        traverseMaster(headNode,callback);
    }
    /**
     * Get the introspection interface for a PVStructure for e copy.
     */
    StructureConstPtr getStructure();
    /**
     * Create a copy instance. Monitors keep a queue of monitor elements.
     * Since each element needs a PVStructure, multiple top-level structures will be created.
     */
    PVStructurePtr createPVStructure();
    /**
     * Given a field in pvMaster. return the offset in copy for the same field.
     * A value of std::string::npos means that the copy does not have this field.
     * @param masterPVField The field in master.
     */
    std::size_t getCopyOffset(PVFieldPtr const  &masterPVField);
    /**
     * Given a field in pvMaster. return the offset in copy for the same field.
     * A value of std::string::npos means that the copy does not have this field.
     * @param masterPVStructure A structure in master that has masterPVField.
     * @param masterPVField The field in master.
     */
    std::size_t getCopyOffset(
        PVStructurePtr const  &masterPVStructure,
        PVFieldPtr const  &masterPVField);
    /**
     * Given an offset in the copy get the corresponding field in pvMaster.
     * @param structureOffset The offset in the copy.
     */
    PVFieldPtr getMasterPVField(std::size_t structureOffset);
    /**
     * Initialize the fields in copyPVStructure by giving each field
     * the value from the corresponding field in pvMaster.
     * bitSet will be set to show that all fields are changed.
     * @param copyPVStructure A copy top-level structure.
     * @param bitSet A bitSet for copyPVStructure.
     */
    void initCopy(
        PVStructurePtr const  &copyPVStructure,
        BitSetPtr const  &bitSet);
    /**
     * Set all fields in copyPVStructure to the value of the corresponding field in pvMaster.
     * Each field that is changed has it's corresponding bit set in bitSet.
     * @param copyPVStructure A copy top-level structure.
     * @param bitSet A bitSet for copyPVStructure.
     */
    void updateCopySetBitSet(
        PVStructurePtr const  &copyPVStructure,
        BitSetPtr const  &bitSet);
    /**
     * For each set bit in bitSet
     * set the field in copyPVStructure to the value of the corresponding field in pvMaster.
     * @param copyPVStructure A copy top-level structure.
     * @param bitSet A bitSet for copyPVStructure.
     */
    void updateCopyFromBitSet(
        PVStructurePtr const  &copyPVStructure,
        BitSetPtr const  &bitSet);
    /**
     * For each set bit in bitSet
     * set the field in pvMaster to the value of the corresponding field in copyPVStructure
     * @param copyPVStructure A copy top-level structure.
     * @param bitSet A bitSet for copyPVStructure.
     */
    void updateMaster(
        PVStructurePtr const  &copyPVStructure,
        BitSetPtr const  &bitSet);
    /**
     * Get the options for the field at the specified offset.
     * @param fieldOffset the offset in copy.
     * @returns A NULL is returned if no options were specified for the field.
     * If options were specified,PVStructurePtr is a structures
     *  with a set of PVString subfields that specify name,value pairs.s
     *  name is the subField name and value is the subField value.
     */
    PVStructurePtr getOptions(std::size_t fieldOffset);
    /**
     * For debugging.
     */
    std::string dump();
private:
    void dump(
        std::string *builder,
        CopyNodePtr const &node,
        int indentLevel);
    PVCopyPtr getPtrSelf()
    {
        return shared_from_this();
    }
    void traverseMaster(CopyNodePtr const &node, PVCopyTraverseMasterCallbackPtr const & callback);

    PVStructurePtr pvMaster;
    StructureConstPtr structure;
    CopyNodePtr headNode;
    PVStructurePtr cacheInitStructure;
    PVCopy(PVStructurePtr const &pvMaster);
    friend class PVCopyMonitor;
    bool init(PVStructurePtr const &pvRequest);
    std::string dump(
        std::string const &value,
        CopyNodePtr const &node,
        int indentLevel);
    StructureConstPtr createStructure(
        PVStructurePtr const &pvMaster,
        PVStructurePtr const &pvFromRequest);
    CopyNodePtr createStructureNodes(
        PVStructurePtr const &pvMasterStructure,
        PVStructurePtr const &pvFromRequest,
        PVStructurePtr const &pvFromField);
    void updateStructureNodeSetBitSet(
        PVStructurePtr const &pvCopy,
        CopyStructureNodePtr const &structureNode,
        BitSetPtr const &bitSet);
    void updateSubFieldSetBitSet(
        PVFieldPtr const &pvCopy,
        PVFieldPtr const &pvMaster,
        BitSetPtr const &bitSet);
    void updateStructureNodeFromBitSet(
        PVStructurePtr const &pvCopy,
        CopyStructureNodePtr const &structureNode,
        BitSetPtr const &bitSet,
        bool toCopy,
        bool doAll);
    void updateSubFieldFromBitSet(
        PVFieldPtr const &pvCopy,
        PVFieldPtr const &pvMasterField,
        BitSetPtr const &bitSet,
        bool toCopy,
        bool doAll);
    CopyMasterNodePtr getCopyOffset(
        CopyStructureNodePtr const &structureNode,
        PVFieldPtr const &masterPVField);
    CopyMasterNodePtr getMasterNode(
        CopyStructureNodePtr const &structureNode,
        std::size_t structureOffset);
    
};

}}

#endif  /* PVCOPY_H */
