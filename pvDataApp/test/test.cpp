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
    scalar.toString(*myString);
    printf("%s\n",myString->c_str());
    return(0);
}
