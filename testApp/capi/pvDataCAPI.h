/* pvDataCAPI.h */
/**
 * Copyright - See the Copyright that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#ifndef PVDATACAPI_H
#define PVDATACAPI_H

#ifdef __cplusplus

#include <pv/pvData.h>
namespace epics { namespace pvData { 

class PVFieldCAPI;
class PVScalarCAPI;
class PVScalarArrayCAPI;
class PVStructureArrayCAPI;
class PVStructureCAPI;
    

typedef std::tr1::shared_ptr<PVFieldCAPI> PVFieldCAPIPtr;
typedef std::tr1::shared_ptr<PVScalarCAPI> PVScalarCAPIPtr;
typedef std::tr1::shared_ptr<PVScalarArrayCAPI> PVScalarArrayCAPIPtr;
typedef std::tr1::shared_ptr<PVStructureArrayCAPI> PVStructureArrayCAPIPtr;
typedef std::tr1::shared_ptr<PVStructureCAPI> PVStructureCAPIPtr;

typedef std::vector<PVFieldCAPIPtr> PVFieldCAPIPtrArray;
typedef std::tr1::shared_ptr<PVFieldCAPIPtrArray> PVFieldCAPIPtrArrayPtr;

class PVFieldCAPI {
public:
    Type getType(){return type;};
    virtual ~PVFieldCAPI(){}
protected:
    PVFieldCAPI(Type type);
private:
    Type type;
};

class PVScalarCAPI : public PVFieldCAPI {
public:
    /**
     * Create a PVScalarCAPI
     * &param pvScalarPtr The PVScalarPtr
     * @return void *addrPVScalarCAPI
     */
    static void *create(PVScalarPtr const & pvScalarPtr);
    static PVScalarCAPIPtr getPVScalarCAPIPtr(void *addrPVScalarCAPI);
    static void destroy(void *addrPVScalarCAPI);
    virtual ~PVScalarCAPI(){}
    PVScalarPtr getPVScalarPtr() {return pvScalarPtr;}
private:
    PVScalarCAPI(PVScalarPtr const & pvScalarPtr);
    PVScalarPtr pvScalarPtr;
    friend class PVStructureCAPI;
};


class PVScalarArrayCAPI : public PVFieldCAPI {
public:
    static void *create(PVScalarArrayPtr const & pvScalarArrayPtr);
    static PVScalarArrayCAPIPtr getPVScalarArrayCAPIPtr(
        void *addrPVScalarArrayCAPI);
    static void destroy(void *addrPVScalarArrayCAPI);
    virtual ~PVScalarArrayCAPI(){}
    PVScalarArrayPtr getPVScalarArrayPtr() {return pvScalarArrayPtr;}
private:
    PVScalarArrayCAPI(PVScalarArrayPtr const & pvScalarArrayPtr);
    PVScalarArrayPtr pvScalarArrayPtr;
    friend class PVStructureCAPI;
};

class PVStructureArrayCAPI : public PVFieldCAPI {
public:
    static void *create(PVStructureArrayPtr const & pvStructureArrayPtr);
    static PVStructureArrayCAPIPtr getPVStructureArrayCAPIPtr(
        void *addrPVStructureArrayCAPI);
    static void destroy(void *addrPVStructureArrayCAPI);
    virtual ~PVStructureArrayCAPI(){}
    PVStructureArrayPtr getPVStructureArrayPtr() {return pvStructureArrayPtr;}
private:
    PVStructureArrayCAPI(PVStructureArrayPtr const & pvStructureArrayPtr);
    PVStructureArrayPtr pvStructureArrayPtr;
    friend class PVStructureCAPI;
};


class PVStructureCAPI : public PVFieldCAPI {
public:
    /**
     * Create a PVStructureCAPI
     * &param pvStructurePtr The PVStructurePtr
     * @return void *addrPVStructureCAPI
     */
    static void *create(PVStructurePtr const &pvStructurePtr);
    static PVStructureCAPIPtr getPVStructureCAPIPtr(void *addrPVStructureCAPI);
    static void destroy(void *addrPVStructureCAPI);
    virtual ~PVStructureCAPI(){}
    PVFieldCAPIPtrArrayPtr getPVFieldCAPIPtrArrayPtr() {return pvFieldCAPIPtrArrayPtr;}
    PVStructurePtr getPVStructurePtr() {return pvStructurePtr;}
private:
    static PVStructureCAPIPtr createPtr(PVStructurePtr const & pvStructurePtr);
    PVStructureCAPI(PVStructurePtr const &pvStructurePtr);
    PVStructurePtr pvStructurePtr;
    PVFieldCAPIPtrArrayPtr pvFieldCAPIPtrArrayPtr;
};
}}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    /**
     * Destroy the PVStructure.
     * @param addrPVStructureCAPIAddr PVStructureCAPIAddr holds the PVStructureCAPI.
     */
    extern void pvCAPIDestroyPVStructureCAPI(void *addrPVStructureCAPIAddr);
    /**
     * Dump the PVStructure.
     * @param addrPVStructureCAPIAddr PVStructureCAPIAddr holds the PVStructureCAPI.
     */
    extern void pvCAPIDumpPVStructureCAPI(void *addrPVStructureCAPIAddr);
    /**
     * Get the index of the subfield with the specified fieldName
     * @param addrPVStructureCAPIAddr PVStructureCAPIAddr holds the PVStructureCAPI.
     * @param fieldName The fieldname
     * @return The index or -1 if the field is not found.
     */
    extern int pvCAPIGetSubfield(void *addrPVStructureCAPIAddr, const char *fieldName);
    /**
     * Get the number of subfields
     * @param addrPVStructureCAPIAddr PVStructureCAPIAddr holds the PVStructureCAPI.
     * @return The number of subfields.
     */
    extern size_t pvCAPIGetNumberFields(void *addrPVStructureCAPIAddr);
    /**
     * Get the field type
     * @param addrPVStructureCAPIAddr PVStructureCAPIAddr holds the PVStructureCAPI.
     * @param index The subfield.
     * @return The field type.
     */
    extern int pvCAPIGetFieldType(void *addrPVStructureCAPIAddr, size_t index);
    /**
     * Get the field name
     * @param addrPVStructureCAPIAddr PVStructureCAPIAddr holds the PVStructureCAPI.
     * @param index The subfield.
     * @return The field name.
     */
    extern const char *pvCAPIGetFieldName(void *addrPVStructureCAPIAddr, size_t index);
    /**
     * Get a scalar subfield.
     * @param addrPVStructureCAPIAddr PVStructureCAPIAddr holds the PVStructureCAPI.
     * @param index The subfield.
     * @return void * addrPVScalarCAPIAddr.
     */
    extern void * pvCAPIGetPVScalarCAPI(void *addrPVStructureCAPIAddr, size_t index);
    /**
     * Destroy the PVScalar.
     * @param addrPVScalarCAPIAddr PVScalarCAPIAddr holds the PVScalarCAPI.
     */
    extern void pvCAPIDestroyPVScalarCAPI(void *addrPVScalarCAPIAddr);
    /**
     * Given a PVScalarCAPI get the scalarType
     * @param addrPVScalarCAPIAddr PVScalarCAPIAddr holds the PVScalarCAPI.
     * @return The type.
     */
    extern int pvCAPIScalarGetScalarType(void * addrPVScalarCAPIAddr);
    /**
     * Given a PVScalar get the value converted to integer.
     * @param addrPVScalarCAPIAddr PVScalarCAPIAddr holds the PVScalarCAPI.
     * @return The value converted to int.
     */
    extern int pvCAPIScalarGetInt(void * addrPVScalarCAPIAddr);
    /**
     * Given a PVScalar put the value
     * @param addrPVScalarCAPIAddr PVScalarCAPIAddr holds the PVScalarCAPI.
     * @param value The value.
     */
    extern void pvCAPIScalarPutInt(void * addrPVScalarCAPIAddr, int value);
    /**
     * Given a PVScalar get the value converted to double.
     * @param addrPVScalarCAPIAddr PVScalarCAPIAddr holds the PVScalarCAPI.
     * @return The value converted to double.
     */
    extern double pvCAPIScalarGetDouble(void * addrPVScalarCAPIAddr);
    /**
     * Given a PVScalar put the value
     * @param addrPVScalarCAPIAddr PVScalarCAPIAddr holds the PVScalarCAPI.
     * @param value The value.
     */
    extern void pvCAPIScalarPutDouble(void * addrPVScalarCAPIAddr, double value);
    /**
     * Get a scalar array subfield.
     * @param addrPVStructureCAPIAddr PVStructureCAPIAddr holds the PVStructureCAPI.
     * @param index The subfield.
     * @return void * addrPVScalarArrayCAPIAddr.
     */
    extern void * pvCAPIGetPVScalarArrayCAPI(void *addrPVStructureCAPIAddr, size_t index);
    /**
     * Destroy the PVScalarArray.
     * @param addrPVScalarArrayCAPIAddr PVScalarArrayCAPIAddr holds the PVScalarArrayCAPI.
     */
    extern void pvCAPIDestroyPVScalarArrayCAPI(void *addrPVScalarArrayCAPIAddr);
    /**
     * Get a structure array subfield.
     * @param addrPVStructureCAPIAddr PVStructureCAPIAddr holds the PVStructureCAPI.
     * @param index The subfield.
     * @return void * addrPVStructureArrayCAPIAddr.
     */
    extern void * pvCAPIGetPVStructureArrayCAPI(void *addrPVStructureCAPIAddr, size_t index);
    /**
     * Destroy the PVStructureArray.
     * @param addrPVStructureArrayCAPIAddr PVStructureArrayCAPIAddr holds the PVStructureArrayCAPI.
     */
    extern void pvCAPIDestroyPVStructureArrayCAPI(void *addrPVStructureArrayCAPIAddr);
    /**
     * Get a structure subfield.
     * @param addrPVStructureCAPIAddr PVStructureCAPIAddr holds the PVStructureCAPI.
     * @param index The subfield.
     * @return void * addrPVStructureCAPIAddr.
     */
    extern void * pvCAPIGetPVStructureCAPI(void *addrPVStructureCAPIAddr, size_t index);
#ifdef __cplusplus
}
#endif


    
#endif  /* PVDATACAPI_H */
