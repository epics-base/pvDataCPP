/* serialize.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <pv/byteBuffer.h>
#include <pv/sharedPtr.h>

#include <shareLib.h>

namespace epics { namespace pvData { 

    class SerializableControl;
    class DeserializableControl;
    class Serializable;
    class BitSetSerializable;
    class SerializableArray;
    class BitSet;
    class Field;

    /**
     * @brief Callback class for serialization.
     *
     * This must be provided by code that calls serialize.
     */
    class epicsShareClass SerializableControl {
    public:
        /**
         * Destructor.
         */
        virtual ~SerializableControl(){}
        /**
         * Done with this buffer. Flush it.
         */
        virtual void flushSerializeBuffer() =0;
        /**
         * Make sure buffer has at least size bytes remaining.
         * If not flush existing buffer and provide a new one.
         * @param size The number of bytes.
         */
        virtual void ensureBuffer(std::size_t size) =0;
        /**
         * Add pad bytes to buffer.
         * @param alignment allignment required.
         */
        virtual void alignBuffer(std::size_t alignment) =0;
        /**
         * method for serializing array data.
         * This should only be used for arrays of primitive types.
         * i. e. boolean,byte,...,double.
         * It can not be called for string, structure, or union arrays.
         * @param existingBuffer the existing buffer from the caller.
         * @param toSerialize location of data to be put into buffer.
         * @param elementCount number of elements.
         * @param elementSize element size.
         */
        virtual bool directSerialize(
            ByteBuffer *existingBuffer,
            const char* toSerialize,
            std::size_t elementCount,
            std::size_t elementSize) = 0;
        /**
         * serialize via cache
         * @param field instance to be serialized
         * @param buffer buffer to be serialized to
         */
        virtual void cachedSerialize(
            std::tr1::shared_ptr<const Field> const & field,
            ByteBuffer* buffer) = 0;
    };

    /**
     * @brief Callback class for deserialization.
     *
     * This must be provided by code that calls deserialize.
     */
    class epicsShareClass DeserializableControl {
    public:
        /**
         * Destructor.
         */
        virtual ~DeserializableControl(){}
        /**
         * Helper method.
         * Ensures specified size of bytes, provides it if necessary.
         * @param size The number of bytes.
         */
        virtual void ensureData(std::size_t size) =0;
        /**
         * Align buffer.
         * Note that this takes care only current buffer alignment.
         * If streaming protocol is used,
         * care must be taken that entire stream is aligned.
         * @param alignment size in bytes, must be power of two.
         */
        virtual void alignData(std::size_t alignment) =0;
        /**
         * method for deserializing array data.
         * This should only be used for arrays of primitive types.
         * i. e. boolean,byte,...,double.
         * It can not be called for string, structure, or union arrays.
         * @param existingBuffer the existing buffer from the caller.
         * @param deserializeTo location of data.
         * @param elementCount number of elements.
         * @param elementSize element size.
         */
        virtual bool directDeserialize(
            ByteBuffer *existingBuffer,
            char* deserializeTo,
            std::size_t elementCount,
            std::size_t elementSize) = 0;
        /**
         * deserialize via cache
         * @param field instance to be deserialized
         * @param buffer buffer to be deserialized from
         */
        virtual std::tr1::shared_ptr<const Field> cachedDeserialize(
            ByteBuffer* buffer) = 0;
    };

    /**
     * @brief Base class for serialization.
     *
     */
    class epicsShareClass Serializable {
    public:
        /**
         * Destructor.
         */
        virtual ~Serializable(){}
        /**
         * Serialize field into given buffer.
         * @param buffer serialization buffer.
         * @param flusher flush interface.
         */
        virtual void serialize(ByteBuffer *buffer,
            SerializableControl *flusher) const = 0;
	/**
         * Deserialize buffer.
         * @param buffer serialization buffer.
         * @param flusher deserialization control.
         */
        virtual void deserialize(ByteBuffer *buffer,
            DeserializableControl *flusher) = 0;
    };


    /**
     * @brief Class for serializing bitSets.
     *
     */
    class epicsShareClass BitSetSerializable {
    public:
        /**
         * Destructor.
         */
        virtual ~BitSetSerializable(){}
        /**
         * Serialize field into given buffer.
         * @param buffer serialization buffer.
         * @param flusher flush interface.
         * &param bitSet The bitSet to serialize.
         */
        virtual void serialize(ByteBuffer *buffer,
            SerializableControl *flusher,BitSet *bitSet) const = 0;
	/**
         * Deserialize buffer.
         * @param buffer serialization buffer.
         * @param flusher deserialization control.
         * &param bitSet The bitSet to deserialize.
         */
        virtual void deserialize(ByteBuffer *buffer,
            DeserializableControl *flusher,BitSet *bitSet) = 0;
    };


    /**
     * @brief Class for serializing arrays.
     *
     */
    class epicsShareClass SerializableArray : public virtual Serializable {
    public:
        /**
         * Destructor.
         */
        virtual ~SerializableArray(){}
        using Serializable::serialize;
        /**
         * Serialize field into given buffer.
         * @param buffer serialization buffer.
         * @param flusher flush interface.
         * &param offset offset in elements.
         * @param count number of elements
         */
        virtual void serialize(
            ByteBuffer *buffer,
            SerializableControl *flusher,
            std::size_t offset,
            std::size_t count) const = 0;
    };

}}
#endif  /* SERIALIZE_H */
