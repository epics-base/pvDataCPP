/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */

#include <vector>
#include <sstream>

#include <pv/pvdVersion.h>
#include <pv/pvData.h>
#include <pv/valueBuilder.h>

#if EPICS_VERSION_INT>=VERSION_INT(3,15,0,1)

#include <yajl_parse.h>

#define epicsExportSharedSymbols
#include "pv/json.h"

namespace pvd = epics::pvData;

namespace {
struct context {

    std::string msg;

    typedef std::vector<pvd::PVFieldPtr> stack_t;
    stack_t stack;

    context(const pvd::PVFieldPtr& root)
    {
        stack.push_back(root);
    }
};

#define TRY context *self = (context*)ctx; assert(!self->stack.empty()); try

#define CATCH() catch(std::exception& e) { self->msg = e.what(); return 0; }

int jtree_null(void * ctx)
{
    TRY {
        self->msg = "NULL value not permitted";
        return 0;
    }CATCH()
}

template<typename PVD>
void valueAssign(context *self, typename PVD::value_type val)
{
    pvd::Type type(self->stack.back()->getField()->getType());
    if(type==pvd::scalar) {
        pvd::PVScalar* fld(static_cast<pvd::PVScalar*>(self->stack.back().get()));
        if(!fld)
            throw std::invalid_argument("Not a scalar field");
        fld->putFrom(val);
        self->stack.pop_back();
        // structure back at the top of the stack

    } else if(type==pvd::scalarArray) {
        pvd::PVScalarArray* fld(static_cast<pvd::PVScalarArray*>(self->stack.back().get()));

        PVD* arrfld(dynamic_cast<PVD*>(fld));
        if(!arrfld)
            throw std::invalid_argument("wrong type for scalar array");

        typename PVD::const_svector carr;
        arrfld->swap(carr);

        typename PVD::svector arr(pvd::thaw(carr));

        arr.push_back(val);

        arrfld->replace(pvd::freeze(arr));

        // leave array field at top of stack
    } else {
        throw std::invalid_argument("Can't assign value");
    }
}

int jtree_boolean(void * ctx, int boolVal)
{
    TRY {
        valueAssign<pvd::PVBooleanArray>(self, !!boolVal);
        return 1;
    }CATCH()
}

int jtree_integer(void * ctx, long integerVal)
{
    TRY {
        valueAssign<pvd::PVLongArray>(self, integerVal);
        return 1;
    }CATCH()
}

int jtree_double(void * ctx, double doubleVal)
{
    TRY {
        valueAssign<pvd::PVDoubleArray>(self, doubleVal);
        return 1;
    }CATCH()
}

int jtree_string(void * ctx, const unsigned char * stringVal,
                    unsigned int stringLen)
{
    TRY {
        std::string val((const char*)stringVal, stringLen);
        valueAssign<pvd::PVStringArray>(self, val);
        return 1;
    }CATCH()
}

int jtree_start_map(void * ctx)
{
    TRY {
        pvd::PVFieldPtr& back(self->stack.back());
        pvd::Type type = back->getField()->getType();
        if(type==pvd::structure) {
            // will fill in
        } else if(type==pvd::structureArray) {
            // starting new element in structure array
            pvd::PVStructureArrayPtr sarr(std::tr1::static_pointer_cast<pvd::PVStructureArray>(back));

            pvd::PVStructurePtr elem(pvd::getPVDataCreate()->createPVStructure(sarr->getStructureArray()->getStructure()));

            self->stack.push_back(elem);
        } else {
            throw std::runtime_error("Can't map (sub)structure");
        }

        assert(self->stack.back()->getField()->getType()==pvd::structure);
        return 1;
    }CATCH()
}

int jtree_map_key(void * ctx, const unsigned char * key,
                     unsigned int stringLen)
{
    TRY {
        std::string name((const char*)key, stringLen);

        // start_map() ensures we have a structure at the top of the stack
        pvd::PVStructure *fld = static_cast<pvd::PVStructure*>(self->stack.back().get());

        try {
            self->stack.push_back(fld->getSubFieldT(name));
        }catch(std::runtime_error& e){
            std::ostringstream strm;
            strm<<"At "<<fld->getFullName()<<" : "<<e.what()<<"\n";
            throw std::runtime_error(strm.str());
        }

        return 1;
    }CATCH()
}

int jtree_end_map(void * ctx)
{
    TRY {
        assert(self->stack.back()->getField()->getType()==pvd::structure);

        pvd::PVStructurePtr elem(std::tr1::static_pointer_cast<pvd::PVStructure>(self->stack.back()));
        self->stack.pop_back();

        if(!self->stack.empty() && self->stack.back()->getField()->getType()==pvd::structureArray) {
            // append element to struct array
            pvd::PVStructureArray *sarr = static_cast<pvd::PVStructureArray*>(self->stack.back().get());

            pvd::PVStructureArray::const_svector cval;
            sarr->swap(cval);

            pvd::PVStructureArray::svector val(pvd::thaw(cval));

            val.push_back(elem);

            sarr->replace(pvd::freeze(val));
        }

        return 1;
    }CATCH()
}

int jtree_start_array(void * ctx)
{
    TRY {
        pvd::PVFieldPtr& back(self->stack.back());
        pvd::Type type = back->getField()->getType();
        if(type!=pvd::structureArray && type!=pvd::scalarArray)
            throw std::runtime_error("Can't assign array");

        return 1;
    }CATCH()
}
int jtree_end_array(void * ctx)
{
    TRY {
        self->stack.pop_back();
        return 1;
    }CATCH()
}


yajl_callbacks jtree_cbs = {
    &jtree_null,
    &jtree_boolean,
    &jtree_integer,
    &jtree_double,
    NULL, // number
    &jtree_string,
    &jtree_start_map,
    &jtree_map_key,
    &jtree_end_map,
    &jtree_start_array,
    &jtree_end_array,
};

struct handler {
    yajl_handle handle;
    handler(yajl_handle handle) :handle(handle)
    {
        if(!handle)
            throw std::runtime_error("Failed to allocate yajl handle");
    }
    ~handler() {
        yajl_free(handle);
    }
    operator yajl_handle() { return handle; }
};

} // namespace

namespace epics{namespace pvData{

epicsShareFunc
void parseJSON(std::istream& strm,
               const PVField::shared_pointer& dest)
{
    yajl_parser_config conf = {
        .allowComments = 1,
        .checkUTF8 = 1,
    };

    context ctxt(dest);

    handler handle(yajl_alloc(&jtree_cbs, &conf, NULL, &ctxt));

    if(!yajl_parse_helper(strm, handle, conf))
        throw std::runtime_error(ctxt.msg);

    if(!ctxt.stack.empty())
        throw std::logic_error("field stack not empty");
}

}} // namespace epics::pvData

#endif // EPICS_VERSION_INT

