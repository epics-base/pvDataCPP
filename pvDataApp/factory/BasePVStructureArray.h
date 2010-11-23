/*BasePVStructureArray.h*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef BASEPVSTRUCTUREARRAY_H
#define BASEPVSTRUCTUREARRAY_H
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "pvData.h"
#include "factory.h"
#include "serializeHelper.h"

namespace epics { namespace pvData {

    PVStructureArray::PVStructureArray(PVStructure *parent,
        StructureArrayConstPtr structureArray)
    : PVArray(parent,structureArray)
    {}

    PVStructureArray::~PVStructureArray() {}

    class BasePVStructureArray : public PVStructureArray {
    public:
        BasePVStructureArray(PVStructure *parent,
             StructureArrayConstPtr structureArray);
        virtual ~BasePVStructureArray();
        virtual StructureArrayConstPtr getStructureArray();
        virtual void setCapacity(int capacity);
        virtual int get(int offset, int length,
            StructureArrayData *data);
        virtual int put(int offset,int length,
            PVStructurePtrArray from, int fromOffset);
        virtual bool operator==(PVField &pv);
        virtual bool operator!=(PVField &pv);
        virtual void shareData( PVStructurePtrArray value,int capacity,int length);
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher);
        virtual void deserialize(ByteBuffer *buffer,
            DeserializableControl *pflusher);
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher, int offset, int count);
    private:
        StructureArrayConstPtr structureArray;
        StructureArrayData *structureArrayData;
        PVStructurePtrArray value;
    };


    BasePVStructureArray::BasePVStructureArray(
        PVStructure *parent,StructureArrayConstPtr structureArray)
    : PVStructureArray(parent,structureArray),
      structureArray(structureArray),
      structureArrayData(new StructureArrayData()),
      value(new PVStructurePtr[0])
     {
        structureArray->incReferenceCount();
     }

    BasePVStructureArray::~BasePVStructureArray()
    {
        structureArray->decReferenceCount();
        delete structureArrayData;
        int length = getLength();
        for(int i=0; i<length; i++) {
           if(value[i]!=0) delete value[i];
        }
        delete[] value;
    }

    void BasePVStructureArray::setCapacity(int capacity) {
        if(getCapacity()==capacity) return;
        if(!isCapacityMutable()) {
            std::string message("not capacityMutable");
            PVField::message(message, errorMessage);
            return;
        }
        int length = PVArray::getLength();
        PVStructurePtrArray  newValue = new PVStructurePtr[capacity];
        int limit = length;
        if(length>capacity) limit = capacity;
        for(int i=0; i<limit; i++) newValue[i] = value[i];
        for(int i=limit; i<capacity; i++) newValue[i] = 0;
        if(length>capacity) length = capacity;
        delete[] value;
        value = newValue;
        setCapacityLength(capacity,length);
    }


    StructureArrayConstPtr BasePVStructureArray::getStructureArray()
    {
        return structureArray;
    }

    int BasePVStructureArray::get(
        int offset, int len, StructureArrayData *data)
    {
        int n = len;
        int length = getLength();
        if(offset+len > length) {
            n = length - offset;
            if(n<0) n = 0;
        }
        data->data = value;
        data->offset = offset;
        return n;
    }

    int BasePVStructureArray::put(int offset,int len,
        PVStructurePtrArray  from, int fromOffset)
    {
        if(isImmutable()) {
            message(String("field is immutable"), errorMessage);
            return 0;
        }
        if(from==value) return len;
        if(len<1) return 0;
        int length = getLength();
        int capacity = getCapacity();
        if(offset+len > length) {
            int newlength = offset + len;
            if(newlength>capacity) {
                setCapacity(newlength);
                capacity = getCapacity();
                newlength = capacity;
                len = newlength - offset;
                if(len<=0) return 0;
            }
            length = newlength;
            setLength(length);
        }
        StructureConstPtr structure = structureArray->getStructure();
        for(int i=0; i<len; i++) {
        	PVStructurePtr frompv = from[i+fromOffset];
        	if(frompv==0) {
        		value[i+offset] = 0;
        		continue;
        	}
        	if(frompv->getStructure()!=structure) {
                     throw std::invalid_argument(String(
                       "Element is not a compatible structure"));
        	}
        	value[i+offset] = frompv;
        }
        postPut();
        return len;
    }

    void BasePVStructureArray::shareData(
        PVStructurePtrArray value,int capacity,int length)
    {
        this->value = value;
        setCapacity(capacity);
        setLength(length);
    }

    void BasePVStructureArray::serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher) {
        serialize(pbuffer, pflusher, 0, getLength());
    }

    void BasePVStructureArray::deserialize(ByteBuffer *pbuffer,
            DeserializableControl *pcontrol) {
        int size = SerializeHelper::readSize(pbuffer, pcontrol);
        if(size>=0) {
            // prepare array, if necessary
            if(size>getCapacity()) setCapacity(size);
            for(int i = 0; i<size; i++) {
                pcontrol->ensureData(1);
                int8 temp = pbuffer->getByte();
                if(temp==0) {
                    value[i] = NULL;
                }
                else {
                    if(value[i]==NULL) {
                        value[i] = getPVDataCreate()->createPVStructure(
                                NULL, structureArray->getStructure());
                    }
                    value[i]->deserialize(pbuffer, pcontrol);
                }
            }
            setLength(size);
            postPut();
        }
    }

    void BasePVStructureArray::serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher, int offset, int count) {
        // cache
        int length = getLength();

        // check bounds
        if(offset<0)
            offset = 0;
        else if(offset>length) offset = length;
        if(count<0) count = length;

        int maxCount = length-offset;
        if(count>maxCount) count = maxCount;

        // write
        SerializeHelper::writeSize(count, pbuffer, pflusher);
        for(int i = 0; i<count; i++) {
            if(pbuffer->getRemaining()<1) pflusher->flushSerializeBuffer();
            PVStructure* pvStructure = value[i+offset];
            if(pvStructure==NULL) {
                pbuffer->putByte(0);
            }
            else {
                pbuffer->putByte(1);
                pvStructure->serialize(pbuffer, pflusher);
            }
        }
    }

    bool BasePVStructureArray::operator==(PVField &pvField)
    {
        return getConvert()->equals(this,&pvField);
    }

    bool BasePVStructureArray::operator!=(PVField &pvField)
    {
        return !(getConvert()->equals(this,&pvField));
    }

}}
#endif  /* BASEPVSTRUCTUREARRAY_H */
