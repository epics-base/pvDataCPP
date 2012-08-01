#include <iostream>

#include <pv/pvData.h>
#include <pv/standardField.h>
#include <pv/standardPVField.h>

using namespace epics::pvData;

struct StructureHandle;

struct FieldVariant
{
    char * name;
    Type t;
    ScalarType st;
    StructureHandle * sh;
};

struct StructureHandle
{
    StructureConstPtr s;
    StructureHandle(StructureConstPtr s) : s(s) {}
};

struct PVStructureHandle
{
    PVStructurePtr s;
    PVStructureHandle(PVStructurePtr s) : s(s) {}
};

template<typename T> void putField(void * handle, char * name, T value)
{
    PVStructureHandle * ph = (PVStructureHandle *)handle;
    std::tr1::shared_ptr<PVScalarValue<T> > a = 
        std::tr1::static_pointer_cast<PVScalarValue<T> >(ph->s->getSubField(name));
    a->put(value);
}

#define MAKEputField(T) \
    void putField_##T(void * handle, char * name, T value) \
    {                                                     \
        putField(handle, name, value);                    \
    }

extern "C"
{
    double * getDoubleArray(void * handle, char * name)
    {
        PVStructureHandle * ph = (PVStructureHandle *)handle;
        PVScalarArrayPtr p = ph->s->getScalarArrayField(name, pvDouble);
        PVDoubleArrayPtr d = std::tr1::static_pointer_cast<PVDoubleArray>(p);
        return d->get();
    }
    int getLength(void * handle, char * name)
    {
        PVStructureHandle * ph = (PVStructureHandle *)handle;
        PVFieldPtr p = ph->s->getSubField(name);
        PVScalarArrayPtr a = std::tr1::static_pointer_cast<PVScalarArray>(p);
        return a->getLength();
    }
    void setLength(void * handle, char * name, int length)
    {
        PVStructureHandle * ph = (PVStructureHandle *)handle;
        PVFieldPtr p = ph->s->getSubField(name);
        PVScalarArrayPtr a = std::tr1::static_pointer_cast<PVScalarArray>(p);
        return a->setLength(length);
    }
    
    int getInt(void * handle, char * name)
    {
        PVStructureHandle * ph = (PVStructureHandle *)handle;
        return ph->s->getIntField(name)->get();
    }

    MAKEputField(int);
    MAKEputField(double);
    MAKEputField(short);
    
    void PVStructuretoString(void * handle)
    {
        PVStructureHandle * ph = (PVStructureHandle *)handle;
        std::string text;
        ph->s->toString(&text);
        std::cout << text << std::endl;
    }
    
    void * createPVStructure(void * handle)
    {
        StructureHandle * sh = (StructureHandle *)handle;
        PVDataCreatePtr pvDataCreate = getPVDataCreate();
        PVStructurePtr sp = pvDataCreate->createPVStructure(sh->s);
        PVStructureHandle * ph = new PVStructureHandle(sp);
        return (void *)ph;
    }

    void * createStructureVariant(FieldVariant * fieldVars, int length)
    {
        FieldCreatePtr fieldCreate = getFieldCreate();
        FieldConstPtrArray fields;
        StringArray fieldNames;
        for(int n = 0; n < length; n++)
        {
            fieldNames.push_back(fieldVars[n].name);
            ScalarType t = (ScalarType)fieldVars[n].st;
            Type bt = (Type)fieldVars[n].t;
            if(bt == scalarArray)
            {
                fields.push_back(fieldCreate->createScalarArray(t));
            }
            else if(bt == scalar)
            {
                fields.push_back(fieldCreate->createScalar(t));
            }
            else if(bt == structure)
            {
                fields.push_back(fieldVars[n].sh->s);
            }
            else if(bt == structureArray)
            {
                fields.push_back(fieldCreate->createStructureArray(fieldVars[n].sh->s));
            }
        }
        StructureConstPtr s = fieldCreate->createStructure(fieldNames, fields);
        StructureHandle * sh = new StructureHandle(s);
        return (void *)sh;
    }

    void * createTest()
    {
        FieldCreatePtr fieldCreate = getFieldCreate();
        PVDataCreatePtr pvDataCreate = getPVDataCreate();
        FieldConstPtrArray fields;
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

    int getFieldType(void * handle, int n)
    {
        StructureHandle * sh = (StructureHandle *)handle;
        return sh->s->getField(n)->getType();
    }

    int getScalarType(void * handle, int n)
    {
        StructureHandle * sh = (StructureHandle *)handle;
        FieldConstPtr f = sh->s->getField(n);
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

    int getElementType(void * handle, int n)
    {
        StructureHandle * sh = (StructureHandle *)handle;
        FieldConstPtr f = sh->s->getField(n);
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
