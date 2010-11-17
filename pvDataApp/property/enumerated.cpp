/* enumerated.cpp */
#include <string>
#include <stdexcept>
#include "pvType.h"
#include "pvIntrospect.h"
#include "pvData.h"
#include "enumerated.h"
#include "noDefaultMethods.h"
namespace epics { namespace pvData { 


static String notStructure("field is not a structure");
static String notEqual2("structure does not have exactly two fields");
static String badIndex("structure does not have field index of type int");
static String badChoices("structure does not have field choices of type stringArray");

Enumerated* Enumerated::create(PVField *pvField){
    if(pvField->getField()->getType()!=structure) {
        pvField->message(notStructure,errorMessage);
        return 0;
    }
    PVStructure *pvStructure = (PVStructure*)pvField;
    int numberFields = pvStructure->getNumberFields();
    if(numberFields!=2) {
        pvField->message(notEqual2,errorMessage);
        return 0;
    }
    PVFieldPtrArray pvFields = pvStructure->getPVFields();
    FieldConstPtrArray fields = pvStructure->getStructure()->getFields();
    FieldConstPtr field = fields[0];
    if(field->getFieldName().compare("index")!=0 || field->getType()!=scalar) {
        pvField->message(badIndex,errorMessage);
        return 0;
    }
    PVInt *pvInt = (PVInt *) pvFields[0];
    field = fields[1];
    if(field->getFieldName().compare("choices")!=0 || field->getType()!=scalarArray) {
	pvField->message(badChoices,errorMessage);
	return 0;
    }
    ScalarArrayConstPtr pscalarArray = (ScalarArrayConstPtr)field;
    if(pscalarArray->getElementType()!=pvString) {
	pvField->message(badChoices,errorMessage);
	return 0;
    }
    PVStringArray *pvStringArray = (PVStringArray *) pvFields[1];
    return new Enumerated(pvStructure,pvInt,pvStringArray);
}

Enumerated::~Enumerated() {}

Enumerated::Enumerated(
    PVStructure *pvStructure,PVInt *pvIndex,PVStringArray *pvChoices)
: pvStructure(pvStructure),pvIndex(pvIndex),pvChoices(pvChoices),
  stringArrayData(StringArrayData()) {}
    
String Enumerated::getChoice() {
    pvChoices->get(0, pvChoices->getLength(), &stringArrayData);
    return stringArrayData.data[pvIndex->get()];
}

}}
