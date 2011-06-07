/* serialize.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef SERIALIZE_H
#define SERIALIZE_H
#include <pv/byteBuffer.h>
namespace epics { namespace pvData { 

    class SerializableControl;
    class DeserializableControl;
    class Serializable;
    class BitSetSerializable;
    class SerializableArray;
    class BitSet;

    class SerializableControl {
    public:
        virtual ~SerializableControl(){}
        virtual void flushSerializeBuffer() =0;
        virtual void ensureBuffer(int size) =0;
    };

    class DeserializableControl {
    public:
        virtual ~DeserializableControl(){}
        virtual void ensureData(int size) =0;
    };

    class Serializable {
    public:
        virtual ~Serializable(){}
        virtual void serialize(ByteBuffer *buffer,
            SerializableControl *flusher) const = 0;
        virtual void deserialize(ByteBuffer *buffer,
            DeserializableControl *flusher) = 0;
    };

    class BitSetSerializable {
    public:
        virtual ~BitSetSerializable(){}
        virtual void serialize(ByteBuffer *buffer,
            SerializableControl *flusher,BitSet *bitSet) const = 0;
        virtual void deserialize(ByteBuffer *buffer,
            DeserializableControl *flusher,BitSet *bitSet) = 0;
    };


    class SerializableArray : virtual public Serializable {
    public:
        virtual ~SerializableArray(){}
        virtual void serialize(ByteBuffer *buffer,
            SerializableControl *flusher, int offset, int count) const = 0;
    };

}}
#endif  /* SERIALIZE_H */
