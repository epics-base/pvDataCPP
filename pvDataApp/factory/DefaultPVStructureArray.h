/*DefaultPVStructureArray.h*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef DEFAULTPVSTRUCTUREARRAY_H
#define DEFAULTPVSTRUCTUREARRAY_H
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <pv/pvData.h>
#include <pv/convert.h>
#include <pv/factory.h>
#include <pv/serializeHelper.h>

namespace epics { namespace pvData {

class BasePVStructureArray : public PVStructureArray {
public:
    POINTER_DEFINITIONS(PVStructureArray);
    typedef PVStructurePtr* pointer;
    typedef std::vector<PVStructurePtr> vector;
    typedef std::tr1::shared_ptr<vector> shared_vector;

    BasePVStructureArray(PVStructure *parent,
         StructureArrayConstPtr structureArray);
    virtual ~BasePVStructureArray();
    virtual StructureArrayConstPtr getStructureArray();
    virtual std::size_t append(std::size_t number);
    virtual bool remove(std::size_t offset,std::size_t number);
    virtual void compress();
    virtual void setCapacity(std::size_t capacity);
    virtual std::size_t get(std::size_t offset, std::size_t length,
        StructureArrayData &data);
    virtual std::size_t put(std::size_t offset,std::size_t length,
        vector const & from, std::size_t fromOffset);
    virtual void shareData(
        shared_vector const & value,
        std::size_t capacity,
        std::size_t length);
    virtual pointer getRaw();
    virtual pointer getRaw() const;
    virtual vector const & getVector(){return *value.get();}
    virtual shared_vector const & getSharedVector(){return value;};

    virtual void serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher) const;
    virtual void deserialize(ByteBuffer *buffer,
        DeserializableControl *pflusher);
    virtual void serialize(ByteBuffer *pbuffer,
        SerializableControl *pflusher, std::size_t offset, std::size_t count) const ;
private:
    StructureArrayConstPtr structureArray;
    shared_vector value;
};
}}

#endif /*DEFAULTPVSTRUCTUREARRAY_H*/
