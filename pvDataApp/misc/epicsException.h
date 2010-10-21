/*
 * epicsException.hpp
 *
 *  Created on: Oct 18, 2010
 *      Author: miha_vitorovic
 */

#ifndef EPICSEXCEPTION_H_
#define EPICSEXCEPTION_H_

// TODO to be redefined!!!!!!

#include <stdexcept>
#include <string>

namespace epics { namespace pvData {

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
