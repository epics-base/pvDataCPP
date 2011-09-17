/* byteBuffer.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H

#include <string>
#include <pv/pvType.h>
#include <epicsEndian.h>
#include <string.h>

#include <pv/epicsException.h>

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
    (((uintptr_t)(const void *)(POINTER)) % (BYTE_COUNT) == 0)

/*template <bool ENDIANESS_SUPPORT = false,
          bool UNALIGNED_ACCESS = false,
          bool ADAPTIVE_ACCESS = true,
          bool USE_INLINE_MEMCPY = true>*/

#define ENDIANESS_SUPPORT true
#define UNALIGNED_ACCESS true
#define ADAPTIVE_ACCESS true
#define USE_INLINE_MEMCPY true
          
class ByteBuffer 
{
public:
    ByteBuffer(uintptr_t size, int byteOrder = EPICS_BYTE_ORDER) :
        _buffer(0), _size(size),
        _reverseEndianess(byteOrder != EPICS_BYTE_ORDER),
        _reverseFloatEndianess(byteOrder != EPICS_FLOAT_WORD_ORDER)
    {
        _buffer = (char*)malloc(size);
        clear();
    }
    
    ~ByteBuffer()
    {
        if (_buffer) free(_buffer);
    }
    
    inline void setEndianess(int byteOrder)
    {
        _reverseEndianess = (byteOrder != EPICS_BYTE_ORDER);
        _reverseFloatEndianess = (byteOrder != EPICS_FLOAT_WORD_ORDER);
    }
    
    inline const char* getBuffer()
    {
        return _buffer;
    }

    inline void clear()
    {
        _position = _buffer;
        _limit = _buffer + _size;
    }
    
    inline void flip() {
        _limit = _position;
        _position = _buffer;
    }

    inline void rewind() {
        _position = _buffer;
    }

    inline uintptr_t getPosition()
    {
        return (((uintptr_t)(const void *)_position) - ((uintptr_t)(const void *)_buffer));
    }
    
    inline void setPosition(uintptr_t pos)
    {
        _position = _buffer + pos;
    }
    
    inline uintptr_t getLimit()
    {
        return (((uintptr_t)(const void *)_limit) - ((uintptr_t)(const void *)_buffer));
    }

    inline void setLimit(uintptr_t limit)
    {
        _limit = _buffer + limit;
    }

    inline uintptr_t getRemaining()
    {
        return (((uintptr_t)(const void *)_limit) - ((uintptr_t)(const void *)_position));
    }
    
    inline uintptr_t getSize()
    {
        return _size;
    }
    
    
   
   
    template<typename T>
    inline void put(T value)
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
            // NOTE: some CPU handle unaligned access preety good (e.g. x86)
            *((T*)_position) = value;
            _position += sizeof(T);
        }
        else
        {
            // NOTE: this check and branching does not always payoff 
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
    inline void put(uintptr_t index, T value)
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
            // NOTE: some CPU handle unaligned access preety good (e.g. x86)
            *((T*)(_buffer + index)) = value;
        }
        else
        {
            // NOTE: this check and branching does not always payoff 
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




    template<typename T>
    inline T get()
    {
        // this avoids int8 specialization, compiler will take care if optimization, -O2 or more
        if (sizeof(T) == 1)
        {
            return (int8)(*(_position++));
        }
        

        T value;
        
        if (UNALIGNED_ACCESS)
        {
            // NOTE: some CPU handle unaligned access preety good (e.g. x86)
            value = *((T*)_position);
            _position += sizeof(T);
        }
        else
        {
            // NOTE: this check and branching does not always payoff 
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
    inline T get(uintptr_t index)
    {
        // this avoids int8 specialization, compiler will take care if optimization, -O2 or more
        if (sizeof(T) == 1)
        {
            return (int8)(*(_buffer + index));
        }
        

        T value;
        
        if (UNALIGNED_ACCESS)
        {
            // NOTE: some CPU handle unaligned access preety good (e.g. x86)
            value = *((T*)(_buffer + index));
        }
        else
        {
            // NOTE: this check and branching does not always payoff 
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




    inline void put(const char* src, uintptr_t src_offset, uintptr_t count) {
        //if(count>getRemaining()) THROW_BASE_EXCEPTION("buffer overflow");
        memcpy(_position, src + src_offset, count);
        _position += count;
    }


    inline void get(char* dest, uintptr_t dest_offset, uintptr_t count) {
        //if(count>getRemaining()) THROW_BASE_EXCEPTION("buffer overflow");
        memcpy(dest + dest_offset, _position, count);
        _position += count;
    }

    template<typename T>
    inline void putArray(T* values, uintptr_t count)
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
        
        // ... so that we can be fast changing endianess
        if (ENDIANESS_SUPPORT && reverse<T>()) 
        {
            for (uintptr_t i = 0; i < count; i++)
            {
                *start = swap<T>(*start);
                start++;            
            }   
        }
    }






    template<typename T>
    inline void getArray(T* values, uintptr_t count)
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
        
        // ... so that we can be fast changing endianess
        if (ENDIANESS_SUPPORT && reverse<T>()) 
        {
            for (uintptr_t i = 0; i < count; i++)
            {
                *start = swap<T>(*start);
                start++;            
            }   
        }
    }
    
    
    template<typename T>
    inline bool reverse()
    {
        return _reverseEndianess;
    }
    
    

    // NOTE: size must be power of 2
    inline void align(int size)
    {
        _position = (char*)((((uintptr_t)(const void *)_position) + size - 1) & ~((uintptr_t)(size - 1)));
    }
    
    
    
    
    
    inline void putBoolean(  bool value) { put<  int8>(value ? 1 : 0); }
    inline void putByte   (  int8 value) { put<  int8>(value); }
    inline void putShort  ( int16 value) { put< int16>(value); }
    inline void putInt    ( int32 value) { put< int32>(value); }
    inline void putLong   ( int64 value) { put< int64>(value); }
    inline void putFloat  ( float value) { put< float>(value); }
    inline void putDouble (double value) { put<double>(value); }

    inline void putBoolean(uintptr_t index,  bool value) { put<  int8>(index, value); }
    inline void putByte   (uintptr_t index,  int8 value) { put<  int8>(index, value); }
    inline void putShort  (uintptr_t index, int16 value) { put< int16>(index, value); }
    inline void putInt    (uintptr_t index, int32 value) { put< int32>(index, value); }
    inline void putLong   (uintptr_t index, int64 value) { put< int64>(index, value); }
    inline void putFloat  (uintptr_t index, float value) { put< float>(index, value); }
    inline void putDouble (uintptr_t index,double value) { put<double>(index, value); }

    inline   bool getBoolean() { return get<  int8>() != 0; }
    inline   int8 getByte   () { return get<  int8>(); }
    inline  int16 getShort  () { return get< int16>(); }
    inline  int32 getInt    () { return get< int32>(); }
    inline  int64 getLong   () { return get< int64>(); }
    inline  float getFloat  () { return get< float>(); }
    inline double getDouble () { return get<double>(); }

    inline   bool getBoolean(uintptr_t index) { return get<  int8>(index) != 0; }
    inline   int8 getByte   (uintptr_t index) { return get<  int8>(index); }
    inline  int16 getShort  (uintptr_t index) { return get< int16>(index); }
    inline  int32 getInt    (uintptr_t index) { return get< int32>(index); }
    inline  int64 getLong   (uintptr_t index) { return get< int64>(index); }
    inline  float getFloat  (uintptr_t index) { return get< float>(index); }
    inline double getDouble (uintptr_t index) { return get<double>(index); }

    // TODO remove
    inline const char* getArray()
    {
        return _buffer;
    }

    
private:
    char* _buffer;
    char* _position;
    char* _limit;
    uintptr_t _size;
    bool _reverseEndianess; 
    bool _reverseFloatEndianess; 
};

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


    }
}
#endif  /* BYTEBUFFER_H */
