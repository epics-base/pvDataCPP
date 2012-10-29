/* pvEnumerated.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#include <string>
#include <stdexcept>
#include <pv/pvType.h>
#include <pv/pvIntrospect.h>
#include <pv/pvData.h>
#include <pv/pvEnumerated.h>
namespace epics { namespace pvData { 

using std::tr1::static_pointer_cast;

String PVEnumerated::notFound("No enumerated structure found");
String PVEnumerated::notAttached("Not attached to an enumerated structure");

bool PVEnumerated::attach(PVFieldPtr const & pvField)
{
    if(pvField->getField()->getType()!=structure) {
            pvField->message(notFound,errorMessage);
            return false;
    }
    PVStructurePtr pvStructure = static_pointer_cast<PVStructure>(pvField);
    pvIndex = pvStructure->getIntField("index");
    if(pvIndex.get()==NULL) {
        pvField->message(notFound,errorMessage);
        return false;
    }
    PVScalarArrayPtr pvScalarArray = pvStructure->getScalarArrayField(
        "choices",pvString);
    if(pvScalarArray.get()==NULL) {
        pvIndex.reset();
        pvField->message(notFound,errorMessage);
        return false;
    }
    pvChoices = static_pointer_cast<PVStringArray>(pvScalarArray);
    return true;
}

void PVEnumerated::detach()
{
    pvIndex.reset();
    pvChoices.reset();
}

bool PVEnumerated::isAttached() {
    if(pvIndex.get()==NULL) return false;
    return true;
}

bool PVEnumerated::setIndex(int32 index)
{
    if(pvIndex.get()==NULL ) {
         throw std::logic_error(notAttached);
    }
    if(pvIndex->isImmutable()) return false;
    pvIndex->put(index);
    return true;
}

int32 PVEnumerated::getIndex()
{
    if(pvIndex.get()==NULL ) {
         throw std::logic_error(notAttached);
    }
    return pvIndex->get();
}

String PVEnumerated::getChoice()
{
    if(pvIndex.get()==NULL ) {
         throw std::logic_error(notAttached);
    }
    int index = pvIndex->get();
    StringArrayData data;
    pvChoices->get(0,pvChoices->getLength(),data);
    return data.data[index];
}

bool PVEnumerated::choicesMutable()
{
    if(pvIndex.get()==NULL ) {
         throw std::logic_error(notAttached);
    }
    return pvChoices->isImmutable();
}

StringArrayPtr const & PVEnumerated:: getChoices()
{
    if(pvIndex.get()==NULL ) {
         throw std::logic_error(notAttached);
    }
    StringArrayData data;
    return pvChoices->getSharedVector();
}

int32 PVEnumerated::getNumberChoices()
{
    if(pvIndex.get()==NULL ) {
         throw std::logic_error(notAttached);
    }
    return pvChoices->getLength();
}

bool PVEnumerated:: setChoices(StringArray & choices)
{
    if(pvIndex.get()==NULL ) {
         throw std::logic_error(notAttached);
    }
    if(pvChoices->isImmutable()) return false;
    pvChoices->put(0,choices.size(),get(choices),0);
    return true;
}


}}
