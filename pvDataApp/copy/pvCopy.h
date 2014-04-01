/* pvCopy.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
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

#include <shareLib.h>

#include <pv/pvData.h>
#include <pv/bitSet.h>

namespace epics { namespace pvData{ 

class PVCopyTraverseMasterCallback;
typedef std::tr1::shared_ptr<PVCopyTraverseMasterCallback> PVCopyTraverseMasterCallbackPtr;

class epicsShareClass PVCopyTraverseMasterCallback
{
public:
    POINTER_DEFINITIONS(PVCopyTraverseMasterCallback);
    virtual ~PVCopyTraverseMasterCallback() {}
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


class epicsShareClass PVCopy : 
    public std::tr1::enable_shared_from_this<PVCopy>
{
public:
    POINTER_DEFINITIONS(PVCopy);
    static PVCopyPtr create(
        PVStructurePtr const &pvMaster,
        PVStructurePtr const &pvRequest,
        String const & structureName);
    virtual ~PVCopy(){}
    virtual void destroy();
    PVStructurePtr getPVMaster();
    void traverseMaster(PVCopyTraverseMasterCallbackPtr const & callback)
    {
        traverseMaster(headNode,callback);
    }
    StructureConstPtr getStructure();
    PVStructurePtr createPVStructure();
    std::size_t getCopyOffset(PVFieldPtr const  &masterPVField);
    std::size_t getCopyOffset(
        PVStructurePtr const  &masterPVStructure,
        PVFieldPtr const  &masterPVField);
    PVFieldPtr getMasterPVField(std::size_t structureOffset);
    void initCopy(
        PVStructurePtr const  &copyPVStructure,
        BitSetPtr const  &bitSet);
    void updateCopySetBitSet(
        PVStructurePtr const  &copyPVStructure,
        BitSetPtr const  &bitSet);
    void updateCopyFromBitSet(
        PVStructurePtr const  &copyPVStructure,
        BitSetPtr const  &bitSet);
    void updateMaster(
        PVStructurePtr const  &copyPVStructure,
        BitSetPtr const  &bitSet);
    PVStructurePtr getOptions(
        PVStructurePtr const &copyPVStructure,std::size_t fieldOffset);
    String dump();
private:
    void dump(
        String *builder,
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
    String dump(
        String const &value,
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
