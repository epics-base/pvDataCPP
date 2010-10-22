/* testBaseException.cpp */
/* Author:  Matej Sekoranja Date: 2010.10.18 */

#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "epicsException.h"


#include <epicsAssert.h>

using namespace epics::pvData;

void testBaseException() {
    printf("testBaseException... ");

    try {
        THROW_BASE_EXCEPTION("all is OK");
    } catch (BaseException *be) {
        std::string str;
        be->toString(str);
        printf("\n\n%s\n\n", str.c_str());
    }

    try {
        try {
            try {
                THROW_BASE_EXCEPTION("the root cause");
            } catch (BaseException *be3) {
                THROW_BASE_EXCEPTION_CAUSE("exception 1", be3);
            }
        } catch (BaseException *be2) {
            THROW_BASE_EXCEPTION_CAUSE("excepton 2", be2);
        }
    } catch (BaseException *be) {
        std::string str;
        be->toString(str);
        printf("\n\n%s\n\n", str.c_str());
        delete be;
    }

    printf("PASSED\n");
}

int main(int argc,char *argv[])
{
    testBaseException();
    return(0);
}

