/* serialize.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef SERIALIZE_H
#define SERIALIZE_H
#include <pv/byteBuffer.h>
#include <pv/sharedPtr.h>
namespace epics { namespace pvData { 

    class SerializableControl;
    class DeserializableControl;
    class Serializable;
    class BitSetSerializable;
    class SerializableArray;
    class BitSet;
    class Field;

    class SerializableControl {
    public:
        virtual ~SerializableControl(){}
        virtual void flushSerializeBuffer() =0;
        virtual void ensureBuffer(std::size_t size) =0;
        virtual void alignBuffer(int alignment) =0;
        virtual void cachedSerialize(std::tr1::shared_ptr<const Field> const & field, ByteBuffer* buffer) = 0;
    };

    class DeserializableControl {
    public:
        virtual ~DeserializableControl(){}
        virtual void ensureData(std::size_t size) =0;
        virtual void alignData(int alignment) =0;
        virtual std::tr1::shared_ptr<const Field> cachedDeserialize(ByteBuffer* buffer) = 0;
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
            SerializableControl *flusher, std::size_t offset, std::size_t count) const = 0;
    };

}}
#endif  /* SERIALIZE_H */
