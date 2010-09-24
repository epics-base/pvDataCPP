/* pvDataMain.cpp */
/* Author:  Marty Kraimer Date:    17MAR2000 */

#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "pvIntrospect.h"

using namespace epics::pvData;
using namespace std;

static void dumpString(char const prefix[],string const* value) {
    printf("%s\n value %p",prefix,value);
    // Just convert value to a pointer
    int *pvalue = (int *)value;
    // Now get what pvalue points to. It is the cstr.
    char * cstr = (char *)*pvalue;
    if(cstr==0) { printf("\n"); return; }
    printf(" *value %p",cstr);
    int * stuff = (int *)*pvalue;
    //At least on my implementation the following is true
    stuff -= 3;
    printf(" length %d capacity %d refcount %d str %s\n",
       stuff[0],stuff[1],stuff[2],cstr);
}

int main(int argc,char *argv[])
{
    FieldCreate * pfieldCreate = getFieldCreate();
    std::string valueName("value");
    std::string* myString= new std::string("type ");
    int numberFields = 2;
    FieldConstPtr fields[numberFields];
    std::string  name0("high");
    std::string  name1("low");
    dumpString("high",&name0);
    dumpString("low",&name1);
    printf("\ncreate field0 and field1\n");
    fields[0] = pfieldCreate->createScalar(&name0,pvDouble);
    fields[1] = pfieldCreate->createScalar(&name1,pvDouble);
    printf("referenceCounts field0 %d field1 %d\n",
        fields[0]->getReferenceCount(),
        fields[1]->getReferenceCount());
    dumpString("high",&name0);
    dumpString("low",&name1);
    printf("\ncreate scalarArray");
    std::string scalarArrayName("scalarArray");
    ScalarArrayConstPtr pscalarArray = pfieldCreate->createScalarArray(
        &scalarArrayName,pvDouble);
    myString->clear();
    pscalarArray->toString(myString);
    printf("%s\n",myString->c_str());
    printf("referenceCount pscalarArray %d\n", pscalarArray->getReferenceCount());
    dumpString("scalarArray",&scalarArrayName);
    pscalarArray->incReferenceCount();
    printf("after incReferenceCounnt referenceCount pscalarArray %d\n",
        pscalarArray->getReferenceCount());
    dumpString("high",&name0);
    pscalarArray->decReferenceCount();
    dumpString("after decReferenceCount scalarArray",&scalarArrayName);

    printf("\ncreate structure\n");
    StructureConstPtr pstructure = pfieldCreate->createStructure(
        &valueName,numberFields,fields);
    myString->clear();
    pstructure->toString(myString);
    printf("%s\n",myString->c_str());
    FieldConstPtr pfield = pstructure;
    myString->clear();
    pfield->toString(myString);
    printf("as Field\n%s\n",myString->c_str());
    printf("referenceCounts pfield %d field0 %d field1 %d\n",
        pfield->getReferenceCount(),
        fields[0]->getReferenceCount(),
        fields[1]->getReferenceCount());
    dumpString("high",&name0);
    dumpString("low",&name1);
    pfield->incReferenceCount();
    pfield->incReferenceCount();
    printf("after incReferenceCounter twice referenceCounts pfield %d field0 %d field1 %d\n",
        pfield->getReferenceCount(),
        fields[0]->getReferenceCount(),
        fields[1]->getReferenceCount());
    dumpString("high",&name0);
    dumpString("low",&name1);
    pfield->decReferenceCount();
    printf("after decReferenceCount referenceCounts pfield %d field0 %d field1 %d\n",
        pfield->getReferenceCount(),
        fields[0]->getReferenceCount(),
        fields[1]->getReferenceCount());
    dumpString("high",&name0);
    dumpString("low",&name1);
    StructureArrayConstPtr pstructureArray = pfieldCreate->createStructureArray(
        &valueName,pstructure);
    pstructureArray->incReferenceCount();
    printf("after createStructureArray referenceCounts pstructureArray %d pfield %d field0 %d field1 %d\n",
        pstructureArray->getReferenceCount(),
        pfield->getReferenceCount(),
        fields[0]->getReferenceCount(),
        fields[1]->getReferenceCount());
    pstructureArray->decReferenceCount();
    printf("after structureArray decReferenceCount referenceCounts pfield %d field0 %d field1 %d\n",
        pfield->getReferenceCount(),
        fields[0]->getReferenceCount(),
        fields[1]->getReferenceCount());
    dumpString("high",&name0);
    dumpString("low",&name1);
    printf("field recReferenceCount\n");
    pfield->decReferenceCount();
    dumpString("high",&name0);
    dumpString("low",&name1);
    return(0);
}
