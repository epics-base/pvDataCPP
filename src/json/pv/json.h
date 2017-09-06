/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */
#ifndef PV_JSON_H
#define PV_JSON_H

#include <istream>
#include <ostream>
#include <string>
#include <map>

#include <pv/pvdVersion.h>
#include <pv/pvData.h>

#if EPICS_VERSION_INT>=VERSION_INT(3,15,0,1)

#ifdef epicsExportSharedSymbols
#   define pvjson_epicsExportSharedSymbols
#   undef epicsExportSharedSymbols
#endif

#include <yajl_parse.h>

#ifdef pvjson_epicsExportSharedSymbols
#   define epicsExportSharedSymbols
#   include "shareLib.h"
#endif

#include <shareLib.h>

namespace epics{namespace pvData{

/** @defgroup pvjson JSON print/parse
 *
 * Printing PVField as JSON and parsing JSON into PVField.
 *
 * @{
 */

//! Options used during printing
struct epicsShareClass JSONPrintOptions
{
    bool multiLine;        //!< include new lines
    bool ignoreUnprintable;//!< ignore union/union array when encountered
    unsigned indent;       //!< Initial indentation (# of spaces)
    JSONPrintOptions();
};

/** Print PVStructure as JSON
 *
 * Restrictions:
 *
 * - No support for union or array of union
 */
epicsShareFunc
void printJSON(std::ostream& strm,
               const PVField::const_shared_pointer& val,
               const JSONPrintOptions& opts = JSONPrintOptions());

/** Parse JSON text into a PVStructure
 *
 * Restrictions:
 *
 * - Top level must be {}  dict/object
 * - field values must be number, string, array, or dict/object
 * - array values must be number or string
 */
epicsShareFunc
PVStructure::shared_pointer parseJSON(std::istream& strm);

/** Parse JSON and store into the provided PVStructure.
 *
 * Restrictions:
 *
 * - array of union not permitted
 * - Only scalar value assigned to union
 */
epicsShareFunc
void parseJSON(std::istream& strm,
               const PVField::shared_pointer& dest);


/** Wrapper around yajl_parse()
 *
 * Parse entire input stream.
 * Errors if extranious non-whitespace found after the point were parsing completes.
 *
 * @param src The stream from which input charactors are read
 * @param handle A parser handle previously allocated with yajl_alloc().  Not free'd on success or failure.
 * @param config The same configuration passed to yajl_alloc().  Used to decide if trailing comments are allowed
 *
 * @returns true if parsing completes successfully.  false if parsing cancelled by callback.  throws other errors
 */
epicsShareFunc
bool yajl_parse_helper(std::istream& src,
                       yajl_handle handle,
                       const yajl_parser_config& config);

/** @} */

}} // namespace epics::pvData

#else
#  error JSON parser requires EPICS Base >= 3.15.0.1
#endif // EPICS_VERSION_INT

#endif // PV_JSON_H
