/* pvDataPy.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#ifndef PVDATAPY_H
#define PVDATAPY_H
#include <pv/pvData.h>
namespace epics { namespace pvData { 

class PVFieldPy;
class PVScalarPy;
class PVScalarArrayPy;
class PVStructureArrayPy;
class PVStructurePy;
class PVTopPy;
    

typedef std::tr1::shared_ptr<PVFieldPy> PVFieldPyPtr;
typedef std::tr1::shared_ptr<PVScalarPy> PVScalarPyPtr;
typedef std::tr1::shared_ptr<PVScalarArrayPy> PVScalarArrayPyPtr;
typedef std::tr1::shared_ptr<PVStructureArrayPy> PVStructureArrayPyPtr;
typedef std::tr1::shared_ptr<PVStructurePy> PVStructurePyPtr;
typedef std::tr1::shared_ptr<PVTopPy> PVTopPyPtr;

typedef std::vector<PVFieldPyPtr> PVFieldPyPtrArray;
typedef std::tr1::shared_ptr<PVFieldPyPtrArray> PVFieldPyPtrArrayPtr;

class PVFieldPy {
public:
    Type getType(){return type;};
    virtual ~PVFieldPy(){}
protected:
    PVFieldPy(Type type) : type(type){}
private:
    Type type;
};

class PVScalarPy : public PVFieldPy {
public:
    virtual ~PVScalarPy(){}
    PVScalarPtr const &getPVScalar() {return pvScalar;}
private:
    PVScalarPy(PVScalarPtr const & pvScalarPtr)
    : PVFieldPy(pvScalarPtr->getField()->getType()),
      pvScalar(pvScalarPtr)
      {}

    PVScalarPtr pvScalar;
    friend class PVTopPy;
};


class PVScalarArrayPy : public PVFieldPy {
public:
    virtual ~PVScalarArrayPy(){}
    PVScalarArrayPtr const &getPVScalarArray() {return pvScalarArray;}
private:
    PVScalarArrayPy(PVScalarArrayPtr const & pvScalarArrayPtr)
    : PVFieldPy(pvScalarArrayPtr->getField()->getType()),
      pvScalarArray(pvScalarArrayPtr)
      {}

    PVScalarArrayPtr pvScalarArray;
    friend class PVTopPy;
};

class PVStructureArrayPy : public PVFieldPy {
public:
    virtual ~PVStructureArrayPy(){}
    PVStructureArrayPtr const &getPVStructureArray() {return pvStructureArray;}
private:
    PVStructureArrayPy(PVStructureArrayPtr const & pvStructureArrayPtr)
    : PVFieldPy(pvStructureArrayPtr->getField()->getType()),
      pvStructureArray(pvStructureArrayPtr)
      {}

    PVStructureArrayPtr pvStructureArray;
    friend class PVTopPy;
};


class PVStructurePy : public PVFieldPy {
public:
    virtual ~PVStructurePy(){}
    PVFieldPyPtrArray const & getPVFieldPyPtrArray()
    {return pvPyFields;}
    PVStructurePtr const & getPVStructurePtr()
    {return pvStructurePtr;}
private:
    PVStructurePy(PVStructurePtr const &pvStructurePtr)
       : PVFieldPy(structure),
         pvStructurePtr(pvStructurePtr)
         {}
    PVStructurePtr const &pvStructurePtr;
    PVFieldPyPtrArray pvPyFields;
    friend class PVTopPy;
};

class PVTopPy {
public:
    static PVTopPyPtr createTop(PVStructurePtr const & pvStructure);
    PVStructurePyPtr const &  getPVStructurePy(){return pvStructurePyPtr;}
private:
    static PVStructurePyPtr create(PVStructurePtr const & pvStructure);
    PVTopPy(PVStructurePyPtr const &pvStructurePyPtr)
    : pvStructurePyPtr(pvStructurePyPtr)
      {}

    PVStructurePyPtr pvStructurePyPtr;
};

    
}}
#endif  /* PVDATAPY_H */
