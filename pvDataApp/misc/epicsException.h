/*
 * epicsException.hpp
 *
 *  Created on: Oct 20, 2010
 *      Author: Matej Sekoranja
 */

#ifndef EPICSEXCEPTION_H_
#define EPICSEXCEPTION_H_

// TODO to be redefined!!!!!!

#include <stdexcept>
#include <string>

namespace epics { namespace pvData {


class BaseException :
    public std::exception {
public:
    BaseException(const char* message, const char* file, int line, std::exception* cause = 0)
      : m_msg(message), m_file(file), m_line(line), m_cause(cause)
    {
    }

    virtual ~BaseException() throw()
    {
        if (m_cause) delete m_cause;
    }

    virtual const char* what() const throw() { return m_msg.c_str(); }

    const char* getFile() const { return m_file.c_str(); }
    int getLine() const { return m_line; }

    void toString(std::string& str) {
        str.append("BaseException: ");
        str.append(m_msg);
        str.append("\n\tat ");
        str.append(m_file);
        str.append(":");
        char sline[10];
        snprintf(sline, 10, "%d", m_line);
        str.append(sline);
        if (m_cause)
        {
            str.append("\ncaused by: ");
            BaseException *be = dynamic_cast<BaseException*>(m_cause);
            if (be)
                be->toString(str);
            else
                str.append(m_cause->what());
        }
    }

private:
    std::string m_msg;
    std::string m_file;
    int m_line;
    std::exception* m_cause;
};


#define THROW_BASE_EXCEPTION(msg) throw new BaseException(msg, __FILE__, __LINE__)
#define THROW_BASE_EXCEPTION_CAUSE(msg, cause) throw new BaseException(msg, __FILE__, __LINE__, cause)

/*
        /// Construct with file, line info and printf-type arguments.
        GenericException(const char *sourcefile, size_t line, const char *format, ...)
        __attribute__ ((format (printf, 4, 5)));
*/

        /** Base Epics Exception */
        class EpicsException : public std::logic_error {
        public:
            explicit EpicsException(const std::string& arg) :
                    std::logic_error(arg) {
            }
        };
    }
}


#endif /* EPICSEXCEPTION_H_ */
