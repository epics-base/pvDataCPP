/* testIntrospectScalar.cpp */
/* Author:  Marty Kraimer Date: 2010.10 */

#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "pvIntrospect.h"
#include "standardField.h"

using namespace epics::pvData;

int main(int argc,char *argv[])
{
    StandardField *standardField = getStandardField();
    StructureConstPtr doubleValue = standardField->scalarValue(
        pvDouble,String("alarm,timeStamp,display,control,doubleAlarm"));
    String buffer("");
    doubleValue->toString(&buffer);
    printf("doubleValue\n,%s\n",buffer.c_str());
    return(0);
}
