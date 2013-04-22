/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/* Author:  Michael Davidsaver */

#include <fstream>
#include <iostream>
#include <limits>
#include <typeinfo>
#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <float.h>
#include <epicsMath.h>

#include <epicsMath.h>

#include "pv/typeCast.h"

#include <stdint.h>

using epics::pvData::String;

namespace {

    template<typename T>
    struct testequal {
        static bool op(T A, T B) {return A==B; }
    };
    template<>
    struct testequal<double> {
        static bool op(double A, double B) {return fabs(A-B)<1e-300; }
    };
    template<>
    struct testequal<float> {
        static bool op(float A, float B) {return fabs(A-B)<1e-30; }
    };

    template<typename TO, typename FROM>
    struct testcase {
        static bool op(std::ostream& msg, TO expect, FROM inp)
        {
            TO actual;
            try {
                actual = ::epics::pvData::castUnsafe<TO,FROM>(inp);
                //actual = ::epics::pvData::detail::cast_helper<TO,FROM>::op(inp);
            } catch(std::runtime_error& e) {
                msg<<"Failed to cast "
                   <<inp<<" ("<<typeid(FROM).name()<<") -> "
                   <<expect<<" ("<<typeid(TO).name()<<")\n Error: "
                   <<typeid(e).name()<<"("<<e.what()<<")\n";
                return false;
            }
            if(!testequal<TO>::op(actual, expect)) {
                msg<<"Failed cast gives unexpected value "
                   <<inp<<" ("<<typeid(FROM).name()<<") -> "
                   <<expect<<" ("<<typeid(TO).name()<<") yields: "
                   <<actual<<"\n";
                return false;
            }
            msg<<"Pass cast "
               <<inp<<" ("<<typeid(FROM).name()<<") -> "
               <<expect<<" ("<<typeid(TO).name()<<") yields: "
               <<actual<<"\n";
            return true;
        }
    };

    template<typename TO, typename FROM>
    struct testfail {
        static bool op(std::ostream& msg, FROM inp)
        {
            TO actual;
            try {
                actual = ::epics::pvData::castUnsafe<TO,FROM>(inp);
                msg<<"Failed to generate expected error "
                   <<inp<<" ("<<typeid(FROM).name()<<") -> ("
                   <<typeid(TO).name()<<") yields: "
                   <<actual<<"\n";
                return false;
            } catch(std::runtime_error& e) {
                msg<<"Got expected error "
                   <<inp<<" ("<<typeid(FROM).name()<<") -> ("
                   <<typeid(TO).name()<<") fails with: "
                   <<e.what()<<"\n";
                return true;
            }
        }
    };


// Test cast
#define TEST(TTO, VTO, TFRO, VFRO) fail |= !testcase<TTO, TFRO>::op(*out, VTO, VFRO)

// Test cast and reverse
#define TEST2(TTO, VTO, TFRO, VFRO) TEST(TTO, VTO, TFRO, VFRO); TEST(TFRO, VFRO, TTO, VTO)

#define FAIL(TTO, TFRO, VFRO) fail |= !testfail<TTO,TFRO>::op(*out, VFRO)

} // end namespace


int main(int argc,char *argv[])
{
    std::ostream *out = &std::cerr;
    std::ofstream outf;
    if(argc>1){
        outf.open(argv[1]);
        if(!outf.good()) {
            std::cerr<<"Failed to open "<<argv[1]<<" for output!\n";
        } else {
            out = &outf;
        }
    }
    bool fail=false;

    int8_t xint8=0;
    uint8_t xuint8=0;
    int16_t xint16=0;
    uint16_t xuint16=0;
    int32_t xint32=0;
    uint32_t xuint32=0;
    int64_t xint64=0;
    uint64_t xuint64=0;
    float xfloat=0.0;
    double xdouble=0.0;
    epics::pvData::String xString("0");

    typedef float float_t;
    typedef double double_t;
    typedef epics::pvData::String String_t;

    // force all possibilities to be compiled
#define CHECK(M, N) x## M = ::epics::pvData::castUnsafe<M ##_t>(x## N)
//#define CHECK(M, N) x## M = ::epics::pvData::detail::cast_helper<M ##_t,N ##_t>::op(x## N)
    CHECK(int8, int8);
    CHECK(int8, uint8);
    CHECK(int8, int16);
    CHECK(int8, uint16);
    CHECK(int8, int32);
    CHECK(int8, uint32);
    CHECK(int8, int64);
    CHECK(int8, uint64);
    CHECK(int8, float);
    CHECK(int8, double);
    CHECK(int8, String);

    CHECK(uint8, int8);
    CHECK(uint8, uint8);
    CHECK(uint8, int16);
    CHECK(uint8, uint16);
    CHECK(uint8, int32);
    CHECK(uint8, uint32);
    CHECK(uint8, int64);
    CHECK(uint8, uint64);
    CHECK(uint8, float);
    CHECK(uint8, double);
    CHECK(uint8, String);

    CHECK(int16, int8);
    CHECK(int16, uint8);
    CHECK(int16, int16);
    CHECK(int16, uint16);
    CHECK(int16, int32);
    CHECK(int16, uint32);
    CHECK(int16, int64);
    CHECK(int16, uint64);
    CHECK(int16, float);
    CHECK(int16, double);
    CHECK(int16, String);

    CHECK(uint16, int8);
    CHECK(uint16, uint8);
    CHECK(uint16, int16);
    CHECK(uint16, uint16);
    CHECK(uint16, int32);
    CHECK(uint16, uint32);
    CHECK(uint16, int64);
    CHECK(uint16, uint64);
    CHECK(uint16, float);
    CHECK(uint16, double);
    CHECK(uint16, String);

    CHECK(int32, int8);
    CHECK(int32, uint8);
    CHECK(int32, int16);
    CHECK(int32, uint16);
    CHECK(int32, int32);
    CHECK(int32, uint32);
    CHECK(int32, int64);
    CHECK(int32, uint64);
    CHECK(int32, float);
    CHECK(int32, double);
    CHECK(int32, String);

    CHECK(uint32, int8);
    CHECK(uint32, uint8);
    CHECK(uint32, int16);
    CHECK(uint32, uint16);
    CHECK(uint32, int32);
    CHECK(uint32, uint32);
    CHECK(uint32, int64);
    CHECK(uint32, uint64);
    CHECK(uint32, float);
    CHECK(uint32, double);
    CHECK(uint32, String);

    CHECK(int64, int8);
    CHECK(int64, uint8);
    CHECK(int64, int16);
    CHECK(int64, uint16);
    CHECK(int64, int32);
    CHECK(int64, uint32);
    CHECK(int64, int64);
    CHECK(int64, uint64);
    CHECK(int64, float);
    CHECK(int64, double);
    //CHECK(int64, String);

    CHECK(uint64, int8);
    CHECK(uint64, uint8);
    CHECK(uint64, int16);
    CHECK(uint64, uint16);
    CHECK(uint64, int32);
    CHECK(uint64, uint32);
    CHECK(uint64, int64);
    CHECK(uint64, uint64);
    CHECK(uint64, float);
    CHECK(uint64, double);
    //CHECK(uint64, String);

    CHECK(float, int8);
    CHECK(float, uint8);
    CHECK(float, int16);
    CHECK(float, uint16);
    CHECK(float, int32);
    CHECK(float, uint32);
    CHECK(float, int64);
    CHECK(float, uint64);
    CHECK(float, float);
    CHECK(float, double);
    CHECK(float, String);

    CHECK(double, int8);
    CHECK(double, uint8);
    CHECK(double, int16);
    CHECK(double, uint16);
    CHECK(double, int32);
    CHECK(double, uint32);
    CHECK(double, int64);
    CHECK(double, uint64);
    CHECK(double, float);
    CHECK(double, double);
    CHECK(double, String);

    CHECK(String, int8);
    CHECK(String, uint8);
    CHECK(String, int16);
    CHECK(String, uint16);
    CHECK(String, int32);
    CHECK(String, uint32);
    CHECK(String, int64);
    CHECK(String, uint64);
    CHECK(String, float);
    CHECK(String, double);
    CHECK(String, String);
#undef CHECK

    *out << "Integer signed <=> unsigned\n";

    TEST2(uint8_t,  std::numeric_limits<uint8_t>::max(),  int8_t, -1);
    TEST2(uint16_t, std::numeric_limits<uint16_t>::max(), int8_t, -1);
    TEST2(uint32_t, std::numeric_limits<uint32_t>::max(), int8_t, -1);
    TEST2(uint64_t, std::numeric_limits<uint64_t>::max(), int8_t, -1);

    *out << "Integer unsigned promote (and demote when in range)\n";

    TEST2(uint16_t, 0xff, uint8_t, 0xff);
    TEST2(uint32_t, 0xffff, uint16_t, 0xffff);
    TEST2(uint64_t, 0xffffffffu, uint32_t, 0xffffffffu);

    TEST2(int16_t, 0x7f, int8_t, 0x7f);
    TEST2(int32_t, 0x7fff, int16_t, 0x7fff);
    TEST2(int64_t, 0x7fffffff, int32_t, 0x7fffffff);

    *out << "Double to int w/ round towards zero (aka truncation)\n";

    TEST(int32_t, 2, double, 2.1);
    TEST(int32_t, 2, double, 2.5);
    TEST(int32_t, 2, double, 2.7);
    TEST(int32_t, -2, double, -2.1);
    TEST(int32_t, -2, double, -2.5);
    TEST(int32_t, -2, double, -2.7);

    *out << "Float to int w/ round towards zero (aka truncation)\n";

    TEST(int32_t, 2, float, 2.1);
    TEST(int32_t, 2, float, 2.5);
    TEST(int32_t, 2, float, 2.7);
    TEST(int32_t, -2, float, -2.1);
    TEST(int32_t, -2, float, -2.5);
    TEST(int32_t, -2, float, -2.7);

    *out << "String Printing/parsing\n";

    TEST2(String, "1", int32_t, 1);
    TEST2(String, "-1", int32_t, -1);
    TEST2(String, "1", int8_t, 1);
    TEST2(String, "-1", int8_t, -1);
    TEST2(String, "1", uint8_t, 1);
    TEST2(String, "-1", char, -1);

    TEST2(String, "127", int32_t, std::numeric_limits<int8_t>::max());
    TEST2(String, "-128", int32_t, std::numeric_limits<int8_t>::min());
    TEST2(String, "255", int32_t, std::numeric_limits<uint8_t>::max());

    TEST2(String, "32767", int32_t, std::numeric_limits<int16_t>::max());
    TEST2(String, "-32768", int32_t, std::numeric_limits<int16_t>::min());
    TEST2(String, "65535", int32_t, std::numeric_limits<uint16_t>::max());

    TEST2(String, "2147483647", int32_t, std::numeric_limits<int32_t>::max());
    TEST2(String, "-2147483648", int32_t, std::numeric_limits<int32_t>::min());
    TEST2(String, "4294967295", uint32_t, std::numeric_limits<uint32_t>::max());

    TEST2(String, "9223372036854775807", int64_t, std::numeric_limits<int64_t>::max());
    TEST2(String, "-9223372036854775808", int64_t, std::numeric_limits<int64_t>::min());
    TEST2(String, "18446744073709551615", uint64_t, std::numeric_limits<uint64_t>::max());

    TEST2(String, "1.1", double, 1.1);
    TEST2(String, "1.1e+100", double, 1.1e100);
    TEST2(String, "1.1e-100", double, 1.1e-100);

    TEST(double, 1.1e100, String, "1.1E+100");

    *out << "String Parsing\n";

    TEST(int32_t, 15, String, "0xf");
    TEST(int32_t, 15, String, "0xF");
    TEST(int32_t, -15, String, "-0xF");
    TEST(int32_t, 16, String, "0x10");
    TEST(int32_t, -16, String, "-0x10");

    TEST(int32_t, 7, String, "07");
    TEST(int32_t, 8, String, "010");
    TEST(int32_t, -7, String, "-07");
    TEST(int32_t, -8, String, "-010");

    TEST(int64_t, 15, String, "0xf");
    TEST(int64_t, 15, String, "0xF");
    TEST(int64_t, -15, String, "-0xF");
    TEST(int64_t, 16, String, "0x10");
    TEST(int64_t, -16, String, "-0x10");

    TEST(int64_t, 7, String, "07");
    TEST(int64_t, 8, String, "010");
    TEST(int64_t, -7, String, "-07");
    TEST(int64_t, -8, String, "-010");

    *out << "String parsing errors\n";

    FAIL(int32_t, String, "hello!");
    FAIL(int32_t, String, "42 is the answer");
    FAIL(int64_t, String, "hello!");
    FAIL(int64_t, String, "42 is the answer");
    FAIL(double, String, "hello!");
    FAIL(double, String, "42 is the answer");

    FAIL(int8_t, String, "1000");
    FAIL(int8_t, String, "-1000");

    FAIL(double, String, "1e+10000000");

    *out << "Floating point overflows\n";

    TEST(float, FLT_MAX, double, 1e300);
    TEST(float, -FLT_MAX, double, -1e300);
    TEST(float, FLT_MIN, double, 1e-300);
    TEST(float, -FLT_MIN, double, -1e-300);
    xfloat = ::epics::pvData::castUnsafe<float,double>(epicsNAN);
    *out << "Cast double NAN to float NAN yields: "<<xfloat<<"\n";
    fail |= !isnan( xfloat );

    return fail ? 1 : 0;
}
