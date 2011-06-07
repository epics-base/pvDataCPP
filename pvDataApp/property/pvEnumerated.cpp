/* pvEnumerated.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <string>
#include <stdexcept>
#include <pv/pvType.h>
#include <pv/pvIntrospect.h>
#include <pv/pvData.h>
#include <pv/pvEnumerated.h>
namespace epics { namespace pvData { 


static String notStructure("field is not a structure");
static String notEnumerated("field is not an enumerated structure");
static String notAttached("Not attached to an enumerated structure");

bool PVEnumerated::attach(PVField *pvField)
{
    if(pvField->getField()->getType()!=structure) {
        pvField->message(notStructure,errorMessage);
        return false;
    }
    PVStructure *pvStructure = static_cast<PVStructure*>(pvField);
    PVInt *pvInt = pvStructure->getIntField(String("index"));
    if(pvInt==0) {
        pvField->message(notEnumerated,errorMessage);
        return false;
    }
    PVScalarArray *pvScalarArray = pvStructure->getScalarArrayField(
        String("choices"),pvString);
    if(pvScalarArray==0) {
        pvField->message(notEnumerated,errorMessage);
        return false;
    }
    pvIndex = pvInt;
    pvChoices = static_cast<PVStringArray *>(pvScalarArray);
    return true;
}

void PVEnumerated::detach()
{
    pvIndex = 0;
    pvChoices = 0;
}

bool PVEnumerated::isAttached() {
    if(pvIndex==0 || pvChoices==0) return false;
    return true;
}

bool PVEnumerated::setIndex(int32 index)
{
    if(pvIndex==0 || pvChoices==0) {
         throw std::logic_error(notAttached);
    }
    if(pvIndex->isImmutable()) return false;
    pvIndex->put(index);
    return true;
}

int32 PVEnumerated::getIndex()
{
    if(pvIndex==0 || pvChoices==0) {
         throw std::logic_error(notAttached);
    }
    return pvIndex->get();
}

String PVEnumerated::getChoice()
{
    if(pvIndex==0 || pvChoices==0) {
         throw std::logic_error(notAttached);
    }
    int index = pvIndex->get();
    StringArrayData data;
    pvChoices->get(0,pvChoices->getLength(),&data);
    return data.data[index];
}

bool PVEnumerated::choicesMutable()
{
    if(pvIndex==0 || pvChoices==0) {
         throw std::logic_error(notAttached);
    }
    return pvChoices->isImmutable();
}

StringArray PVEnumerated:: getChoices()
{
    if(pvIndex==0 || pvChoices==0) {
         throw std::logic_error(notAttached);
    }
    StringArrayData data;
    pvChoices->get(0,pvChoices->getLength(),&data);
    return data.data;
}

int32 PVEnumerated::getNumberChoices()
{
    if(pvIndex==0 || pvChoices==0) {
         throw std::logic_error(notAttached);
    }
    return pvChoices->getLength();
}

bool PVEnumerated:: setChoices(StringArray choices,int32 numberChoices)
{
    if(pvIndex==0 || pvChoices==0) {
         throw std::logic_error(notAttached);
    }
    if(pvChoices->isImmutable()) return false;
    pvChoices->put(0,numberChoices,choices,0);
    return true;
}


}}
