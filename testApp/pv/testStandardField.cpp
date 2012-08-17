/* testStandardField.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/* Author:  Marty Kraimer Date: 2012.08 */

#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>

#include <epicsAssert.h>
#include <epicsExit.h>

#include <pv/requester.h>
#include <pv/pvIntrospect.h>
#include <pv/pvData.h>
#include <pv/convert.h>
#include <pv/standardField.h>

using namespace epics::pvData;
using std::tr1::static_pointer_cast;

static FieldCreatePtr fieldCreate = getFieldCreate();
static StandardFieldPtr standardField = getStandardField();
static String builder("");

static void print(String name)
{
    printf("\n%s\n%s\n",name.c_str(),builder.c_str());
}

int main(int argc,char *argv[])
{
    StructureConstPtr doubleValue = standardField->scalar(pvDouble,
        "alarm,timeStamp,display,control,valueAlarm");
    builder.clear();
    doubleValue->toString(&builder);
    print("doubleValue");
    StructureConstPtr stringArrayValue = standardField->scalarArray(pvString,
        "alarm,timeStamp");
    builder.clear();
    stringArrayValue->toString(&builder);
    print("stringArrayValue");
    StructureConstPtr enumeratedValue = standardField->enumerated(
        "alarm,timeStamp,valueAlarm");
    builder.clear();
    enumeratedValue->toString(&builder);
    print("enumeratedValue");
    FieldConstPtrArray fields;
    fields.reserve(2);
    fields.push_back(fieldCreate->createScalar(pvDouble));
    fields.push_back(fieldCreate->createScalarArray(pvDouble));
    StringArray fieldNames;
    fieldNames.reserve(2);
    fieldNames.push_back("scalarValue");
    fieldNames.push_back("arrayValue");
    StructureConstPtr structure = fieldCreate->createStructure(fieldNames, fields);
    StructureConstPtr structureArrayValue = standardField->structureArray(structure, "alarm,timeStamp");
    builder.clear();
    structureArrayValue->toString(&builder);
    print("structureArrayValue");
    return(0);
}

