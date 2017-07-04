#ifndef MAINPAGE_H
#define MAINPAGE_H
/**
@mainpage pvDataCPP documentation

- [Download](https://sourceforge.net/projects/epics-pvdata/files/)
- @ref release_notes

The epics::pvData namespace.
See pv/pvData.h header.

@code
#include <pv/pvData.h>
#include <pv/createRequest.h>
@endcode

- Type description epics::pvData::Field and sub-classes
- Value container epics::pvData::PVField and sub-classes
- POD array handling epics::pvData::shared_vector
- pvRequest parsing epics::pvData::createRequest()

Define a structure type and create a container with default values.

@code
epics::pvData::StructureConstPtr stype; // aka std::tr1::shared_ptr<const epics::pvData::Structure>
stype = epics::pvData::getFieldCreate()->createFieldBuilder()
                        ->add("fld1", epics::pvData::pvInt)
                        ->addNestedStructure("sub")
                            ->add("fld2", epics::pvData::pvString)
                        ->endNested()
                        ->createStructure();

epics::pvData::PVStructuretPtr value; // aka std::tr1::shared_ptr<epics::pvData::PVStructure>
value = epics::pvData::getPVDataCreate()->createPVStructure(stype);

value->getSubField<epics::pvData::PVInt>("fld1")->put(4); // store integer 4
value->getSubField<epics::pvData::PVScalar>("sub.fld2")->putFrom(4.2); // convert and store string "4.2"
@endcode

is equivalent to the following pseudo-code.

@code
struct stype {
    epics::pvData::int32 fld1;
    struct {
        std::string fld2;
    } sub;
};
stype value;
value.fld1 = 4;
value.fld2 = epics::pvData::castUnsafe<std::string>(4.2);
@endcode

*/

#endif /* MAINPAGE_H */

