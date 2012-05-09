/* bitSet.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef BITSET_H
#define BITSET_H
#include <stdexcept>
#include <pv/pvType.h>
#include <pv/serialize.h>
#include <pv/sharedPtr.h>

namespace epics { namespace pvData { 

    /**
     * This class implements a vector of bits that grows as needed. Each
     * component of the bit set has a {@code bool} value. The
     * bits of a {@code BitSet} are indexed by nonnegative integers.
     * Individual indexed bits can be examined, set, or cleared. One
     * {@code BitSet} may be used to modify the contents of another
     * {@code BitSet} through logical AND, logical inclusive OR, and
     * logical exclusive OR operations.
     *
     * <p>By default, all bits in the set initially have the value
     * {@code false}.
     *
     * <p>Every bit set has a current size, which is the number of bits
     * of space currently in use by the bit set. Note that the size is
     * related to the implementation of a bit set, so it may change with
     * implementation. The length of a bit set relates to logical length
     * of a bit set and is defined independently of implementation.
     *
     * <p>A {@code BitSet} is not safe for multithreaded use without
     * external synchronization.
     *
     * Based on Java implementation.
     */
    class BitSet;
    typedef std::tr1::shared_ptr<BitSet> BitSetPtr;
    class BitSet : public Serializable {
    public:
        POINTER_DEFINITIONS(BitSet);
        static BitSetPtr create(uint32 nbits);
        /**
         * Creates a new bit set. All bits are initially {@code false}.
         */
        BitSet();

        /**
         * Creates a bit set whose initial size is large enough to explicitly
         * represent bits with indices in the range {@code 0} through
         * {@code nbits-1}. All bits are initially {@code false}.
         *
         * @param  nbits the initial size of the bit set
         */
        BitSet(uint32 nbits);

        /**
         * Destructor.
         */
        virtual ~BitSet();

        /**
         * Sets the bit at the specified index to the complement of its
         * current value.
         *
         * @param  bitIndex the index of the bit to flip
         */
        void flip(uint32 bitIndex);

        /**
         * Sets the bit at the specified index to {@code true}.
         *
         * @param  bitIndex a bit index
         */
        void set(uint32 bitIndex);

        /**
         * Sets the bit specified by the index to {@code false}.
         *
         * @param  bitIndex the index of the bit to be cleared
         */
        void clear(uint32 bitIndex);

        /**
         * Sets the bit at the specified index to the specified value.
         *
         * @param  bitIndex a bit index
         * @param  value a boolean value to set
         */
        void set(uint32 bitIndex, bool value);

        /**
         * Returns the value of the bit with the specified index. The value
         * is {@code true} if the bit with the index {@code bitIndex}
         * is currently set in this {@code BitSet}; otherwise, the result
         * is {@code false}.
         *
         * @param  bitIndex   the bit index
         * @return the value of the bit with the specified index
         */
        bool get(uint32 bitIndex) const;

        /**
         * Sets all of the bits in this BitSet to {@code false}.
         */
        void clear();

        /**
         * Returns the index of the first bit that is set to {@code true}
         * that occurs on or after the specified starting index. If no such
         * bit exists then {@code -1} is returned.
         *
         * <p>To iterate over the {@code true} bits in a {@code BitSet},
         * use the following loop:
         *
         *  <pre> {@code
         * for (int i = bs.nextSetBit(0); i >= 0; i = bs.nextSetBit(i+1)) {
         *     // operate on index i here
         * }}</pre>
         *
         * @param  fromIndex the index to start checking from (inclusive)
         * @return the index of the next set bit, or {@code -1} if there
         *         is no such bit
         */
        int32 nextSetBit(uint32 fromIndex) const;

        /**
         * Returns the index of the first bit that is set to {@code false}
         * that occurs on or after the specified starting index.
         *
         * @param  fromIndex the index to start checking from (inclusive)
         * @return the index of the next clear bit
         */
        int32 nextClearBit(uint32 fromIndex) const;

        /**
         * Returns true if this {@code BitSet} contains no bits that are set
         * to {@code true}.
         *
         * @return indicating whether this {@code BitSet} is empty
         */
        bool isEmpty() const;

        /**
         * Returns the number of bits set to {@code true} in this {@code BitSet}.
         *
         * @return the number of bits set to {@code true} in this {@code BitSet}
         */
        uint32 cardinality() const;

        /**
         * Performs a logical <b>AND</b> of this target bit set with the
         * argument bit set. This bit set is modified so that each bit in it
         * has the value {@code true} if and only if it both initially
         * had the value {@code true} and the corresponding bit in the
         * bit set argument also had the value {@code true}.
         *
         * @param set a bit set
         */
        BitSet& operator&=(const BitSet& set);

        /**
         * Performs a logical <b>OR</b> of this bit set with the bit set
         * argument. This bit set is modified so that a bit in it has the
         * value {@code true} if and only if it either already had the
         * value {@code true} or the corresponding bit in the bit set
         * argument has the value {@code true}.
         *
         * @param set a bit set
         */
        BitSet& operator|=(const BitSet& set);

        /**
         * Performs a logical <b>XOR</b> of this bit set with the bit set
         * argument. This bit set is modified so that a bit in it has the
         * value {@code true} if and only if one of the following
         * statements holds:
         * <ul>
         * <li>The bit initially has the value {@code true}, and the
         *     corresponding bit in the argument has the value {@code false}.
         * <li>The bit initially has the value {@code false}, and the
         *     corresponding bit in the argument has the value {@code true}.
         * </ul>
         *
         * @param  set a bit set
         */
        BitSet& operator^=(const BitSet& set);

        /**
         * Clears all of the bits in this {@code BitSet} whose corresponding
         * bit is set in the specified {@code BitSet}.
         *
         * @param  set the {@code BitSet} with which to mask this
         *         {@code BitSet}
         */
        BitSet& operator-=(const BitSet& set);

        /**
          * Assigment operator.
          */
        BitSet& operator=(const BitSet &set);

        /**
         * Perform AND operation on <code>set1</code> and <code>set2</code>,
         * and OR on result and this instance.
         * @param set1
         * @param set2
         */
        void or_and(const BitSet& set1, const BitSet& set2);

        /**
         * Comparison operator.
         */
        bool operator==(const BitSet &set) const;

        bool operator!=(const BitSet &set) const;

        void toString(StringBuilder buffer, int indentLevel = 0) const;

        virtual void serialize(ByteBuffer *buffer,
            SerializableControl *flusher) const;
        virtual void deserialize(ByteBuffer *buffer,
            DeserializableControl *flusher);

    private:

        /*
         * BitSets are packed into arrays of "words."  Currently a word is
         * a long, which consists of 64 bits, requiring 6 address bits.
         * The choice of word size is determined purely by performance concerns.
         */
        static const uint32 ADDRESS_BITS_PER_WORD = 6;
        static const uint32 BITS_PER_WORD = 1 << ADDRESS_BITS_PER_WORD;
        static const uint32 BIT_INDEX_MASK = BITS_PER_WORD - 1;

        /** Used to shift left or right for a partial word mask */
        static const uint64 WORD_MASK = ~((uint64)0);

        /** The internal field corresponding to the serialField "bits". */
        uint64* words;

        /** The internal field corresponding to the size of words[] array. */
        uint32 wordsLength;

        /** The number of words in the logical size of this BitSet. */
        uint32 wordsInUse;


    private:

        /**
         * Given a bit index, return word index containing it.
         */
        static inline uint32 wordIndex(uint32 bitIndex) {
            return bitIndex >> ADDRESS_BITS_PER_WORD;
        }

        /**
         * Creates a new word array.
         */
        void initWords(uint32 nbits);

        /**
         * Sets the field wordsInUse to the logical size in words of the bit set.
         * WARNING: This method assumes that the number of words actually in use is
         * less than or equal to the current value of wordsInUse!
         */
        void recalculateWordsInUse();

        /**
         * Ensures that the BitSet can hold enough words.
         * @param wordsRequired the minimum acceptable number of words.
         */
        void ensureCapacity(uint32 wordsRequired);

        /**
         * Ensures that the BitSet can accommodate a given wordIndex,
         * temporarily violating the invariants.  The caller must
         * restore the invariants before returning to the user,
         * possibly using recalculateWordsInUse().
         * @param wordIndex the index to be accommodated.
         */
        void expandTo(uint32 wordIndex);

        /**
         * Returns the number of zero bits following the lowest-order ("rightmost")
         * one-bit in the two's complement binary representation of the specified
         * <tt>long</tt> value.  Returns 64 if the specified value has no
         * one-bits in its two's complement representation, in other words if it is
         * equal to zero.
         *
         * @return the number of zero bits following the lowest-order ("rightmost")
         *     one-bit in the two's complement binary representation of the
         *     specified <tt>long</tt> value, or 64 if the value is equal
         *     to zero.
         */
        static uint32 numberOfTrailingZeros(uint64 i);

        /**
         * Returns the number of one-bits in the two's complement binary
         * representation of the specified <tt>long</tt> value.  This function is
         * sometimes referred to as the <i>population count</i>.
         *
         * @return the number of one-bits in the two's complement binary
         *     representation of the specified <tt>long</tt> value.
         */
         static uint32 bitCount(uint64 i);

    };

}}
#endif  /* BITSET_H */



