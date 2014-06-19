#include <string>

#include <epicsUnitTest.h>
#include <testMain.h>

#include <pv/pvData.h>

using namespace epics::pvData;
using namespace std;
using namespace std::tr1;


void test_factory()
{
    testDiag("Test test_factory()");

    FieldCreatePtr fieldCreate = getFieldCreate();

    FieldBuilderPtr fb = fieldCreate->createFieldBuilder();
    testOk1(fb.get() != 0);

    FieldBuilderPtr fb2 = fieldCreate->createFieldBuilder();
	testOk1(fb.get() != fb2.get());
}

void test_structure()
{
    testDiag("Test test_structure()");

    FieldCreatePtr fieldCreate = getFieldCreate();
    FieldBuilderPtr fb = fieldCreate->createFieldBuilder();
    
    // test with simple (non-nested) structure
    string ID = "testStructureID";
    StructureConstPtr s = fb->setId(ID)->
                         add("double", pvDouble)->
                         addArray("intArray", pvInt)->
                         createStructure();
    testOk1(s.get() != 0);
    testOk1(ID == s->getID());
    testOk1(2 == s->getFields().size());
    
    FieldConstPtr f0 = s->getField(0);
    testOk1(scalar == f0->getType());
    testOk1("double" == s->getFieldName(0));
    testOk(pvDouble == static_pointer_cast<const Scalar>(f0)->getScalarType(), "f0 scalar type == double");

    FieldConstPtr f1 = s->getField(1);
    testOk1(scalarArray == f1->getType());
    testOk1("intArray" == s->getFieldName(1));
    testOk(pvInt == static_pointer_cast<const ScalarArray>(f1)->getElementType(), "f1 element type == int");
    
    // test reuse with empty structure
    StructureConstPtr emptyStructure = fb->createStructure();
    testOk1(emptyStructure.get() != 0);
    testOk1(Structure::DEFAULT_ID == emptyStructure->getID());
    testOk1(0 == emptyStructure->getFields().size());
    
    // test add/addArray with Field
    StructureConstPtr s2 = fb->add("s", s)->
                            addArray("sArray", s)->
                            createStructure();
    testOk1(s2 != 0);
    testOk1(Structure::DEFAULT_ID == s2->getID());
    testOk1(2 == s2->getFields().size());

    f0 = s2->getField(0);
    testOk1(structure == f0->getType());
    testOk1("s" == s2->getFieldName(0));
    testOk1(s.get() == f0.get());

    f1 = s2->getField(1);
    testOk1(structureArray == f1->getType());
    testOk1("sArray" == s2->getFieldName(1));
    testOk(s.get() == static_pointer_cast<const StructureArray>(f1)->getStructure().get(), "array element is given structure");
}


void test_invalid()
{
    testDiag("Test test_invalid()");

    FieldCreatePtr fieldCreate = getFieldCreate();

    try
    {
        fieldCreate->createFieldBuilder()->
            add("f1", pvByte)->
            endNested();
        testFail("endNested() allowed in non-nested FieldBuilder");
    }
    catch (std::runtime_error& re) {
        // ok
        testPass("endNested() disallowed in non-nested FieldBuilder");
    }

    try
    {
        fieldCreate->createFieldBuilder()->
            add("f1", pvByte)->
            addNestedStructure("nested")->
                add("n1", pvUInt)->
                createStructure();
        testFail("createStructure() allowed in nested FieldBuilder");
    }
    catch (std::runtime_error& re) {
        // ok
        testPass("createStructure() disallowed in nested FieldBuilder");
    }
}


void test_nestedStructure()
{
    testDiag("Test test_nestedStructure()");

    FieldCreatePtr fieldCreate = getFieldCreate();
    
    string NESTED_ID = "nestedID";
    StructureConstPtr s = fieldCreate->createFieldBuilder()->
                            add("double", pvDouble)->
                            addNestedStructure("nested")->
                                setId(NESTED_ID)->
                                add("short", pvShort)->
                                add("long", pvLong)->
                                endNested()->
                            addArray("intArray", pvInt)->
                            createStructure();
    testOk1(s.get() != 0);
    testOk1(Structure::DEFAULT_ID == s->getID());
    testOk1(3 == s->getFields().size());

    FieldConstPtr f0 = s->getField(0);
    testOk1(scalar == f0->getType());
    testOk1("double" == s->getFieldName(0));
    testOk(pvDouble == static_pointer_cast<const Scalar>(f0)->getScalarType(), "f0 scalar type == double");

    FieldConstPtr f1 = s->getField(1);
    testOk1(structure == f1->getType());
    testOk1("nested" == s->getFieldName(1));

    {
        StructureConstPtr s2 = static_pointer_cast<const Structure>(f1);
        
        testOk1(s2.get() != 0);
        testOk1(NESTED_ID == s2->getID());
        testOk1(2 == s2->getFields().size());
        
        FieldConstPtr f20 = s2->getField(0);
        testOk1(scalar == f20->getType());
        testOk1("short" == s2->getFieldName(0));
        testOk(pvShort == static_pointer_cast<const Scalar>(f20)->getScalarType(), "f20 scalar type == short");
    
        FieldConstPtr f21 = s2->getField(1);
        testOk1(scalar == f21->getType());
        testOk1("long" == s2->getFieldName(1));
        testOk(pvLong == static_pointer_cast<const Scalar>(f21)->getScalarType(), "f21 element type == long");
        
    }

    FieldConstPtr f2 = s->getField(2);
    testOk1(scalarArray == f2->getType());
    testOk1("intArray" == s->getFieldName(2));
    testOk(pvInt == static_pointer_cast<const ScalarArray>(f2)->getElementType(), "f2 element type == int");

}	
	

void test_nestedStructureArray()
{
    testDiag("Test test_nestedStructureArray()");

    FieldCreatePtr fieldCreate = getFieldCreate();
    
    string NESTED_ID = "nestedID";
    StructureConstPtr s = fieldCreate->createFieldBuilder()->
                            add("double", pvDouble)->
                            addNestedStructureArray("nested")->
                                setId(NESTED_ID)->
                                add("short", pvShort)->
                                add("long", pvLong)->
                                endNested()->
                            addArray("intArray", pvInt)->
                            createStructure();
    testOk1(s.get() != 0);
    testOk1(Structure::DEFAULT_ID == s->getID());
    testOk1(3 == s->getFields().size());

    FieldConstPtr f0 = s->getField(0);
    testOk1(scalar == f0->getType());
    testOk1("double" == s->getFieldName(0));
    testOk(pvDouble == static_pointer_cast<const Scalar>(f0)->getScalarType(), "f0 scalar type == double");

    FieldConstPtr f1 = s->getField(1);
    testOk1(structureArray == f1->getType());
    testOk1("nested" == s->getFieldName(1));

    {
        StructureConstPtr s2 = static_pointer_cast<const StructureArray>(f1)->getStructure();
        
        testOk1(s2.get() != 0);
        testOk1(NESTED_ID == s2->getID());
        testOk1(2 == s2->getFields().size());
        
        FieldConstPtr f20 = s2->getField(0);
        testOk1(scalar == f20->getType());
        testOk1("short" == s2->getFieldName(0));
        testOk(pvShort == static_pointer_cast<const Scalar>(f20)->getScalarType(), "f20 scalar type == short");
    
        FieldConstPtr f21 = s2->getField(1);
        testOk1(scalar == f21->getType());
        testOk1("long" == s2->getFieldName(1));
        testOk(pvLong == static_pointer_cast<const Scalar>(f21)->getScalarType(), "f21 element type == long");
        
    }

    FieldConstPtr f2 = s->getField(2);
    testOk1(scalarArray == f2->getType());
    testOk1("intArray" == s->getFieldName(2));
    testOk(pvInt == static_pointer_cast<const ScalarArray>(f2)->getElementType(), "f2 element type == int");

}	


MAIN(testFieldBuilder)
{
    testPlan(65);
    testDiag("Tests for FieldBuilder");

    test_factory();
    test_structure();
    test_nestedStructure();
    test_nestedStructureArray();
    
    
    test_invalid();

    return testDone();
}
