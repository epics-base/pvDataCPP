/* pvDataCAPICreateTest.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#include <pv/pvDataCAPICreateTest.h>
#include <pv/pvDataCAPI.h>
#include <pv/standardField.h>
#include <pv/pvIntrospect.h>
#include <pv/standardPVField.h>

using namespace epics::pvData;

template<typename T> struct ConstHandle
{
    typedef std::tr1::shared_ptr<const T> ConstPtr;
    ConstPtr p;
    ConstHandle(ConstPtr p) : p(p) {}
    static ConstPtr get(void * p);
};

template<typename T> std::tr1::shared_ptr<const T> 
ConstHandle<T>::get(void * p)
{
    ConstHandle<T> * h = static_cast<ConstHandle<T> *>(p);
    return h->p;
}

template<typename T> ConstHandle<T> * makeHandle(
    std::tr1::shared_ptr<const T> p)
{
    return new ConstHandle<T>(p);
}

extern "C"
{
    void * pvCAPICreateScalar(int scalarType,const char *properties)
    {
        StandardPVFieldPtr standardPVField = getStandardPVField();
        String xxx(properties);
        ScalarType yyy = static_cast<ScalarType>(scalarType);
        return PVStructureCAPI::create(standardPVField->scalar(yyy,xxx));
    }

    void * pvCAPICreateScalarArray(int elementType,const char *properties)
    {
        StandardPVFieldPtr standardPVField = getStandardPVField();
        String xxx(properties);
        ScalarType yyy = static_cast<ScalarType>(elementType);
        return PVStructureCAPI::create(standardPVField->scalarArray(yyy,xxx));
    }
    
    void * pvCAPIFieldCreateScalar(int scalarType)
    {
        return makeHandle(getFieldCreate()->createScalar(
                              static_cast<ScalarType>(scalarType)));
    }
    
    void * pvCAPIFieldCreateScalarArray(int scalarType)
    {
        return makeHandle(getFieldCreate()->createScalarArray(
                              static_cast<ScalarType>(scalarType)));
    }
    
    void * pvCAPIFieldCreateStructureArray(void * handle)
    {
        return makeHandle(getFieldCreate()->createStructureArray(
                              ConstHandle<Structure>::get(handle)));
    }

    void * pvCAPIFieldCreateStructure(char ** names,
                                      void ** handles, 
                                      int length)
    {
        FieldConstPtrArray fields;
        StringArray fieldNames;
        for(int n = 0; n < length; n++)
        {
            fieldNames.push_back(names[n]);
            fields.push_back(ConstHandle<Structure>::get(handles[n]));
        }
        return makeHandle(getFieldCreate()->createStructure(
                              fieldNames, fields));
    }

    void pvCAPIDumpStructure(void * handle)
    {
        ConstHandle<Structure> * p = 
            static_cast<ConstHandle<Structure> *>(handle);
        String buffer;
        p->p->toString(&buffer);
        printf("%s\n",buffer.c_str());
    }

}
