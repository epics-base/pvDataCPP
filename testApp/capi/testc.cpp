#include <pv/pvData.h>
#include <pv/standardField.h>
#include <pv/standardPVField.h>

using namespace epics::pvData;

struct StructureHandle
{
    StructureConstPtr s;
    StructureHandle(StructureConstPtr s) : s(s) {}
};

extern "C"
{
    void * createTest()
    {
        FieldCreatePtr fieldCreate = getFieldCreate();
        PVDataCreatePtr pvDataCreate = getPVDataCreate();
        FieldConstPtrArray  fields;
        StringArray fieldNames;
        fields.push_back(fieldCreate->createScalar(pvString));
        fields.push_back(fieldCreate->createScalarArray(pvDouble));
        fieldNames.push_back("name");
        fieldNames.push_back("value");
        StructureConstPtr s = fieldCreate->createStructure(fieldNames, fields);
        StructureHandle * sh = new StructureHandle(s);
        return (void *)sh;
    }

    int getNumberFields(void * handle)
    {
        StructureHandle * sh = (StructureHandle *)handle;
        return sh->s->getNumberFields();
    }

    const char * getFieldName(void * handle, int n)
    {
        StructureHandle * sh = (StructureHandle *)handle;
        return sh->s->getFieldNames()[n].c_str();
    }

    int getFieldType(void * handle, const char * name)
    {
        StructureHandle * sh = (StructureHandle *)handle;
        /* api missing const for getField(size_t) so have to use name */
        return sh->s->getField(name)->getType();
    }

    int getScalarType(void * handle, const char * name)
    {
        StructureHandle * sh = (StructureHandle *)handle;
        FieldConstPtr f = sh->s->getField(name);
        if(f->getType() == scalar)
        {
            ScalarConstPtr sp = std::tr1::static_pointer_cast<const Scalar>(f);
            return sp->getScalarType();
        }
        else
        {
            return -1;
        }
    }

    int getElementType(void * handle, const char * name)
    {
        StructureHandle * sh = (StructureHandle *)handle;
        FieldConstPtr f = sh->s->getField(name);
        if(f->getType() == scalarArray)
        {
            ScalarArrayConstPtr sp = std::tr1::static_pointer_cast<const ScalarArray>(f);
            return sp->getElementType();
        }
        else
        {
            return -1;
        }
    }

    void destroy(void * handle)
    {
        StructureHandle * sh = (StructureHandle *)handle;
        delete sh;
    }

};


