/* serialize.h */
#ifndef SERIALIZE_H
#define SERIALIZE_H
#include "bitSet.h"
#include "byteBuffer.h"
namespace epics { namespace pvData { 


    class SerializableControl {
        virtual void flushSerializeBuffer() const =0;
        virtual void ensureBuffer(int size) const =0;
    };

    class DeserializableControl {
        virtual void ensureData(int size) =0;
    };

    class Serializable {
    public:
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher) const = 0;
        virtual void deserialize(ByteBuffer *pbuffer,
            DeserializableControl *pflusher) = 0;
    };

    class BitSetSerializable {
    public:
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher,BitSet *pbitSet) const = 0;
        virtual void deserialize(ByteBuffer *pbuffer,
            DeserializableControl*pflusher,BitSet *pbitSet) = 0;
    };


    class SerializableArray : public Serializable {
    public:
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher, int offset, int count) const = 0;
    };

}}
#endif  /* SERIALIZE_H */
