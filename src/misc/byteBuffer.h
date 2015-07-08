/* byteBuffer.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mse
 */
#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H

#include <string>
#include <string.h>

#ifdef epicsExportSharedSymbols
#define byteBufferepicsExportSharedSymbols
#undef epicsExportSharedSymbols
#endif

#include <epicsEndian.h>

#ifdef byteBufferepicsExportSharedSymbols
#define epicsExportSharedSymbols
#undef byteBufferepicsExportSharedSymbols
#endif

#include <pv/pvType.h>
#include <pv/epicsException.h>

#include <shareLib.h>

namespace epics { 
    namespace pvData {
        
/*
TODO can be used:

MS Visual C++:

You include intrin.h and call the following functions:

For 16 bit numbers:
unsigned short _byteswap_ushort(unsigned short value);

For 32 bit numbers:
unsigned long _byteswap_ulong(unsigned long value);

For 64 bit numbers:
unsigned __int64 _byteswap_uint64(unsigned __int64 value);
*/

/*
For floats and doubles it's more difficult as with plain integers as these may or not may be in the host machines byte-order.
You can get little-endian floats on big-endian machines and vice versa.
*/



#define GCC_VERSION_SINCE(major, minor, patchlevel) \
  (defined(__GNUC__) && !defined(__INTEL_COMPILER) && \
   ((__GNUC__ > (major)) ||  \
    (__GNUC__ == (major) && __GNUC_MINOR__ > (minor)) || \
    (__GNUC__ == (major) && __GNUC_MINOR__ == (minor) && __GNUC_PATCHLEVEL__ >= (patchlevel))))
    
 
#if GCC_VERSION_SINCE(4,3,0)

#define swap32(x) __builtin_bswap32(x)
#define swap64(x) __builtin_bswap64(x)

#define __byte_swap16(x) \
        (((x) >> 8) | \
         ((x) << 8))

static inline uint16_t
swap16(uint16_t _x)
{
    return (__byte_swap16(_x));
}

#else


#define __byte_swap16(x) \
        (((x) >> 8) | \
         ((x) << 8))

#define __byte_swap32(x) \
        ((((x) & 0xff000000) >> 24) | \
         (((x) & 0x00ff0000) >>  8) | \
         (((x) & 0x0000ff00) <<  8) | \
         (((x) & 0x000000ff) << 24))

#define __byte_swap64(x) \
        (((x) >> 56) | \
        (((x) >> 40) & 0xff00) | \
        (((x) >> 24) & 0xff0000) | \
        (((x) >> 8)  & 0xff000000) | \
        (((x) << 8)  & ((uint64_t)0xff << 32)) | \
        (((x) << 24) & ((uint64_t)0xff << 40)) | \
        (((x) << 40) & ((uint64_t)0xff << 48)) | \
        (((x) << 56)))
   
static inline uint16_t
swap16(uint16_t _x)
{
    return (__byte_swap16(_x));
}

static inline uint32_t
swap32(uint32_t _x)
{
    return (__byte_swap32(_x));
}

static inline uint64_t
swap64(uint64_t _x)
{
    return (__byte_swap64(_x));
}

#endif


template<typename T>
inline T swap(T val) { return val; }  // not valid

template<>
inline int16 swap(int16 val)
{
    return swap16(val);
}

template<>
inline int32 swap(int32 val)
{
    return swap32(val);
}

template<>
inline int64 swap(int64 val)
{
    return swap64(val);
}

template<>
inline uint16 swap(uint16 val)
{
    return swap16(val);
}

template<>
inline uint32 swap(uint32 val)
{
    return swap32(val);
}

template<>
inline uint64 swap(uint64 val)
{
    return swap64(val);
}

template<>
inline float swap(float val)
{
    union {
        int32 i;
        float f;
    } conv;
    conv.f = val;
    conv.i = swap32(conv.i);
    return conv.f;
}

template<>
inline double swap(double val)
{
    union {
        int64 i;
        double d;
    } conv;
    conv.d = val;
    conv.i = swap64(conv.i);
    return conv.d;
}

#define is_aligned(POINTER, BYTE_COUNT) \
    (((std::ptrdiff_t)(const void *)(POINTER)) % (BYTE_COUNT) == 0)

/*template <bool ENDIANESS_SUPPORT = false,
          bool UNALIGNED_ACCESS = false,
          bool ADAPTIVE_ACCESS = true,
          bool USE_INLINE_MEMCPY = true>*/

#define ENDIANESS_SUPPORT true
#define UNALIGNED_ACCESS true
#define ADAPTIVE_ACCESS true
#define USE_INLINE_MEMCPY true

#if defined (__GNUC__) && (__GNUC__ < 3)
#define GET(T) get((T*)0)
#else
#define GET(T) get<T>()
#endif

/**
 * @brief This class implements a Bytebuffer that is like the java.nio.ByteBuffer.
 * 
 * <p>A {@code BitSet} is not safe for multithreaded use without
 * external synchronization.
 *
 * Based on Java implementation.
 */
class ByteBuffer 
{
public:
    /**
     * Constructor.
     *
     * @param  size      The number of bytes.
     * @param  byteOrder The byte order.
     * Must be one of EPICS_BYTE_ORDER,EPICS_ENDIAN_LITTLE,EPICS_ENDIAN_BIG.
     */
    ByteBuffer(std::size_t size, int byteOrder = EPICS_BYTE_ORDER) :
        _buffer(0), _size(size),
        _reverseEndianess(byteOrder != EPICS_BYTE_ORDER),
        _reverseFloatEndianess(byteOrder != EPICS_FLOAT_WORD_ORDER),
        _wrapped(false)
    {
        _buffer = (char*)malloc(size);
        clear();
    }

    /**
     * Constructor for wrapping existing buffers.
     * Given buffer will not be released by the ByteBuffer instance.
     * @param  buffer    Existing buffer.
     * @param  size      The number of bytes.
     * @param  byteOrder The byte order.
     * Must be one of EPICS_BYTE_ORDER,EPICS_ENDIAN_LITTLE,EPICS_ENDIAN_BIG.
     */
    ByteBuffer(char* buffer, std::size_t size, int byteOrder = EPICS_BYTE_ORDER) :
        _buffer(buffer), _size(size),
        _reverseEndianess(byteOrder != EPICS_BYTE_ORDER),
        _reverseFloatEndianess(byteOrder != EPICS_FLOAT_WORD_ORDER),
        _wrapped(true)
    {
        clear();
    }
    /**
     * Destructor
     */
    ~ByteBuffer()
    {
        if (_buffer && !_wrapped) free(_buffer);
    }
    /**
     * Set the byte order.
     *
     * @param  byteOrder The byte order.
     * Must be one of EPICS_BYTE_ORDER,EPICS_ENDIAN_LITTLE,EPICS_ENDIAN_BIG,
     */
    inline void setEndianess(int byteOrder)
    {
        _reverseEndianess = (byteOrder != EPICS_BYTE_ORDER);
        _reverseFloatEndianess = (byteOrder != EPICS_FLOAT_WORD_ORDER);
    }
    /**
     * Get the raw buffer data.
     * @return the raw buffer data.
     */
    inline const char* getBuffer()
    {
        return _buffer;
    }
    /**
     * Makes a buffer ready for a new sequence of channel-read or relative put operations:
     * It sets the limit to the capacity and the position to zero.
     */
    inline void clear()
    {
        _position = _buffer;
        _limit = _buffer + _size;
    }
    /**
     * Makes a buffer ready for a new sequence of channel-write or relative get operations:
     * It sets the limit to the current position and then sets the position to zero. 
     */
    inline void flip() {
        _limit = _position;
        _position = _buffer;
    }
    /**
     * Makes a buffer ready for re-reading the data that it already contains:
     * It leaves the limit unchanged and sets the position to zero.
     */
    inline void rewind() {
        _position = _buffer;
    }
    /**
     * Returns the current position.
     * @return The current position in the raw data.
     */
    inline std::size_t getPosition()
    {
        return (std::size_t)(((std::ptrdiff_t)(const void *)_position) - ((std::ptrdiff_t)(const void *)_buffer));
    }
    /**
     * Sets the buffer position.
     * If the mark is defined and larger than the new position then it is discarded.
     *
     * @param  pos The offset into the raw buffer.
     * The new position value; must be no larger than the current limit
     */
    inline void setPosition(std::size_t pos)
    {
        _position = _buffer + pos;
    }
    /**
     * Returns this buffer's limit. 
     *
     * @return The offset into the raw buffer.
     */
    inline std::size_t getLimit()
    {
        return (std::size_t)(((std::ptrdiff_t)(const void *)_limit) - ((std::ptrdiff_t)(const void *)_buffer));
    }
    /**
     * Sets this buffer's limit.
     * If the position is larger than the new limit then it is set to the new limit.s
     * If the mark is defined and larger than the new limit then it is discarded. 
     *
     * @param  limit The new position value;
     * must be no larger than the current limit 
     */
    inline void setLimit(std::size_t limit)
    {
        _limit = _buffer + limit;
    }
    /**
     * Returns the number of elements between the current position and the limit.
     *
     * @return The number of elements remaining in this buffer.
     */
    inline std::size_t getRemaining()
    {
        return (std::size_t)(((std::ptrdiff_t)(const void *)_limit) - ((std::ptrdiff_t)(const void *)_position));
    }
    /**
     * Returns The size, i.e. capacity of the raw data buffer in bytes.
     *
     * @return The size of the raw data buffer.
     */
    inline std::size_t getSize()
    {
        return _size;
    }
    /**
     * Put the value into the raw buffer as a byte stream in the current byte order.
     *
     * @param  value The value to be put into the byte buffer.
     */
    template<typename T>
    inline void put(T value);
    /**
     * Put the value into the raw buffer at the specified index  as a byte stream in the current byte order.
     *
     * @param  index Offset in the byte buffer.
     * @param  value The value to be put into the byte buffer.
     */
    template<typename T>
    inline void put(std::size_t index, T value);
    /**
     * Get the new object from  the byte buffer. The item MUST have type {@code T}.
     * The position is adjusted based on the type.
     *
     * @return The object.
     */
#if defined (__GNUC__) && (__GNUC__ < 3)
    template<typename T>
    inline T get(const T*);
#else
    template<typename T>
    inline T get();
#endif
    /**
     * Get the new object from  the byte buffer at the specified index.
     * The item MUST have type {@code T}.
     * The position is adjusted based on the type.
     *
     * @param index The location in the byte buffer.
     * @return The object.
     */
    template<typename T>
    inline T get(std::size_t index);
    /**
     * Put a sub-array of bytes into the byte buffer.
     * The position is increased by the count.
     *
     * @param  src    The source array.
     * @param  offset The starting position within src.
     * @param  count  The number of bytes to put into the byte buffer,
     */
    inline void put(const char* src, std::size_t src_offset, std::size_t count) {
        //if(count>getRemaining()) THROW_BASE_EXCEPTION("buffer overflow");
        memcpy(_position, src + src_offset, count);
        _position += count;
    }
    /**
     * Get a sub-array of bytes from the byte buffer.
     * The position is increased by the count.
     *
     * @param  dest    The destination array.
     * @param  offset The starting position within src.
     * @param  count  The number of bytes to put into the byte buffer,
     */
    inline void get(char* dest, std::size_t dest_offset, std::size_t count) {
        //if(count>getRemaining()) THROW_BASE_EXCEPTION("buffer overflow");
        memcpy(dest + dest_offset, _position, count);
        _position += count;
    }
    /**
     * Put an array of type {@code T} into the byte buffer.
     * The position is adjusted.
     *
     * @param  values The input array.
     * @param  count  The number of elements.
     */
    template<typename T>
    inline void putArray(const T* values, std::size_t count);
    /**
     * Get an array of type {@code T} from the byte buffer.
     * The position is adjusted.
     *
     * @param  values The destination array.
     * @param  count  The number of elements.
     */
    template<typename T>
    inline void getArray(T* values, std::size_t count);
    /**
     * Is the byte order the EPICS_BYTE_ORDER
     * @return (false,true) if (is, is not) the EPICS_BYTE_ORDER
     */
    template<typename T>
    inline bool reverse()
    {
        return _reverseEndianess;
    }
    /**
     * Adjust position so that it is aligned to the specified size.
     * Size MUST be a power of 2.
     * @param  size The alignment requirement.
     */
    inline void align(std::size_t size)
    {
        const std::size_t k = size - 1;
        _position = (char*)((((std::ptrdiff_t)(const void *)_position) + k) & ~(k));
    }
    /**
     * Put a boolean value into the byte buffer.
     *
     * @param  value The value.
     */
    inline void putBoolean(  bool value) { put<  int8>(value ? 1 : 0); }
    /**
     * Put a byte value into the byte buffer.
     *
     * @param  value The value.
     */
    inline void putByte   (  int8 value) { put<  int8>(value); }
    /**
     * Put a short value into the byte buffer.
     *
     * @param  value The value.
     */
    inline void putShort  ( int16 value) { put< int16>(value); }
    /**
     * Put an int value into the byte buffer.
     *
     * @param  value The value.
     */
    inline void putInt    ( int32 value) { put< int32>(value); }
    /**
     * Put a long value into the byte buffer.
     *
     * @param  value The value.
     */
    inline void putLong   ( int64 value) { put< int64>(value); }
    /**
     * Put a float value into the byte buffer.
     *
     * @param  value The value.
     */
    inline void putFloat  ( float value) { put< float>(value); }
    /**
     * Put a double value into the byte buffer.
     *
     * @param  value The value.
     */
    inline void putDouble (double value) { put<double>(value); }

    /**
     * Put a boolean value into the byte buffer at the specified index.
     *
     * @param  index The offset in the byte buffer,
     * @param  value The value.
     */
    inline void putBoolean(std::size_t  index,  bool value) { put<  int8>(index, value); }
    /**
     * Put a byte value into the byte buffer at the specified index.
     *
     * @param  index The offset in the byte buffer,
     * @param  value The value.
     */
    inline void putByte   (std::size_t  index,  int8 value) { put<  int8>(index, value); }
    /**
     * Put a short value into the byte buffer at the specified index.
     *
     * @param  index The offset in the byte buffer,
     * @param  value The value.
     */
    inline void putShort  (std::size_t  index, int16 value) { put< int16>(index, value); }
    /**
     * Put an int value into the byte buffer at the specified index.
     *
     * @param  index The offset in the byte buffer,
     * @param  value The value.
     */
    inline void putInt    (std::size_t  index, int32 value) { put< int32>(index, value); }
    /**
     * Put a long value into the byte buffer at the specified index.
     *
     * @param  index The offset in the byte buffer,
     * @param  value The value.
     */
    inline void putLong   (std::size_t  index, int64 value) { put< int64>(index, value); }
    /**
     * Put a float value into the byte buffer at the specified index.
     *
     * @param  index The offset in the byte buffer,
     * @param  value The value.
     */
    inline void putFloat  (std::size_t  index, float value) { put< float>(index, value); }
    /**
     * Put a double value into the byte buffer at the specified index.
     *
     * @param  index The offset in the byte buffer,
     * @param  value The value.
     */
    inline void putDouble (std::size_t  index, double value) { put<double>(index, value); }
    /**
     * Get a boolean value from the byte buffer.
     *
     * @return The value.
     */
    inline   bool getBoolean() { return GET(  int8) != 0; }
    /**
     * Get a byte value from the byte buffer.
     *
     * @return The value.
     */
    inline   int8 getByte   () { return GET(  int8); }
    /**
     * Get a short value from the byte buffer.
     *
     * @return The value.
     */
    inline  int16 getShort  () { return GET( int16); }
    /**
     * Get a int value from the byte buffer.
     *
     * @return The value.
     */
    inline  int32 getInt    () { return GET( int32); }
    /**
     * Get a long value from the byte buffer.
     *
     * @return The value.
     */
    inline  int64 getLong   () { return GET( int64); }
    /**
     * Get a float value from the byte buffer.
     *
     * @return The value.
     */
    inline  float getFloat  () { return GET( float); }
    /**
     * Get a double value from the byte buffer.
     *
     * @return The value.
     */
    inline double getDouble () { return GET(double); }
    /**
     * Get a boolean value from the byte buffer at the specified index.
     *
     * @param  index The offset in the byte buffer.
     * @return The value.
     */
    inline   bool getBoolean(std::size_t  index) { return get<  int8>(index) != 0; }
    /**
     * Get a byte value from the byte buffer at the specified index.
     *
     * @param  index The offset in the byte buffer.
     * @return The value.
     */
    inline   int8 getByte   (std::size_t  index) { return get<  int8>(index); }
    /**
     * Get a short value from the byte buffer at the specified index.
     *
     * @param  index The offset in the byte buffer.
     * @return The value.
     */
    inline  int16 getShort  (std::size_t  index) { return get< int16>(index); }
    /**
     * Get an int value from the byte buffer at the specified index.
     *
     * @param  index The offset in the byte buffer.
     * @return The value.
     */
    inline  int32 getInt    (std::size_t  index) { return get< int32>(index); }
    /**
     * Get a long value from the byte buffer at the specified index.
     *
     * @param  index The offset in the byte buffer.
     * @return The value.
     */
    inline  int64 getLong   (std::size_t  index) { return get< int64>(index); }
    /**
     * Get a float value from the byte buffer at the specified index.
     *
     * @param  index The offset in the byte buffer.
     * @return The value.
     */
    inline  float getFloat  (std::size_t  index) { return get< float>(index); }
    /**
     * Get a boolean value from the byte buffer at the specified index.
     *
     * @param  double The offset in the byte buffer.
     * @return The value.
     */
    inline double getDouble (std::size_t  index) { return get<double>(index); }

    // TODO remove
    inline const char* getArray()
    {
        return _buffer;
    }

    
private:
    char* _buffer;
    char* _position;
    char* _limit;
    std::size_t  _size;
    bool _reverseEndianess; 
    bool _reverseFloatEndianess;
    bool _wrapped;
};

    template<>
    inline bool ByteBuffer::reverse<bool>()
    {
        return false;
    }

    template<>
    inline bool ByteBuffer::reverse<int8>()
    {
        return false;
    }

    template<>
    inline bool ByteBuffer::reverse<uint8>()
    {
        return false;
    }

    template<>
    inline bool ByteBuffer::reverse<float>()
    {
        return _reverseFloatEndianess;
    }

    template<>
    inline bool ByteBuffer::reverse<double>()
    {
        return _reverseFloatEndianess;
    }

    // the following methods must come after the specialized reverse<>() methods to make pre-gcc3 happy

    template<typename T>
    inline void ByteBuffer::put(T value)
    {
        // this avoids int8 specialization, compiler will take care if optimization, -O2 or more
        if (sizeof(T) == 1)
        {
            *(_position++) = (int8)value;
            return;
        }

        if (ENDIANESS_SUPPORT && reverse<T>())
        {
            value = swap<T>(value);
        }

        if (UNALIGNED_ACCESS)
        {
            // NOTE: some CPU handle unaligned access pretty good (e.g. x86)
            *((T*)_position) = value;
            _position += sizeof(T);
        }
        else
        {
            // NOTE: this check and branching does not always pay off
            if (ADAPTIVE_ACCESS && is_aligned(_position, sizeof(T)))
            {
                *((T*)_position) = value;
                _position += sizeof(T);
            }
            else
            {
                if (USE_INLINE_MEMCPY)
                {
                    // NOTE: it turns out that this compiler can optimize this with inline code, e.g. gcc
                    memcpy(_position, &value, sizeof(T));
                    _position += sizeof(T);
                }
                else
                {
                    // NOTE: compiler should optimize this and unroll the loop
                    for (size_t i = 0; i < sizeof(T); i++)
                        _position[i] = ((char*)&value)[i];
                    _position += sizeof(T);
                }
            }
        }

    }

    template<typename T>
    inline void ByteBuffer::put(std::size_t index, T value)
    {
        // this avoids int8 specialization, compiler will take care if optimization, -O2 or more
        if (sizeof(T) == 1)
        {
            *(_buffer + index) = (int8)value;
            return;
        }

        if (ENDIANESS_SUPPORT && reverse<T>())
        {
            value = swap<T>(value);
        }

        if (UNALIGNED_ACCESS)
        {
            // NOTE: some CPU handle unaligned access pretty good (e.g. x86)
            *((T*)(_buffer + index)) = value;
        }
        else
        {
            // NOTE: this check and branching does not always pay off
            if (ADAPTIVE_ACCESS && is_aligned(_position, sizeof(T)))
            {
                *((T*)(_buffer + index)) = value;
            }
            else
            {
                if (USE_INLINE_MEMCPY)
                {
                    // NOTE: it turns out that this compiler can optimize this with inline code, e.g. gcc
                    memcpy(_buffer + index, &value, sizeof(T));
                }
                else
                {
                    // NOTE: compiler should optimize this and unroll the loop
                    char *p = _buffer + index;
                    for (size_t i = 0; i < sizeof(T); i++)
                        p[i] = ((char*)&value)[i];
                }
            }
        }

    }

#if defined (__GNUC__) && (__GNUC__ < 3)
    template<typename T>
    inline T ByteBuffer::get(const T*)
#else
    template<typename T>
    inline T ByteBuffer::get()
#endif
    {
        // this avoids int8 specialization, compiler will take care if optimization, -O2 or more
        if (sizeof(T) == 1)
        {
            return (int8)(*(_position++));
        }


        T value;

        if (UNALIGNED_ACCESS)
        {
            // NOTE: some CPU handle unaligned access pretty good (e.g. x86)
            value = *((T*)_position);
            _position += sizeof(T);
        }
        else
        {
            // NOTE: this check and branching does not always pay off
            if (ADAPTIVE_ACCESS && is_aligned(_position, sizeof(T)))
            {
                value = *((T*)_position);
                _position += sizeof(T);
            }
            else
            {
                if (USE_INLINE_MEMCPY)
                {
                    // NOTE: it turns out that this compiler can optimize this with inline code, e.g. gcc
                    memcpy(&value, _position, sizeof(T));
                    _position += sizeof(T);
                }
                else
                {
                    // NOTE: compiler should optimize this and unroll the loop
                    for (size_t i = 0; i < sizeof(T); i++)
                        ((char*)&value)[i] = _position[i];
                    _position += sizeof(T);
                }
            }
        }

        if (ENDIANESS_SUPPORT && reverse<T>())
        {
            value = swap<T>(value);
        }

        return value;
    }

    template<typename T>
    inline T ByteBuffer::get(std::size_t index)
    {
        // this avoids int8 specialization, compiler will take care if optimization, -O2 or more
        if (sizeof(T) == 1)
        {
            return (int8)(*(_buffer + index));
        }


        T value;

        if (UNALIGNED_ACCESS)
        {
            // NOTE: some CPU handle unaligned access pretty good (e.g. x86)
            value = *((T*)(_buffer + index));
        }
        else
        {
            // NOTE: this check and branching does not always pay off
            if (ADAPTIVE_ACCESS && is_aligned(_position, sizeof(T)))
            {
                value = *((T*)(_buffer + index));
            }
            else
            {
                if (USE_INLINE_MEMCPY)
                {
                    // NOTE: it turns out that this compiler can optimize this with inline code, e.g. gcc
                    memcpy(&value, _buffer + index, sizeof(T));
                }
                else
                {
                    // NOTE: compiler should optimize this and unroll the loop
                    char* p = _buffer + index;
                    for (size_t i = 0; i < sizeof(T); i++)
                        ((char*)&value)[i] = p[i];
                }
            }
        }

        if (ENDIANESS_SUPPORT && reverse<T>())
        {
            value = swap<T>(value);
        }

        return value;
    }

    template<typename T>
    inline void ByteBuffer::putArray(const T* values, std::size_t count)
    {
        // this avoids int8 specialization, compiler will take care if optimization, -O2 or more
        if (sizeof(T) == 1)
        {
            put((const char*)values, 0, count);
            return;
        }

        T* start = (T*)_position;

        size_t n = sizeof(T)*count;
        // we require aligned arrays...
        memcpy(_position, values, n);
        _position += n;

        // ... so that we can be fast changing endianness
        if (ENDIANESS_SUPPORT && reverse<T>())
        {
            for (std::size_t i = 0; i < count; i++)
            {
                *start = swap<T>(*start);
                start++;
            }
        }
    }

    template<typename T>
    inline void ByteBuffer::getArray(T* values, std::size_t count)
    {
        // this avoids int8 specialization, compiler will take care if optimization, -O2 or more
        if (sizeof(T) == 1)
        {
            get((char*)values, 0, count);
            return;
        }

        T* start = (T*)values;

        size_t n = sizeof(T)*count;
        // we require aligned arrays...
        memcpy(values, _position, n);
        _position += n;

        // ... so that we can be fast changing endianness
        if (ENDIANESS_SUPPORT && reverse<T>())
        {
            for (std::size_t i = 0; i < count; i++)
            {
                *start = swap<T>(*start);
                start++;
            }
        }
    }

    }
}
#endif  /* BYTEBUFFER_H */
