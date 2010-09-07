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
    TypeFunc::toString(*myString,type);
    *myString += " scalarType ";
    ScalarTypeFunc::toString(*myString,scalarType);
    printf("%s\n",myString->c_str());
    FieldCreate &fieldCreate = getFieldCreate();
    std::string valueName("value");
    Scalar const & scalar = fieldCreate.createScalar(valueName,scalarType);
    type = scalar.getType();
    myString->clear();
    *myString += "type ";
    TypeFunc::toString(*myString,type);
    printf("%s\n",myString->c_str());
    myString->clear();
    *myString += "fieldName ";
    std::string const & fieldName = scalar.getFieldName();
    *myString += fieldName;
    printf("%s\n",myString->c_str());
    myString->clear();
    scalar.toString(*myString);
    printf("%s\n",myString->c_str());
    ScalarArray const & scalarArray = fieldCreate.createScalarArray(valueName,pvString);
    myString->clear();
    scalarArray.toString(*myString);
    printf("%s\n",myString->c_str());
    int numberFields = 2;
    FieldPtrConst fields[numberFields];
    std::string  name0("high");
    std::string  name1("low");
    fields[0] = &fieldCreate.createScalar(name0,pvDouble);
    fields[1] = &fieldCreate.createScalar(name1,pvDouble);
    Structure const & structure = fieldCreate.createStructure(valueName,numberFields,fields);
    myString->clear();
    structure.toString(*myString);
    printf("%s\n",myString->c_str());
    Field const & field = structure;
    myString->clear();
    field.toString(*myString);
    printf("as Field\n%s/n",myString->c_str());
    return(0);
}
