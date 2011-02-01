/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/*
 * epicsException.hpp
 *
 *  Created on: Oct 20, 2010
 *      Author: Matej Sekoranja
 */

#include <cstdio>
#ifndef EPICSEXCEPTION_H_
#define EPICSEXCEPTION_H_

#include <stdexcept>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <cxxabi.h>

namespace epics { namespace pvData {


class BaseException :
    public std::exception {
public:
    BaseException(const char* message, const char* file, int line)
    {
        toString(m_what, message, file, line, 0);
    }

    BaseException(const char* message, const char* file, int line, std::exception& cause)
    {
        toString(m_what, message, file, line, cause.what());
    }

    virtual ~BaseException() throw()
    {
    }

    virtual const char* what() const throw() { return m_what.c_str(); }

private:
    static inline void toString(std::string& str, const char* message, const char* file, int line, const char * cause) {
        str.append(message);
        str.append("\n\tat ");
        str.append(file);
        str.append(":");
        char sline[10];
        snprintf(sline, 10, "%d", line);
        str.append(sline);
        str.append("\n");
        getStackTrace(&str);
        if (cause)
            str.append(cause);
    }

    /** Get stack trace, i.e. demangled backtrace of the caller. */
    static inline void getStackTrace(std::string* trace, unsigned int skip_frames = 0, unsigned int max_frames = 63)
    {
#ifdef DISABLE_STACK_TRACE
        trace += "(stack trace disabled)";
#else
        // storage array for stack trace address data
        void* addrlist[max_frames+1];

        // retrieve current stack addresses
        int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));

        if (addrlen == 0) {
            trace->append("(stack trace not available)");
            return;
        }

        // resolve addresses into strings containing "filename(function+address)",
        // this array must be free()-ed
        char** symbollist = backtrace_symbols(addrlist, addrlen);

        // allocate string which will be filled with the demangled function name
        size_t funcnamesize = 256;
        char* funcname = (char*)malloc(funcnamesize);

        // iterate over the returned symbol lines. skip the first, it is the
        // address of this function.
        for (int i = (1 + skip_frames); i < addrlen; i++)
        {
            char *module = 0, *fname = 0, *offset = 0;
#ifdef __APPLE__
            int stage = 0;
            for (char *p = symbollist[i]; *p; ++p)
            {
                // find spaces and separate
                // 0   a.out                               0x0000000100000bbc _Z11print_tracev + 22
                switch (stage)
                {
                case 0: // skip frame index
                    if (*p == ' ') stage++;
                    break;
                case 1: // skip spaces
                    if (*p != ' ') { module = p; stage++; }
                    break;
                  case 2: // module name
                      if (*p == ' ') { *p = '\0'; stage++; }
                      break;
                  case 3: // skip spaces
                      if (*p != ' ') stage++;
                      break;
                  case 4: // address
                      if (*p == ' ') { fname = p+1; stage++; }
                      break;
                  case 5: // function
                      if (*p == ' ') { *p = '\0'; stage++; }
                      break;
                  case 6: // "+ "
                      if (*p == '+') { p++; offset = p+1; };
                      break;
                  }
            }
#else
            // find parentheses and +address offset surrounding the mangled name:
            // ./module(function+0x15c) [0x8048a6d]
            module = symbollist[i];
            for (char *p = symbollist[i]; *p; ++p)
            {
                if (*p == '(') {
                    // terminate module
                    *p = '\0';
                    fname = p+1;
                }
                else if (*p == '+') {
                    // terminate fname
                    *p = '\0';
                    offset = p+1;
                }
                else if (*p == ')' && offset) {
                    // terminate offset
                    *p = '\0';
                    break;
                }
            }
#endif

        if (fname && offset && offset && fname < offset)
        {

            // mangled name is now in [begin_name, begin_offset) and caller
            // offset in [begin_offset, end_offset). now apply
            // __cxa_demangle():

            int status;
            char* ret = abi::__cxa_demangle(fname,
                                            funcname, &funcnamesize, &status);
            if (status == 0) {
                trace->append("\t   ");
                *trace += module;
                trace->append(": ");
                *trace += ret; // use possibly realloc()-ed string
                trace->append("+");
                *trace += offset;
                trace->append("\n");
            }
            else {
                // demangling failed. Output function name as a C function with
                // no arguments.
                trace->append("\t   ");
                *trace += module;
                trace->append(": ");
                *trace += fname;
                *trace += "()+";
                *trace += offset;
                trace->append("\n");
            }
        }
        else
        {
            // couldn't parse the line? print the whole line.
            trace->append("\t   ");
            *trace += symbollist[i];
            trace->append("\n");
        }
    }

    free(funcname);
    free(symbollist);

#endif
    }

private:
    std::string m_what;
};


#define THROW_BASE_EXCEPTION(msg) throw ::epics::pvData::BaseException(msg, __FILE__, __LINE__)
#define THROW_BASE_EXCEPTION_CAUSE(msg, cause) throw ::epics::pvData::BaseException(msg, __FILE__, __LINE__, cause)

    }
}


#endif /* EPICSEXCEPTION_H_ */
