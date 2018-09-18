/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */

#include <sstream>
#include <vector>

#include <testMain.h>
#include <epicsString.h>

#include <pv/pvUnitTest.h>
#include <pv/current_function.h>

#include <pv/bitSet.h>
#include <pv/pvData.h>
#include <pv/standardField.h>

#if EPICS_VERSION_INT>=VERSION_INT(3,15,0,1)
#  define USE_JSON
#endif

namespace pvd = epics::pvData;

typedef std::vector<std::string> lines_t;

namespace {

lines_t lines(const std::string& str)
{
    lines_t ret;
    size_t p = 0;
    while(true) {
        size_t next = str.find_first_of('\n', p);
        ret.push_back(str.substr(p, next-p)); // exclude trailing '\n'
        if(next==str.npos)
            break;
        else
            p = next+1;
    }
    return ret;
}

std::string escape(const std::string& inp) {
    size_t N = epicsStrnEscapedFromRawSize(inp.c_str(), inp.size());
    std::vector<char> esc(N+1, '\0'); // include space for trailing nil
    epicsStrnEscapedFromRaw(&esc[0], esc.size(), inp.c_str(), inp.size());
    return std::string(&esc[0]);
}

std::string print(const pvd::PVStructure::Formatter& fmt)
{
    std::ostringstream strm;
    strm<<fmt;
    return strm.str();
}

struct point {
    size_t L, R;
};

// really primative diff
// expect -> actual
::detail::testPassx
testDiff(const std::string& expect, const std::string& actual, const std::string& msg = std::string())
{
    bool match = expect==actual;
    ::detail::testPassx ret(match);
    ret<<msg<<'\n';

    lines_t lhs(lines(expect)), rhs(lines(actual));

    size_t L=0, R=0;

    while(L<lhs.size() && R<rhs.size()) {
        if(lhs[L]==rhs[R]) {
            ret<<"  "<<escape(lhs[L])<<'\n';
            L++;
            R++;

        } else {
            // ugly... diagonalization hardcoded...
            static const point search[] = {
                {1,0},
                {0,1},
                {1,1},
                {2,1},
                {1,2},
                {2,2},
            };


            size_t Lp, Rp;
            for(size_t n=0, N=sizeof(search)/sizeof(search[0]); n<N; n++) {
                Lp = std::min(L+search[n].L, lhs.size()-1u);
                Rp = std::min(R+search[n].R, rhs.size()-1u);

                if(lhs[Lp]==rhs[Rp])
                    break;
            }

            for(size_t l=L; l<Lp; l++)
                ret<<"- "<<escape(lhs[l])<<'\n';
            for(size_t r=R; r<Rp; r++)
                ret<<"+ "<<escape(rhs[r])<<'\n';
            L = Lp;
            R = Rp;
            // loop around and print matching line
        }
    }

    for(; L<lhs.size(); L++)
        ret<<"- "<<escape(lhs[L])<<'\n';
    for(; R<rhs.size(); R++)
        ret<<"+ "<<escape(rhs[R])<<'\n';

    return ret;
}

static const pvd::StructureConstPtr scalarNumeric(pvd::getFieldCreate()->createFieldBuilder()
                                                  ->setId("epics:nt/NTScalar:1.0")
                                                  ->add("value", pvd::pvInt)
                                                  ->add("alarm", pvd::getStandardField()->alarm())
                                                  ->add("timeStamp", pvd::getStandardField()->timeStamp())
                                                  ->createStructure());

void showNTScalarNumeric()
{
    testDiag("%s", CURRENT_FUNCTION);
    pvd::PVStructurePtr input(pvd::getPVDataCreate()->createPVStructure(scalarNumeric));
    input->getSubFieldT<pvd::PVScalar>("value")->putFrom(-42);

    testDiff("<undefined> -42 \n", print(input->stream()));

    input->getSubFieldT<pvd::PVScalar>("alarm.severity")->putFrom(1);
    input->getSubFieldT<pvd::PVScalar>("alarm.status")->putFrom(1);
    input->getSubFieldT<pvd::PVString>("alarm.message")->put("FOO");

    testDiff("<undefined> -42 MINOR DEVICE FOO \n", print(input->stream()));
}

static const pvd::StructureConstPtr scalarString(pvd::getFieldCreate()->createFieldBuilder()
                                                  ->setId("epics:nt/NTScalar:1.0")
                                                  ->add("value", pvd::pvString)
                                                  ->add("alarm", pvd::getStandardField()->alarm())
                                                  ->add("timeStamp", pvd::getStandardField()->timeStamp())
                                                  ->createStructure());

void showNTScalarString()
{
    testDiag("%s", CURRENT_FUNCTION);
    pvd::PVStructurePtr input(pvd::getPVDataCreate()->createPVStructure(scalarString));
    testDiff("<undefined>  \n", print(input->stream()));

    input->getSubFieldT<pvd::PVString>("value")->put("bar");

    testDiff("<undefined> bar \n", print(input->stream()));

    input->getSubFieldT<pvd::PVScalar>("alarm.severity")->putFrom(1);
    input->getSubFieldT<pvd::PVScalar>("alarm.status")->putFrom(1);
    input->getSubFieldT<pvd::PVString>("alarm.message")->put("FOO");

    testDiff("<undefined> bar MINOR DEVICE FOO \n", print(input->stream()));
}

static const pvd::StructureConstPtr everything(pvd::getFieldCreate()->createFieldBuilder()
                                               ->setId("omg")
                                               ->add("scalar", pvd::pvString)
                                               ->addArray("scalarArray", pvd::pvString)
                                               ->addNestedStructure("below")
                                                    ->add("A", pvd::pvInt)
                                                    ->addNestedUnion("select")
                                                        ->add("one", pvd::pvInt)
                                                        ->add("two", pvd::pvInt)
                                                    ->endNested()
                                                    ->addNestedUnionArray("arrselect")
                                                        ->add("foo", pvd::pvInt)
                                                        ->add("bar", pvd::pvInt)
                                                    ->endNested()
                                                    ->addNestedStructureArray("astruct")
                                                       ->add("red", pvd::pvInt)
                                                       ->add("blue", pvd::pvInt)
                                                   ->endNested()
                                               ->endNested()
                                               ->add("anything", pvd::getFieldCreate()->createVariantUnion())
                                               ->add("arrayany", pvd::getFieldCreate()->createVariantUnionArray())
                                               ->createStructure());

void testRaw()
{
    testDiag("%s", CURRENT_FUNCTION);
    pvd::PVStructurePtr input(pvd::getPVDataCreate()->createPVStructure(everything));

    testDiff("omg \n"
             "    string scalar \n"
             "    string[] scalarArray []\n"
             "    structure below\n"
             "        int A 0\n"
             "        union select\n"
             "            (none)\n"
             "        union[] arrselect\n"
             "        structure[] astruct\n"
             "    any anything\n"
             "        (none)\n"
             "    any[] arrayany\n"
             , print(input->stream()));

    testDiff("omg \n"
             "    string scalar \n"
             "    structure below\n"
             "        int A 0\n"
             , print(input->stream().show(pvd::BitSet().set(1).set(4))));

    testDiff("omg \n"
             "\033[1m    string scalar \n"
             "\033[0m\033[1m    string[] scalarArray []\n"
             "\033[0m    structure below\n"
             "\033[1m        int A 0\n"
             "\033[0m        union select\n"
             "            (none)\n"
             "        union[] arrselect\n"
             "        structure[] astruct\n"
             "    any anything\n"
             "        (none)\n"
             "    any[] arrayany\n"
             , print(input->stream()
                     .mode(pvd::PVStructure::Formatter::ANSI) // force use of escapes
                     .highlight(pvd::BitSet().set(1).set(2).set(4))
                     ));
}

} // namespace

MAIN(testprinter)
{
    testPlan(0);
    showNTScalarNumeric();
    showNTScalarString();
    testRaw();
    return testDone();
}
