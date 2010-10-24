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

struct Unroller
{
    template <int N>
    void unroll(double d) {
        unroll<N-1>(d);
    }
};

template<>
void Unroller::unroll<0>(double d) {
    THROW_BASE_EXCEPTION("the root cause");
}

void internalTestBaseException(int unused = 0)
{
    try {
        // NOTE: 5, 4, 3, 2, 1 calls will be optimized and not shown
        Unroller().unroll<5>(42.0);
    } catch (BaseException *be3) {
        THROW_BASE_EXCEPTION_CAUSE("exception 1", be3);
    }
}

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
            internalTestBaseException();
        } catch (BaseException *be2) {
            THROW_BASE_EXCEPTION_CAUSE("exception 2", be2);
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

