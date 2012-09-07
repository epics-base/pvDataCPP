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
    PVFieldPy(Type type);
private:
    Type type;
};

class PVScalarPy : public PVFieldPy {
public:
    virtual ~PVScalarPy(){}
    PVScalarPtr getPVScalarPtr() {return pvScalarPtr;}
    PVScalarPtr *addrPVScalarPtr() {return &pvScalarPtr;}
private:
    PVScalarPy(PVScalarPtr const & pvScalarPtr);
    PVScalarPtr pvScalarPtr;
    friend class PVTopPy;
};


class PVScalarArrayPy : public PVFieldPy {
public:
    virtual ~PVScalarArrayPy(){}
    PVScalarArrayPtr getPVScalarArrayPtr() {return pvScalarArrayPtr;}
    PVScalarArrayPtr *addrPVScalarArrayPtr() {return &pvScalarArrayPtr;}
private:
    PVScalarArrayPy(PVScalarArrayPtr const & pvScalarArrayPtr);
    PVScalarArrayPtr pvScalarArrayPtr;
    friend class PVTopPy;
};

class PVStructureArrayPy : public PVFieldPy {
public:
    virtual ~PVStructureArrayPy(){}
    PVStructureArrayPtr getPVStructureArrayPtr() {return pvStructureArrayPtr;}
    PVStructureArrayPtr *addrPVStructureArrayPtr() {return &pvStructureArrayPtr;}
private:
    PVStructureArrayPy(PVStructureArrayPtr const & pvStructureArrayPtr);
    PVStructureArrayPtr pvStructureArrayPtr;
    friend class PVTopPy;
};


class PVStructurePy : public PVFieldPy {
public:
    virtual ~PVStructurePy(){}
    PVFieldPyPtrArrayPtr getPVFieldPyPtrArrayPtr() {return pvFieldPyPtrArrayPtr;}
    PVStructurePtr getPVStructurePtr() {return pvStructurePtr;}
    PVFieldPyPtrArrayPtr * addrPVFieldPyPtrArrayPtr() {return &pvFieldPyPtrArrayPtr;}
    PVStructurePtr *addrPVStructurePtr() {return &pvStructurePtr;}
private:
    PVStructurePy(PVStructurePtr const &pvStructurePtr);
    PVStructurePtr pvStructurePtr;
    PVFieldPyPtrArrayPtr pvFieldPyPtrArrayPtr;
    friend class PVTopPy;
};

class PVTopPy {
public:
    static PVTopPyPtr createTop(PVStructurePtr const & pvStructurePtr);
    PVStructurePyPtr getPVStructurePyPtr(){return pvStructurePyPtr;}
    PVStructurePyPtr *addrPVStructurePyPtr(){return &pvStructurePyPtr;};
private:
    static PVStructurePyPtr create(PVStructurePtr const & pvStructurePtr);
    PVTopPy(PVStructurePyPtr const & pvStructurePyPtr);
    PVStructurePyPtr pvStructurePyPtr;
};

    
}}
#endif  /* PVDATAPY_H */
