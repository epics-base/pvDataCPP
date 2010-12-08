/* bitSet.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include "string.h"
#include "stdio.h"
#include "bitSet.h"

namespace epics { namespace pvData {

    BitSet::BitSet() : words(0), wordsLength(0), wordsInUse(0) {
        initWords(BITS_PER_WORD);
    }

    BitSet::BitSet(uint32 nbits) : words(0), wordsLength(0), wordsInUse(0) {
        initWords(nbits);
    }

    BitSet::~BitSet() {
        delete words;
    }

    void BitSet::initWords(uint32 nbits) {
        uint32 length = (nbits <= 0) ? 1 : wordIndex(nbits-1) + 1;
        if (words) delete words;
        words = new uint64[length];
        bzero(words, sizeof(uint64)*length);
        wordsLength = length;
    }

    void BitSet::recalculateWordsInUse() {
        // wordsInUse is unsigned
        if (wordsInUse == 0)
            return;

        // Traverse the bitset until a used word is found
        uint32 i;
        for (i = wordsInUse-1; i >= 0; i--)
            if (words[i] != 0)
                break;

        wordsInUse = i+1; // The new logical size
    }

    void BitSet::ensureCapacity(uint32 wordsRequired) {
        if (wordsLength < wordsRequired) {

            // create and copy
            uint64* newwords = new uint64[wordsRequired];
            bzero(newwords, sizeof(uint64)*wordsRequired);
            memcpy(newwords, words, sizeof(uint64)*wordsLength);
            if (words) delete words;
            words = newwords;
            wordsLength = wordsRequired;
        }
    }

    void BitSet::expandTo(uint32 wordIndex) {
        uint32 wordsRequired = wordIndex+1;
        if (wordsInUse < wordsRequired) {
            ensureCapacity(wordsRequired);
            wordsInUse = wordsRequired;
        }
    }

    void BitSet::flip(uint32 bitIndex) {

        uint32 wordIdx = wordIndex(bitIndex);
        expandTo(wordIdx);

        words[wordIdx] ^= (((uint64)1) << (bitIndex % BITS_PER_WORD));

        recalculateWordsInUse();
    }

    void BitSet::set(uint32 bitIndex) {

        uint32 wordIdx = wordIndex(bitIndex);
        expandTo(wordIdx);

        words[wordIdx] |= (((uint64)1) << (bitIndex % BITS_PER_WORD));
    }

    void BitSet::clear(uint32 bitIndex) {

        uint32 wordIdx = wordIndex(bitIndex);
        if (wordIdx >= wordsInUse)
            return;

        words[wordIdx] &= ~(((uint64)1) << (bitIndex % BITS_PER_WORD));

        recalculateWordsInUse();
    }

    void BitSet::set(uint32 bitIndex, bool value) {
        if (value)
            set(bitIndex);
        else
            clear(bitIndex);
    }

    bool BitSet::get(uint32 bitIndex) const {
        uint32 wordIdx = wordIndex(bitIndex);
        return ((wordIdx < wordsInUse)
            && ((words[wordIdx] & (((uint64)1) << (bitIndex % BITS_PER_WORD))) != 0));
    }

    void BitSet::clear() {
        while (wordsInUse > 0)
            words[--wordsInUse] = 0;
    }

    uint32 BitSet::numberOfTrailingZeros(uint64 i) {
        // HD, Figure 5-14
        uint32 x, y;
        if (i == 0) return 64;
        uint32 n = 63;
        y = (uint32)i; if (y != 0) { n = n -32; x = y; } else x = (uint32)(i>>32);
        y = x <<16; if (y != 0) { n = n -16; x = y; }
        y = x << 8; if (y != 0) { n = n - 8; x = y; }
        y = x << 4; if (y != 0) { n = n - 4; x = y; }
        y = x << 2; if (y != 0) { n = n - 2; x = y; }
        return n - ((x << 1) >> 31);
    }

    uint32 BitSet::bitCount(uint64 i) {
        // HD, Figure 5-14
        i = i - ((i >> 1) & 0x5555555555555555LL);
        i = (i & 0x3333333333333333LL) + ((i >> 2) & 0x3333333333333333LL);
        i = (i + (i >> 4)) & 0x0f0f0f0f0f0f0f0fLL;
        i = i + (i >> 8);
        i = i + (i >> 16);
        i = i + (i >> 32);
        return (uint32)(i & 0x7f);
     }

    int32 BitSet::nextSetBit(uint32 fromIndex) const {

        uint32 u = wordIndex(fromIndex);
        if (u >= wordsInUse)
            return -1;

        uint64 word = words[u] & (WORD_MASK << (fromIndex % BITS_PER_WORD));

        while (true) {
            if (word != 0)
                return (u * BITS_PER_WORD) + numberOfTrailingZeros(word);
            if (++u == wordsInUse)
                return -1;
            word = words[u];
        }
    }

    int32 BitSet::nextClearBit(uint32 fromIndex) const {
        // Neither spec nor implementation handle bitsets of maximal length.

        uint32 u = wordIndex(fromIndex);
        if (u >= wordsInUse)
            return fromIndex;

        uint64 word = ~words[u] & (WORD_MASK << (fromIndex % BITS_PER_WORD));

        while (true) {
            if (word != 0)
                return (u * BITS_PER_WORD) + numberOfTrailingZeros(word);
            if (++u == wordsInUse)
                return wordsInUse * BITS_PER_WORD;
            word = ~words[u];
        }
    }

    bool BitSet::isEmpty() const {
        return (wordsInUse == 0);
    }

    uint32 BitSet::cardinality() const {
        uint32 sum = 0;
        for (uint32 i = 0; i < wordsInUse; i++)
            sum += bitCount(words[i]);
        return sum;
    }

    BitSet& BitSet::operator&=(const BitSet& set) {

        while (wordsInUse > set.wordsInUse)
            words[--wordsInUse] = 0;

        // Perform logical AND on words in common
        for (uint32 i = 0; i < wordsInUse; i++)
            words[i] &= set.words[i];

        recalculateWordsInUse();

        return *this;
    }

    BitSet& BitSet::operator|=(const BitSet& set) {

        uint32 wordsInCommon;
        if (wordsInUse < set.wordsInUse) {
            wordsInCommon = wordsInUse;
            //ensureCapacity(set.wordsInUse);
            //wordsInUse = set.wordsInUse;
        }
        else
            wordsInCommon = set.wordsInUse;

        // Perform logical OR on words in common
        uint32 i = 0;
        for (; i < wordsInCommon; i++)
            words[i] |= set.words[i];

        // TODO what to do if BitSets are not the same size !!!

        // recalculateWordsInUse() is not needed

        return *this;
    }

    BitSet& BitSet::operator^=(const BitSet& set) {

        uint32 wordsInCommon;
        if (wordsInUse < set.wordsInUse) {
            wordsInCommon = wordsInUse;
            //ensureCapacity(set.wordsInUse);
            //wordsInUse = set.wordsInUse;
        }
        else
            wordsInCommon = set.wordsInUse;

        // Perform logical XOR on words in common
        uint32 i = 0;
        for (; i < wordsInCommon; i++)
            words[i] ^= set.words[i];

        // TODO what to do if BitSets are not the same size !!!

        recalculateWordsInUse();

        return *this;
    }

    BitSet& BitSet::operator-=(const BitSet& set) {

        uint32 wordsInCommon;
        if (wordsInUse < set.wordsInUse) {
            wordsInCommon = wordsInUse;
            //ensureCapacity(set.wordsInUse);
            //wordsInUse = set.wordsInUse;
        }
        else
            wordsInCommon = set.wordsInUse;

        // Perform logical (a & !b) on words in common
        uint32 i = 0;
        for (; i < wordsInCommon; i++)
            words[i] &= ~set.words[i];

        recalculateWordsInUse();

        return *this;
    }

    BitSet& BitSet::operator=(const BitSet &set) {
        // Check for self-assignment!
        if (this == &set)
            return *this;

        // we ensure that words array size is adequate (and not wordsInUse to ensure capacity to the future)
        if (wordsLength < set.wordsLength)
        {
            if (words) delete words;
            words = new uint64[set.wordsLength];
            wordsLength = set.wordsLength;
        }
        memcpy(words, set.words, sizeof(uint64)*set.wordsInUse);
        wordsInUse = set.wordsInUse;

        return *this;
    }

    void BitSet::or_and(const BitSet& set1, const BitSet& set2) {
        uint32 inUse = (set1.wordsInUse < set2.wordsInUse) ? set1.wordsInUse : set2.wordsInUse;

        ensureCapacity(inUse);
        wordsInUse = inUse;

        // Perform logical AND on words in common
        for (uint32 i = 0; i < inUse; i++)
            words[i] |= (set1.words[i] & set2.words[i]);

        // recalculateWordsInUse()...
    }

    bool BitSet::operator==(const BitSet &set) const
    {
        if (this == &set)
            return true;

        if (wordsInUse != set.wordsInUse)
            return false;

        // Check words in use by both BitSets
        for (uint32 i = 0; i < wordsInUse; i++)
            if (words[i] != set.words[i])
                return false;

        return true;
    }

    bool BitSet::operator!=(const BitSet &set) const
    {
        return !(*this == set);
    }

    void BitSet::toString(StringBuilder buffer) { toString(buffer, 0); }

    void BitSet::toString(StringBuilder buffer, int indentLevel) const
    {
        *buffer += '{';
        int32 i = nextSetBit(0);
        char tmp[30];
        if (i != -1) {
            sprintf(tmp,"%d",i); *buffer += tmp;
            for (i = nextSetBit(i+1); i >= 0; i = nextSetBit(i+1)) {
                int32 endOfRun = nextClearBit(i);
                do { *buffer += ", "; sprintf(tmp,"%d",i); *buffer += tmp; } while (++i < endOfRun);
            }
        }
        *buffer += '}';
    }


    /*

void serialize(ByteBuffer buffer, SerializableControl flusher) {

    final int n = wordsInUse;
    if (n == 0) {
        SerializeHelper.writeSize(0, buffer, flusher);
        return;
    }
    int len = 8 * (n-1);
    for (long x = words[n - 1]; x != 0; x >>>= 8)
        len++;

    SerializeHelper.writeSize(len, buffer, flusher);
    flusher.ensureBuffer(len);

    for (int i = 0; i < n - 1; i++)
        buffer.putLong(words[i]);

    for (long x = words[n - 1]; x != 0; x >>>= 8)
        buffer.put((byte) (x & 0xff));
}

public void deserialize(ByteBuffer buffer, DeserializableControl control) {

    final int bytes = SerializeHelper.readSize(buffer, control);	// in bytes

    wordsInUse = (bytes + 7) / 8;
    if (wordsInUse > words.length)
        words = new long[wordsInUse];

    if (wordsInUse == 0)
        return;

    control.ensureData(bytes);

    int i = 0;
    final int longs = bytes / 8;
    while (i < longs)
        words[i++] = buffer.getLong();

    for (int j = i; j < wordsInUse; j++)
        words[j] = 0;

    for (int remaining = (bytes - longs * 8), j = 0; j < remaining; j++)
        words[i] |= (buffer.get() & 0xffL) << (8 * j);

}
     */

}};
