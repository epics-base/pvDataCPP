/* pvCopy.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 * @author Marty Kraimer
 * @date 2013.04
 */
#include <string>
#include <stdexcept>
#include <memory>
#include <sstream>

#include <pv/thread.h>

#include <pv/pvCopy.h>
#include <pv/convert.h>


namespace epics { namespace pvData { 

using namespace epics::pvData;
using std::tr1::static_pointer_cast;
using std::tr1::dynamic_pointer_cast;
using std::size_t;
using std::cout;
using std::endl;

static PVCopyPtr NULLPVCopy;
static FieldConstPtr NULLField;
static StructureConstPtr NULLStructure;
static PVStructurePtr NULLPVStructure;
static CopyNodePtr NULLCopyNode;
static CopyMasterNodePtr NULLCopyMasterNode;

struct CopyNode {
    CopyNode()
    : isStructure(false),
      structureOffset(0),
      nfields(0)
    {}
    bool isStructure;
    size_t structureOffset; // In the copy
    size_t nfields;
    PVStructurePtr options;
};
    
struct CopyMasterNode : public CopyNode{
    PVFieldPtr masterPVField;
};

typedef std::vector<CopyNodePtr> CopyNodePtrArray;
typedef std::tr1::shared_ptr<CopyNodePtrArray> CopyNodePtrArrayPtr;
    
struct CopyStructureNode : public  CopyNode {
    CopyNodePtrArrayPtr nodes;
};

PVCopyPtr PVCopy::create(
    PVStructurePtr const &pvMaster, 
    PVStructurePtr const &pvRequest, 
    String const & structureName)
{
    PVStructurePtr pvStructure(pvRequest);
    if(structureName.size()>0) {
        if(pvRequest->getStructure()->getNumberFields()>0) {
            pvStructure = pvRequest->getStructureField(structureName);
            if(pvStructure.get()==NULL) return NULLPVCopy;
        }
    } else if(pvStructure->getSubField("field")!=NULL) {
        pvStructure = pvRequest->getStructureField("field");
    }
    PVCopyPtr pvCopy = PVCopyPtr(new PVCopy(pvMaster));
    bool result = pvCopy->init(pvStructure);
    if(!result) pvCopy.reset();
    return pvCopy;
}

PVCopy::PVCopy(
    PVStructurePtr const &pvMaster)
: pvMaster(pvMaster)
{
}

void PVCopy::destroy()
{
    headNode.reset();
}

PVStructurePtr PVCopy::getPVMaster()
{
    return pvMaster;
}

void PVCopy::traverseMaster(CopyNodePtr const &innode, PVCopyTraverseMasterCallbackPtr const & callback)
{
    CopyNodePtr node = innode;
    if(!node->isStructure) {
        CopyMasterNodePtr masterNode = static_pointer_cast<CopyMasterNode>(node);
        callback->nextMasterPVField(masterNode->masterPVField);
        return;
    }
    CopyStructureNodePtr structNode = static_pointer_cast<CopyStructureNode>(node);
    CopyNodePtrArrayPtr nodes = structNode->nodes;
    for(size_t i=0; i< nodes->size(); i++) {
        node = (*nodes)[i];
        traverseMaster(node,callback);
    }
}

StructureConstPtr PVCopy::getStructure()
{
    return structure;
}

PVStructurePtr PVCopy::createPVStructure()
{
    if(cacheInitStructure.get()!=NULL) {
        PVStructurePtr save = cacheInitStructure;
        cacheInitStructure.reset();
        return save;
    }
    PVStructurePtr pvStructure = 
        getPVDataCreate()->createPVStructure(structure);
    return pvStructure;
}

PVStructurePtr PVCopy::getOptions(std::size_t fieldOffset)
{
    if(fieldOffset==0) return headNode->options;
    CopyNodePtr node = headNode;
    while(true) {
        if(!node->isStructure) {
            if(node->structureOffset==fieldOffset) return node->options;
            return NULLPVStructure;
        }
        CopyStructureNodePtr structNode = static_pointer_cast<CopyStructureNode>(node);
        CopyNodePtrArrayPtr nodes = structNode->nodes;
        boolean okToContinue = false;
        for(size_t i=0; i< nodes->size(); i++) {
            node = (*nodes)[i];
            size_t soff = node->structureOffset;
            if(fieldOffset>=soff && fieldOffset<soff+node->nfields) {
                if(fieldOffset==soff) return node->options;
                if(!node->isStructure) {
                    return NULLPVStructure;
                }
                okToContinue = true;
                break;
            }
        }
        if(okToContinue) continue;
        throw std::invalid_argument("fieldOffset not valid");
    }
}

size_t PVCopy::getCopyOffset(PVFieldPtr const &masterPVField)
{
    if(masterPVField->getFieldOffset()==0) return 0;
    if(!headNode->isStructure) {
        CopyMasterNodePtr masterNode = static_pointer_cast<CopyMasterNode>(headNode);
        if((masterNode->masterPVField.get())==masterPVField.get()) {
             return headNode->structureOffset;
        }
        PVStructure * parent = masterPVField->getParent();
        size_t offsetParent = parent->getFieldOffset();
        size_t off = masterPVField->getFieldOffset();
        size_t offdiff = off -offsetParent;
        if(offdiff<masterNode->nfields) return headNode->structureOffset + offdiff;
        return String::npos;
    }
    CopyStructureNodePtr node = static_pointer_cast<CopyStructureNode>(headNode);
    CopyMasterNodePtr masterNode = getCopyOffset(node,masterPVField);
    if(masterNode.get()!=NULL) return masterNode->structureOffset;
    return String::npos;
}

size_t PVCopy::getCopyOffset(
    PVStructurePtr const  &masterPVStructure,
    PVFieldPtr const  &masterPVField)
{
    CopyMasterNodePtr masterNode;
    if(!headNode->isStructure) {
        masterNode = static_pointer_cast<CopyMasterNode>(headNode);
        if(masterNode->masterPVField.get()!=masterPVStructure.get()) return String::npos;
    } else {
        CopyStructureNodePtr node = static_pointer_cast<CopyStructureNode>(headNode);
        masterNode = getCopyOffset(node,masterPVField);
    }
    if(masterNode.get()==NULL) return String::npos;
    size_t diff = masterPVField->getFieldOffset()
        - masterPVStructure->getFieldOffset();
    return masterNode->structureOffset + diff;
}

PVFieldPtr PVCopy::getMasterPVField(size_t structureOffset)
{
    CopyMasterNodePtr masterNode;
    if(!headNode->isStructure) {
        masterNode = static_pointer_cast<CopyMasterNode>(headNode);
    } else {
        CopyStructureNodePtr node = static_pointer_cast<CopyStructureNode>(headNode);
        masterNode = getMasterNode(node,structureOffset);
    }
    if(masterNode.get()==NULL) {
        throw std::invalid_argument(
            "PVCopy::getMasterPVField: setstructureOffset not valid");
    }
    size_t diff = structureOffset - masterNode->structureOffset;
    PVFieldPtr pvMasterField = masterNode->masterPVField;
    if(diff==0) return pvMasterField;
    PVStructurePtr pvStructure
        = static_pointer_cast<PVStructure>(pvMasterField);
    return pvStructure->getSubField(
        pvMasterField->getFieldOffset() + diff);
}

void PVCopy::initCopy(
    PVStructurePtr const  &copyPVStructure,
    BitSetPtr const  &bitSet)
{
    bitSet->clear();
    bitSet->set(0);
    updateCopyFromBitSet(copyPVStructure,bitSet);
}

void PVCopy::updateCopySetBitSet(
    PVStructurePtr const  &copyPVStructure,
    BitSetPtr const  &bitSet)
{
    if(headNode->isStructure) {
        CopyStructureNodePtr node = static_pointer_cast<CopyStructureNode>(headNode);
        updateStructureNodeSetBitSet(copyPVStructure,node,bitSet);
    } else {
        CopyMasterNodePtr masterNode = static_pointer_cast<CopyMasterNode>(headNode);
        PVFieldPtr pvMasterField= masterNode->masterPVField;
        PVFieldPtr copyPVField = copyPVStructure;
        PVFieldPtrArray const & pvCopyFields = copyPVStructure->getPVFields();
        if(pvCopyFields.size()==1) {
            copyPVField = pvCopyFields[0];
        }
        PVFieldPtr pvField = pvMasterField;
        if(pvField->getField()->getType()==epics::pvData::structure) {
            updateSubFieldSetBitSet(copyPVField,pvMasterField,bitSet);
            return;
        }
        if(pvCopyFields.size()!=1) {
            throw std::logic_error("Logic error");
        }
        ConvertPtr convert = getConvert();
        bool isEqual = convert->equals(copyPVField,pvField);
        if(!isEqual) {
            convert->copy(pvField, copyPVField);
            bitSet->set(copyPVField->getFieldOffset());
        }
    }
}

void PVCopy::updateCopyFromBitSet(
    PVStructurePtr const  &copyPVStructure,
    BitSetPtr const  &bitSet)
{
    bool doAll = bitSet->get(0);
    if(headNode->isStructure) {
        CopyStructureNodePtr node = static_pointer_cast<CopyStructureNode>(headNode);
        updateStructureNodeFromBitSet(copyPVStructure,node,bitSet,true,doAll);
    } else {
        CopyMasterNodePtr masterNode = static_pointer_cast<CopyMasterNode>(headNode);
        PVFieldPtrArray const & pvCopyFields = copyPVStructure->getPVFields();
        if(pvCopyFields.size()==1) {
            updateSubFieldFromBitSet(
                pvCopyFields[0],
                masterNode->masterPVField,bitSet,
                true,doAll);
        } else {
            updateSubFieldFromBitSet(
                copyPVStructure,
                masterNode->masterPVField,bitSet,
                true,doAll);
        }
    }
}

void PVCopy::updateMaster(
    PVStructurePtr const  &copyPVStructure,
    BitSetPtr const  &bitSet)
{
    bool doAll = bitSet->get(0);
    if(headNode->isStructure) {
        CopyStructureNodePtr node =
            static_pointer_cast<CopyStructureNode>(headNode);
        updateStructureNodeFromBitSet(
            copyPVStructure,node,bitSet,false,doAll);
    } else {
        CopyMasterNodePtr masterNode =
            static_pointer_cast<CopyMasterNode>(headNode);
        PVFieldPtrArray const & pvCopyFields =
            copyPVStructure->getPVFields();
        if(pvCopyFields.size()==1) {
            updateSubFieldFromBitSet(
                pvCopyFields[0],
                masterNode->masterPVField,bitSet,
                false,doAll);
        } else {
            updateSubFieldFromBitSet(
                copyPVStructure,
                masterNode->masterPVField,bitSet,
                false,doAll);
        }
    }
}

epics::pvData::String PVCopy::dump()
{
    String builder;
    dump(&builder,headNode,0);
    return builder;
}

void PVCopy::dump(String *builder,CopyNodePtr const &node,int indentLevel)
{
    getConvert()->newLine(builder,indentLevel);
    std::stringstream ss;
    ss << (node->isStructure ? "structureNode" : "masterNode");
    ss << " structureOffset " << node->structureOffset;
    ss << " nfields " << node->nfields;
    *builder +=  ss.str();
    PVStructurePtr options = node->options;
    if(options.get()!=NULL) {
        getConvert()->newLine(builder,indentLevel +1);
        options->toString(builder);
        getConvert()->newLine(builder,indentLevel);
    }
    if(!node->isStructure) {
        CopyMasterNodePtr masterNode = static_pointer_cast<CopyMasterNode>(node);
        String name = masterNode->masterPVField->getFullName();
        *builder += " masterField " + name;
        return;
    }
    CopyStructureNodePtr structureNode =
        static_pointer_cast<CopyStructureNode>(node);
    CopyNodePtrArrayPtr nodes = structureNode->nodes;
    for(size_t i=0; i<nodes->size(); ++i) {
        if((*nodes)[i].get()==NULL) {
            getConvert()->newLine(builder,indentLevel +1);
            ss.str("");
            ss << "node[" << i << "] is null";
            *builder += ss.str();
            continue;
        }
        dump(builder,(*nodes)[i],indentLevel+1);
    }
}

bool PVCopy::init(epics::pvData::PVStructurePtr const &pvRequest)
{
    PVStructurePtr pvMasterStructure = pvMaster;
    size_t len = pvRequest->getPVFields().size();
    bool entireMaster = false;
    if(len==String::npos) entireMaster = true;
    if(len==0) entireMaster = true;
    PVStructurePtr pvOptions;
    if(len==1 && pvRequest->getSubField("_options")!=NULL) {
        pvOptions = pvRequest->getStructureField("_options");
    }
    if(entireMaster) {
        structure = pvMasterStructure->getStructure();
        CopyMasterNodePtr masterNode(new CopyMasterNode());
        headNode = masterNode;
        masterNode->options = pvOptions;
        masterNode->isStructure = false;
        masterNode->structureOffset = 0;
        masterNode->masterPVField = pvMasterStructure;
        masterNode->nfields = pvMasterStructure->getNumberFields();
        return true;
    }
    structure = createStructure(pvMasterStructure,pvRequest);
    if(structure==NULL) return false;
    cacheInitStructure = createPVStructure();
    headNode = createStructureNodes(
        pvMaster,
        pvRequest,
        cacheInitStructure);
    return true;
}

epics::pvData::String PVCopy::dump(
    String const &value,
    CopyNodePtr const &node,
    int indentLevel)
{
    throw std::logic_error(String("Not Implemented"));
}


StructureConstPtr PVCopy::createStructure(
    PVStructurePtr const &pvMaster,
    PVStructurePtr const  &pvFromRequest)
{
    if(pvFromRequest->getStructure()->getNumberFields()==0) {
        return pvMaster->getStructure();
    }
    PVFieldPtrArray const &pvFromRequestFields = pvFromRequest->getPVFields();
    StringArray const &fromRequestFieldNames = pvFromRequest->getStructure()->getFieldNames();
    size_t length = pvFromRequestFields.size();
    if(length==0) return NULLStructure;
    FieldConstPtrArray fields; fields.reserve(length);
    StringArray fieldNames; fields.reserve(length);
    for(size_t i=0; i<length; ++i) {
        String const &fieldName = fromRequestFieldNames[i];
        PVFieldPtr pvMasterField = pvMaster->getSubField(fieldName);
        if(pvMasterField==NULL) continue;
        FieldConstPtr field = pvMasterField->getField();
        if(field->getType()==epics::pvData::structure) {
            PVStructurePtr pvRequestStructure = static_pointer_cast<PVStructure>(
                pvFromRequestFields[i]);
            if(pvRequestStructure->getNumberFields()>0) {
                 StringArray const &names = pvRequestStructure->getStructure()->
                     getFieldNames();
                 size_t num = names.size();
                 if(num>0 && names[0].compare("_options")==0) --num;
                 if(num>0) {
                     if(pvMasterField->getField()->getType()!=epics::pvData::structure) continue;
                     fieldNames.push_back(fieldName);
                     fields.push_back(createStructure(
                         static_pointer_cast<PVStructure>(pvMasterField),
                         pvRequestStructure));
                     continue;
                 }
            }
        }
        fieldNames.push_back(fieldName);
        fields.push_back(field);
    }
    size_t numsubfields = fields.size();
    if(numsubfields==0) return NULLStructure;
    return getFieldCreate()->createStructure(fieldNames, fields);
}

CopyNodePtr PVCopy::createStructureNodes(
    PVStructurePtr const &pvMasterStructure,
    PVStructurePtr const &pvFromRequest,
    PVStructurePtr const &pvFromCopy)
{
    PVFieldPtrArray const & copyPVFields = pvFromCopy->getPVFields();
    PVStructurePtr pvOptions;
    PVFieldPtr pvField = pvFromRequest->getSubField("_options");
    if(pvField!=NULL) pvOptions = static_pointer_cast<PVStructure>(pvField);
    size_t number = copyPVFields.size();
    CopyNodePtrArrayPtr nodes(new CopyNodePtrArray());
    nodes->reserve(number);
    for(size_t i=0; i<number; i++) {
        PVFieldPtr copyPVField = copyPVFields[i];
        String fieldName = copyPVField->getFieldName();
        
        PVStructurePtr requestPVStructure = static_pointer_cast<PVStructure>(
              pvFromRequest->getSubField(fieldName));
        PVStructurePtr pvSubFieldOptions;
        PVFieldPtr pvField = requestPVStructure->getSubField("_options");
        if(pvField!=NULL) pvSubFieldOptions = static_pointer_cast<PVStructure>(pvField);
        PVFieldPtr pvMasterField;
        PVFieldPtrArray const & pvMasterFields = pvMasterStructure->getPVFields();
        for(size_t j=0; i<pvMasterFields.size(); j++ ) {
            if(pvMasterFields[j]->getFieldName().compare(fieldName)==0) {
                pvMasterField = pvMasterFields[j];
                break;
            }
        }
        size_t numberRequest = requestPVStructure->getPVFields().size();
        if(pvSubFieldOptions!=NULL) numberRequest--;
        if(numberRequest>0) {
            nodes->push_back(createStructureNodes(
                static_pointer_cast<PVStructure>(pvMasterField),
                requestPVStructure,
                static_pointer_cast<PVStructure>(copyPVField)));
            continue;
        }
        CopyMasterNodePtr masterNode(new CopyMasterNode());
        masterNode->options = pvSubFieldOptions;
        masterNode->isStructure = false;
        masterNode->masterPVField = pvMasterField;
        masterNode->nfields = copyPVField->getNumberFields();
        masterNode->structureOffset = copyPVField->getFieldOffset();
        nodes->push_back(masterNode);
    }
    CopyStructureNodePtr structureNode(new CopyStructureNode());
    structureNode->isStructure = true;
    structureNode->nodes = nodes;
    structureNode->structureOffset = pvFromCopy->getFieldOffset();
    structureNode->nfields = pvFromCopy->getNumberFields();
    structureNode->options = pvOptions;
    return structureNode;
}

void PVCopy::updateStructureNodeSetBitSet(
    PVStructurePtr const &pvCopy,
    CopyStructureNodePtr const &structureNode,
    epics::pvData::BitSetPtr const &bitSet)
{
    for(size_t i=0; i<structureNode->nodes->size(); i++) {
        CopyNodePtr node = (*structureNode->nodes)[i];
        PVFieldPtr pvField = pvCopy->getSubField(node->structureOffset);
        if(node->isStructure) {
            PVStructurePtr xxx = static_pointer_cast<PVStructure>(pvField);
            CopyStructureNodePtr yyy =
                static_pointer_cast<CopyStructureNode>(node);
            updateStructureNodeSetBitSet(xxx,yyy,bitSet); 
        } else {
            CopyMasterNodePtr masterNode =
                static_pointer_cast<CopyMasterNode>(node);
            updateSubFieldSetBitSet(pvField,masterNode->masterPVField,bitSet);
        }
    }
}

void PVCopy::updateSubFieldSetBitSet(
    PVFieldPtr const &pvCopy,
    PVFieldPtr const &pvMaster,
    BitSetPtr const &bitSet)
{
    FieldConstPtr field = pvCopy->getField();
    Type type = field->getType();
    if(type!=epics::pvData::structure) {
        ConvertPtr convert = getConvert();
        bool isEqual = convert->equals(pvCopy,pvMaster);
    	if(isEqual) {
    	    if(type==structureArray) {
    	        // always act as though a change occurred.
    	        // Note that array elements are shared.
		bitSet->set(pvCopy->getFieldOffset());
    	    }
    	}
        if(isEqual) return;
        convert->copy(pvMaster, pvCopy);
        bitSet->set(pvCopy->getFieldOffset());
        return;
    }
    PVStructurePtr pvCopyStructure = static_pointer_cast<PVStructure>(pvCopy);
    PVFieldPtrArray const & pvCopyFields = pvCopyStructure->getPVFields();
    PVStructurePtr pvMasterStructure =
        static_pointer_cast<PVStructure>(pvMaster);
    PVFieldPtrArray const & pvMasterFields =
        pvMasterStructure->getPVFields();
    size_t length = pvCopyFields.size();
    for(size_t i=0; i<length; i++) {
        updateSubFieldSetBitSet(pvCopyFields[i],pvMasterFields[i],bitSet);
    }
}

void PVCopy::updateStructureNodeFromBitSet(
    PVStructurePtr const &pvCopy,
    CopyStructureNodePtr const &structureNode,
    BitSetPtr const &bitSet,
    bool toCopy,
    bool doAll)
{
    size_t offset = structureNode->structureOffset;
    if(!doAll) {
        size_t nextSet = bitSet->nextSetBit(offset);
        if(nextSet==String::npos) return;
    }
    if(offset>=pvCopy->getNextFieldOffset()) return;
    if(!doAll) doAll = bitSet->get(offset);
    CopyNodePtrArrayPtr  nodes = structureNode->nodes;
    for(size_t i=0; i<nodes->size(); i++) {
        CopyNodePtr node = (*nodes)[i];
        PVFieldPtr pvField = pvCopy->getSubField(node->structureOffset);
        if(node->isStructure) {
            PVStructurePtr xxx = static_pointer_cast<PVStructure>(pvField);
            CopyStructureNodePtr subStructureNode =
                static_pointer_cast<CopyStructureNode>(node);
            updateStructureNodeFromBitSet(
                 xxx,subStructureNode,bitSet,toCopy,doAll);
        } else {
            CopyMasterNodePtr masterNode =
                static_pointer_cast<CopyMasterNode>(node);
            updateSubFieldFromBitSet(
                pvField,masterNode->masterPVField,bitSet,toCopy,doAll);
        }
    }
}

void PVCopy::updateSubFieldFromBitSet(
    PVFieldPtr const &pvCopy,
    PVFieldPtr const &pvMasterField,
    BitSetPtr const &bitSet,
    bool toCopy,
    bool doAll)
{
    if(!doAll) {
        doAll = bitSet->get(pvCopy->getFieldOffset());
    }
    if(!doAll) {
        size_t offset = pvCopy->getFieldOffset();
        size_t nextSet = bitSet->nextSetBit(offset);
        if(nextSet==String::npos) return;
        if(nextSet>=pvCopy->getNextFieldOffset()) return;
    }
    ConvertPtr convert = getConvert();
    if(pvCopy->getField()->getType()==epics::pvData::structure) {
        PVStructurePtr pvCopyStructure =
            static_pointer_cast<PVStructure>(pvCopy);
        PVFieldPtrArray const & pvCopyFields = pvCopyStructure->getPVFields();
        if(pvMasterField->getField()->getType()
        !=epics::pvData::structure)
        {
            if(pvCopyFields.size()!=1) {
                throw std::logic_error(String("Logic error"));
            }
            if(toCopy) {
                convert->copy(pvMasterField, pvCopyFields[0]);
            } else {
                convert->copy(pvCopyFields[0], pvMasterField);
            }
            return;
        }
        PVStructurePtr pvMasterStructure = 
            static_pointer_cast<PVStructure>(pvMasterField);
        PVFieldPtrArray const & pvMasterFields =
            pvMasterStructure->getPVFields();
        for(size_t i=0; i<pvCopyFields.size(); i++) {
            updateSubFieldFromBitSet(
                pvCopyFields[i],
                pvMasterFields[i],
                bitSet,toCopy,doAll);
        }
    } else {
        if(toCopy) {
            convert->copy(pvMasterField, pvCopy);
        } else {
            convert->copy(pvCopy, pvMasterField);
        }
    }
}

CopyMasterNodePtr PVCopy::getCopyOffset(
        CopyStructureNodePtr const &structureNode,
        PVFieldPtr const &masterPVField)
{
    size_t offset = masterPVField->getFieldOffset();
    CopyNodePtrArrayPtr nodes = structureNode->nodes;
    for(size_t i=0; i< nodes->size(); i++) {
        CopyNodePtr node = (*nodes)[i];
        if(!node->isStructure) {
            CopyMasterNodePtr masterNode =
                static_pointer_cast<CopyMasterNode>(node);
            size_t off = masterNode->masterPVField->getFieldOffset();
            size_t nextOffset = masterNode->masterPVField->getNextFieldOffset(); 
            if(offset>= off && offset<nextOffset) return masterNode;
        } else {
            CopyStructureNodePtr subNode =
                static_pointer_cast<CopyStructureNode>(node);
            CopyMasterNodePtr masterNode =
                getCopyOffset(subNode,masterPVField);
            if(masterNode.get()!=NULL) return masterNode;
        }
    }
    return NULLCopyMasterNode;
}

CopyMasterNodePtr PVCopy::getMasterNode(
        CopyStructureNodePtr const &structureNode,
        std::size_t structureOffset)
{
    CopyNodePtrArrayPtr nodes = structureNode->nodes;
    for(size_t i=0; i< nodes->size(); i++) {
        CopyNodePtr node = (*nodes)[i];
        if(structureOffset>=(node->structureOffset + node->nfields)) continue;
        if(!node->isStructure) {
            CopyMasterNodePtr masterNode =
                static_pointer_cast<CopyMasterNode>(node);
            return masterNode;
        }
        CopyStructureNodePtr subNode =
            static_pointer_cast<CopyStructureNode>(node);
        return  getMasterNode(subNode,structureOffset);
    }
    return NULLCopyMasterNode;
}

}}
