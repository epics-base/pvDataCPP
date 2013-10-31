/* pvSubArrayCopy.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 * @author Marty Kraimer
 * @date 2013.07
 */
#ifndef PVSUBARRAYCOPY_H
#define PVSUBARRAYCOPY_H
#include <pv/pvData.h>

namespace epics { namespace pvData { 

/** @brief Copy a subarray from one PVValueArray to another.
 * @warning The two PVValueArrays must both the same type
 * @param from The source
 * @param fromOffset The offset in the source
 * @param to The destination
 * @param toOffset The offset in the destination
 * @param len The total number of elements to copy
 */
template<typename T>
void copy(
    PVValueArray<T> & pvFrom,
    size_t fromOffset,
    PVValueArray<T> & pvTo,
    size_t toOffset,
    size_t len);

/** @brief Copy a subarray from one scalar array to another.
 * @warning The two scalar arrays must both be PVValueArrays of the same type
 * @param from The source
 * @param fromOffset The offset in the source
 * @param to The destination
 * @param toOffset The offset in the destination
 * @param len The total number of elements to copy
 */
void copy(
    PVScalarArray & from,
    size_t fromOffset,
    PVScalarArray & to,
    size_t toOffset,
    size_t len);

/** @brief Copy a subarray from one structure array to another.
 * @warning The two structure arrays must have the same
 * structure introspection interface.
 * @param from The source
 * @param fromOffset The offset in the source
 * @param to The destination
 * @param toOffset The offset in the destination
 * @param len The total number of elements to copy
 */
void copy(
    PVStructureArray & from,
    size_t fromOffset,
    PVStructureArray & to,
    size_t toOffset,
    size_t len);

/** @brief Copy a subarray from one  array to another.
 * @warning The two arrays must have the same
 * introspection interface.
 * @param from The source
 * @param fromOffset The offset in the source
 * @param to The destination
 * @param toOffset The offset in the destination
 * @param len The total number of elements to copy
 */
void copy(
    PVArray & from,
    size_t fromOffset,
    PVArray & to,
    size_t toOffset,
    size_t len);

}}


#endif  /* PVSUBARRAYCOPY_H */
