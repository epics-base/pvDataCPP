/*createRequest.h*/
/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */

#ifndef CREATEREQUEST_H
#define CREATEREQUEST_H
#include <string>
#include <sstream>

#include <pv/pvData.h>
#include <pv/lock.h>

#include <shareLib.h>

namespace epics { namespace pvData {

class BitSet;

/**
 * @brief Create pvRequest structure for Channel methods.
 *
 * Many methods of the Channel class of pvAccess have an
 * argument <b>PVStructurePtr const * pvRequest</b>.
 * This class provides a method that creates a valid pvRequest.
 *
 */
class epicsShareClass CreateRequest {
    public:
    POINTER_DEFINITIONS(CreateRequest);
    /**
     * Create s new instance of CreateRequest
     * @returns A shared pointer to the new instance.
     */
    static CreateRequest::shared_pointer create();
    ~CreateRequest() {};
    /**
    * Create a request structure for the create calls in Channel.
    * See the package overview documentation for details.
    * @param request The field request. See the package overview documentation for details.
    * @return The request PVStructure if a valid request was given.
    * If a NULL PVStructure is returned then getMessage will return
    * the reason.
    */
    PVStructure::shared_pointer createRequest(std::string const & request);
    /**
     * Get the error message of createRequest returns NULL
     * return the error message
     */
    std::string getMessage() {return message;}
protected:
    CreateRequest() {}
    std::string message;
};

/** Parse and build pvRequest structure.
 *
 @params request the Request string to be parsed.  eg. "field(value)"
 @returns The resulting strucuture.  Never NULL
 @throws std::exception for various parsing errors
 */
epicsShareExtern
PVStructure::shared_pointer createRequest(std::string const & request);

/** Extract a bit mask of fields from a field selection mask.
 *
 @param type The Structure to which the mask will be applied
 @param pvRequestMask The 'field' sub-structure of a pvRequest.  May be NULL
 @param expand If true, expand any "compressed" sub-structure bits
 @returns A bit mask, where the bits are field offset in 'type'.
 *
 @code
    PVStructure::const_shared_pointer value(...), // some Structure with .value
                                      pvRequest(createRequest("field(value)"));
    BitSet fieldMask(extractRequestMask(value, pvRequest->getSubField<PVStructure>("field"));
    assert(fieldMask == BitSet().set(value->getSubFieldT("value")->getFieldOffset());
 @endcode
 */
epicsShareExtern
BitSet extractRequestMask(const PVStructure::const_shared_pointer& type,
                          const PVStructure::const_shared_pointer& pvRequestMask,
                          bool expand = true);

}}

#endif /* CREATEREQUEST_H */

