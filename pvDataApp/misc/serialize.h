/* serialize.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef SERIALIZE_H
#define SERIALIZE_H
#include "bitSet.h"
#include "byteBuffer.h"
namespace epics { namespace pvData { 

    class SerializableControl;
    class DeserializableControl;
    class Serializable;
    class BitSetSerializable;
    class SerializableArray;

    class SerializableControl {
    public:
        virtual void flushSerializeBuffer() =0;
        virtual void ensureBuffer(int size) =0;
    };

    class DeserializableControl {
    public:
        virtual void ensureData(int size) =0;
    };

    class Serializable {
    public:
        virtual void serialize(ByteBuffer *buffer,
            SerializableControl *flusher) = 0;
        virtual void deserialize(ByteBuffer *buffer,
            DeserializableControl *flusher) = 0;
    };

    class BitSetSerializable {
    public:
        virtual void serialize(ByteBuffer *buffer,
            SerializableControl *flusher,BitSet *bitSet) = 0;
        virtual void deserialize(ByteBuffer *buffer,
            DeserializableControl *flusher,BitSet *bitSet) = 0;
    };


    class SerializableArray : public Serializable {
    public:
        virtual void serialize(ByteBuffer *buffer,
            SerializableControl *flusher, int offset, int count) = 0;
    };

}}
#endif  /* SERIALIZE_H */
