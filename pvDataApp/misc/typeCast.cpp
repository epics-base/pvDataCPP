/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/* Author:  Michael Davidsaver */
#include <algorithm>

#include "typeCast.h"

using epics::pvData::castUnsafe;
using epics::pvData::String;
using epics::pvData::ScalarType;
using epics::pvData::pvString;

namespace {

static void noconvert(size_t, void*, const void*)
{
    throw std::runtime_error("castUnsafeV: Conversion not supported");
}

template<typename TO, typename FROM>
static void castVTyped(size_t count, void *draw, const void *sraw)
{
    TO *dest=(TO*)draw;
    const FROM *src=(FROM*)sraw;
    std::transform(src, src+count, dest, castUnsafe<TO,FROM>);
}

template<typename T>
static void copyV(size_t count, void *draw, const void *sraw)
{
    T *dest=(T*)draw;
    const T *src=(T*)sraw;
    std::copy(src, src+count, dest);
}

typedef void (*convertfn)(size_t, void*, const void*);

/* lookup table of converter functions.
 * first dimension is TO, second is FROM
 */
static convertfn converters[pvString+1][pvString+1] =
{
    // to pvBoolean
    { &noconvert,
      &noconvert,
      &noconvert,
      &noconvert,
      &noconvert,
      &noconvert,
      &noconvert,
      &noconvert,
      &noconvert,
      &noconvert,
      &noconvert,
      &noconvert
    },
    // to pvByte
    {&noconvert,
     &copyV<int8_t>,
     &castVTyped<int8_t, int16_t>,
     &castVTyped<int8_t, int32_t>,
     &castVTyped<int8_t, int64_t>,
     &castVTyped<int8_t, uint8_t>,
     &castVTyped<int8_t, uint16_t>,
     &castVTyped<int8_t, uint32_t>,
     &castVTyped<int8_t, uint64_t>,
     &castVTyped<int8_t, float>,
     &castVTyped<int8_t, double>,
     &castVTyped<int8_t, String>,
    },
    // to pvShort
    {&noconvert,
     &castVTyped<int16_t, int8_t>,
     &copyV<int16_t>,
     &castVTyped<int16_t, int32_t>,
     &castVTyped<int16_t, int64_t>,
     &castVTyped<int16_t, uint8_t>,
     &castVTyped<int16_t, uint16_t>,
     &castVTyped<int16_t, uint32_t>,
     &castVTyped<int16_t, uint64_t>,
     &castVTyped<int16_t, float>,
     &castVTyped<int16_t, double>,
     &castVTyped<int16_t, String>,
    },
    // to pvInt
    {&noconvert,
     &castVTyped<int32_t, int8_t>,
     &castVTyped<int32_t, int16_t>,
     &copyV<int32_t>,
     &castVTyped<int32_t, int64_t>,
     &castVTyped<int32_t, uint8_t>,
     &castVTyped<int32_t, uint16_t>,
     &castVTyped<int32_t, uint32_t>,
     &castVTyped<int32_t, uint64_t>,
     &castVTyped<int32_t, float>,
     &castVTyped<int32_t, double>,
     &castVTyped<int32_t, String>,
    },
    // to pvLong
    {&noconvert,
     &castVTyped<int64_t, int8_t>,
     &castVTyped<int64_t, int16_t>,
     &castVTyped<int64_t, int32_t>,
     &copyV<int64_t>,
     &castVTyped<int64_t, uint8_t>,
     &castVTyped<int64_t, uint16_t>,
     &castVTyped<int64_t, uint32_t>,
     &castVTyped<int64_t, uint64_t>,
     &castVTyped<int64_t, float>,
     &castVTyped<int64_t, double>,
     &castVTyped<int64_t, String>,
    },
    // to pvUByte
    {&noconvert,
     &castVTyped<uint8_t, int8_t>,
     &castVTyped<uint8_t, int16_t>,
     &castVTyped<uint8_t, int32_t>,
     &castVTyped<uint8_t, uint64_t>,
     &copyV<uint8_t>,
     &castVTyped<uint8_t, uint16_t>,
     &castVTyped<uint8_t, uint32_t>,
     &castVTyped<uint8_t, uint64_t>,
     &castVTyped<uint8_t, float>,
     &castVTyped<uint8_t, double>,
     &castVTyped<uint8_t, String>,
    },
    // to pvUShort
    {&noconvert,
     &castVTyped<uint16_t, int8_t>,
     &castVTyped<uint16_t, int16_t>,
     &castVTyped<uint16_t, int32_t>,
     &castVTyped<uint16_t, uint64_t>,
     &castVTyped<uint16_t, uint8_t>,
     &copyV<uint16_t>,
     &castVTyped<uint16_t, uint32_t>,
     &castVTyped<uint16_t, uint64_t>,
     &castVTyped<uint16_t, float>,
     &castVTyped<uint16_t, double>,
     &castVTyped<uint16_t, String>,
    },
    // to pvUInt
    {&noconvert,
     &castVTyped<uint32_t, int8_t>,
     &castVTyped<uint32_t, int16_t>,
     &castVTyped<uint32_t, int32_t>,
     &castVTyped<uint32_t, uint64_t>,
     &castVTyped<uint32_t, uint8_t>,
     &castVTyped<uint32_t, uint16_t>,
     &copyV<uint32_t>,
     &castVTyped<uint32_t, uint64_t>,
     &castVTyped<uint32_t, float>,
     &castVTyped<uint32_t, double>,
     &castVTyped<uint32_t, String>,
    },
    // to pvULong
    {&noconvert,
     &castVTyped<uint64_t, int8_t>,
     &castVTyped<uint64_t, int16_t>,
     &castVTyped<uint64_t, int32_t>,
     &castVTyped<uint64_t, uint64_t>,
     &castVTyped<uint64_t, uint8_t>,
     &castVTyped<uint64_t, uint16_t>,
     &castVTyped<uint64_t, uint32_t>,
     &copyV<uint64_t>,
     &castVTyped<uint64_t, float>,
     &castVTyped<uint64_t, double>,
     &castVTyped<uint64_t, String>,
    },
    // to pvFloat
    {&noconvert,
     &castVTyped<float, int8_t>,
     &castVTyped<float, int16_t>,
     &castVTyped<float, int32_t>,
     &castVTyped<float, uint64_t>,
     &castVTyped<float, uint8_t>,
     &castVTyped<float, uint16_t>,
     &castVTyped<float, uint32_t>,
     &castVTyped<float, uint64_t>,
     &copyV<float>,
     &castVTyped<float, double>,
     &castVTyped<float, String>,
    },
    // to pvDouble
    {&noconvert,
     &castVTyped<double, int8_t>,
     &castVTyped<double, int16_t>,
     &castVTyped<double, int32_t>,
     &castVTyped<double, uint64_t>,
     &castVTyped<double, uint8_t>,
     &castVTyped<double, uint16_t>,
     &castVTyped<double, uint32_t>,
     &castVTyped<double, uint64_t>,
     &castVTyped<double, float>,
     &copyV<double>,
     &castVTyped<double, String>,
    },
    // to pvString
    {&noconvert,
     &castVTyped<String, int8_t>,
     &castVTyped<String, int16_t>,
     &castVTyped<String, int32_t>,
     &castVTyped<String, uint64_t>,
     &castVTyped<String, uint8_t>,
     &castVTyped<String, uint16_t>,
     &castVTyped<String, uint32_t>,
     &castVTyped<String, uint64_t>,
     &castVTyped<String, float>,
     &castVTyped<String, double>,
     &copyV<String>,
    },
};

} // end namespace

namespace epics { namespace pvData {

void castUnsafeV(size_t count, ScalarType to, void *dest, ScalarType from, const void *src)
{
    unsigned int ito=to, ifrom=from;

    if(ito>pvString || ifrom>pvString)
        throw std::runtime_error("castUnsafeV: Invalid types");

    converters[ito][ifrom](count, dest, src);
}

}}
