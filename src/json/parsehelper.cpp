/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */

#include <stdexcept>
#include <sstream>

#include <pv/pvdVersion.h>

#if EPICS_VERSION_INT>=VERSION_INT(3,15,0,1)

#define epicsExportSharedSymbols
#include "pv/json.h"

namespace {

void check_trailing(const std::string& line, bool commentok)
{
    size_t idx = line.find_first_not_of(" \t\n\r");
    if(idx==line.npos) return;
    // TODO: detect the end of potentially multi-line comments...
    // for now trailing comments not allowed
    throw std::runtime_error("Trailing junk");
}

} // namespace

namespace epics{namespace pvData{

bool yajl_parse_helper(std::istream& src,
                       yajl_handle handle,
                       const yajl_parser_config& config)
{
    unsigned linenum=0;
    bool done = false;

    std::string line;
    while(std::getline(src, line)) {
        linenum++;

        if(done) {
            check_trailing(line, config.allowComments);
            continue;
        }

        yajl_status sts = yajl_parse(handle, (const unsigned char*)line.c_str(), line.size());

        switch(sts) {
        case yajl_status_ok: {
            size_t consumed = yajl_get_bytes_consumed(handle);
            if(consumed<line.size()) {
                check_trailing(line.substr(consumed), config.allowComments);
            }
            done = true;
            break;
        }
        case yajl_status_client_canceled:
            return false;
        case yajl_status_insufficient_data:
            // continue with next line
            break;
        case yajl_status_error:
        {
            std::ostringstream msg;
            unsigned char *raw = yajl_get_error(handle, 1, (const unsigned char*)line.c_str(), line.size());
            if(!raw) {
                msg<<"Unknown error on line "<<linenum;
            } else {
                try {
                    msg<<"Error on line "<<linenum<<" : "<<(const char*)raw;
                }catch(...){
                    yajl_free_error(handle, raw);
                    throw;
                }
                yajl_free_error(handle, raw);
            }
            throw std::runtime_error(msg.str());
        }
        }
    }

    if(!src.eof() || src.bad()) {
        std::ostringstream msg;
        msg<<"I/O error after line "<<linenum;
        throw std::runtime_error(msg.str());

    } else if(!done) switch(yajl_parse_complete(handle)) {
    case yajl_status_ok:
        break;
    case yajl_status_client_canceled:
        return false;
    case yajl_status_insufficient_data:
        throw std::runtime_error("unexpected end of input");
    case yajl_status_error:
        throw std::runtime_error("Error while completing parsing");
    }

    return true;
}

}} // namespace epics::pvData

#endif // EPICS_VERSION_INT
