/* pvDataMain.cpp */
/* Author:  Marty Kraimer Date:    17MAR2000 */

#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "pvData.h"

using namespace epics::pvData;

int main(int argc,char *argv[])
{
    Type type = scalar;
    ScalarType scalarType = pvDouble;

    bool value = ScalarTypeFunc::isNumeric(scalarType);
    printf("isNumeric %s\n",(value ? "true" : "false"));
    std::string* myString= new std::string("type ");
    TypeFunc::toString(myString,type);
    *myString += " scalarType ";
    ScalarTypeFunc::toString(myString,scalarType);
    printf("%s\n",myString->c_str());
    FieldCreate * pfieldCreate = getFieldCreate();
    std::string valueName("value");
    ScalarConstPtr pscalar = pfieldCreate->createScalar(&valueName,scalarType);
    type = pscalar->getType();
    myString->clear();
    *myString += "type ";
    TypeFunc::toString(myString,type);
    printf("%s\n",myString->c_str());
    myString->clear();
    *myString += "fieldName ";
    StringConstPtr pfieldName = pscalar->getFieldName();
    *myString += *pfieldName;
    printf("%s\n",myString->c_str());
    myString->clear();
    pscalar->toString(myString);
    printf("%s\n",myString->c_str());
    ScalarArrayConstPtr pscalarArray = pfieldCreate->createScalarArray(&valueName,pvString);
    myString->clear();
    pscalarArray->toString(myString);
    printf("%s\n",myString->c_str());
    int numberFields = 2;
    FieldConstPtr fields[numberFields];
    std::string  name0("high");
    std::string  name1("low");
    fields[0] = pfieldCreate->createScalar(&name0,pvDouble);
    fields[1] = pfieldCreate->createScalar(&name1,pvDouble);
    StructureConstPtr pstructure = pfieldCreate->createStructure(
        &valueName,numberFields,fields);
    myString->clear();
    pstructure->toString(myString);
    printf("%s\n",myString->c_str());
    FieldConstPtr pfield = pstructure;
    myString->clear();
    pfield->toString(myString);
    printf("as Field\n%s/n",myString->c_str());
    return(0);
}
