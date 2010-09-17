/*PVDataCreateFactory.cpp*/
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "pvData.h"
#include "factory.h"
#include "AbstractPVField.h"

namespace epics { namespace pvData {

   static std::string notImplemented("not implemented");

   PVDataCreate::PVDataCreate(){};

   PVField *PVDataCreate::createPVField(PVStructure *parent,
           FieldConstPtr field) const
   {
       throw std::logic_error(notImplemented);
   };

   PVField *PVDataCreate::createPVField(PVStructure *parent,
           StringConstPtr fieldName,FieldConstPtr fieldToClone) const
   {
        throw std::logic_error(notImplemented);
   };

   PVScalar *PVDataCreate::createPVScalar(PVStructure *parent,ScalarConstPtr scalar) const
   {
        throw std::logic_error(notImplemented);
   };

   PVScalar *PVDataCreate::createPVScalar(PVStructure *parent,
           StringConstPtr fieldName,ScalarType scalarType)
   {
        throw std::logic_error(notImplemented);
   };

   PVScalar *PVDataCreate::createPVScalar(PVStructure *parent,
           StringConstPtr fieldName,ScalarConstPtr scalarToClone) const
   {
        throw std::logic_error(notImplemented);
   };

   PVScalarArray *PVDataCreate::createPVScalarArray(PVStructure *parent,
           ScalarArrayConstPtr scalarArray) const
   {
        throw std::logic_error(notImplemented);
   };

   PVScalarArray *PVDataCreate::createPVScalarArray(PVStructure *parent,
           StringConstPtr fieldName,ScalarType elementType)
   {
        throw std::logic_error(notImplemented);
   };

   PVScalarArray *PVDataCreate::createPVScalarArray(PVStructure *parent,
           StringConstPtr fieldName,ScalarArrayConstPtr scalarArrayToClone) const
   {
        throw std::logic_error(notImplemented);
   };

   PVStructureArray *PVDataCreate::createPVStructureArray(PVStructure *parent,
           StructureArrayConstPtr structureArray) const
   {
        throw std::logic_error(notImplemented);
   };

   PVStructure *PVDataCreate::createPVStructure(PVStructure *parent,
           StructureConstPtr structure)
   {
        throw std::logic_error(notImplemented);
   };

   PVStructure *PVDataCreate::createPVStructure(PVStructure *parent,
           StringConstPtr fieldName,FieldConstPtrArray fields)
   {
        throw std::logic_error(notImplemented);
   };

   PVStructure *PVDataCreate::createPVStructure(PVStructure *parent,
           StringConstPtr fieldName,PVStructure *structToClone)
   {
        throw std::logic_error(notImplemented);
   };

   static PVDataCreate* instance = 0;

   class PVDataCreateExt : public PVDataCreate {
   public:
       PVDataCreateExt(): PVDataCreate(){};
   };

    PVDataCreate * getPVDataCreate() {
           if(instance==0) instance = new PVDataCreateExt();
            return instance;
    }

}}
