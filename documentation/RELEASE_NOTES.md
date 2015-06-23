Release 4.1 IN DEVELOPMENT
===========

The main changes since release 4.0 are:

* Convert copy methods and equals operators (re)moved
* Convert::copyUnion now always copies between subfields.
* CreateRequest prevents a possible SEGFAULT.
* New stream operators for Field and PVField are provided.
* New method getAs that is like getSubField except that it throws exception

Convert copy methods and equals operators
-----------------------------------------

Convert copy methods where moved and replaced with methods
on PVField classes, i.e.

    PVField::copy(const PVField& from)

Methods

    PVField::copyUnchecked(const PVField& from)

where added to allow unchecked copies, to gain performance
where checked are not needed (anymore).

In addition:
- isCompatibleXXX methods were removed in favour of Field::operator==.
- equals methods were remove in favour of PVField::operator==.
- operator== methods where moved to pvIntrospect.h and pvData.h

Convert::copyUnion
-----------------

Before this method, depending on types for to and from,
sometimes did a shallow cppy, i. e. just made to shared_ptr for to 
share the same data as from.
Now it always copies between the subfield of to and from.

CreateRequest change
--------------------

createRequest could cause a SEGFAULT if passed a bad argument.
This has been changed so the it returns a null pvStructure
and provies an error.

New stream operators
--------------------

New steam operators are available for Field and PVField.
Before to print a Field (or any extension) or a PVField (or any extension)
it was necessary to have code like:

     void print(StructureConstPtr struct, PVStructurePtr pv)
     {
         if(struct) {
             cout << *struct << endl;
         } else {
             cout << "nullptr\n"
         }
         if(pv) {
             cout << *.struct << endl;
         } else {
             cout << "nullptr\n"
         }
     }

Now it can be done as follows:

     void print(StructureConstPtr struct, PVStructurePtr pv)
     {
         cout << struct << endl;
         cout << pv << endl;
     }

New method getAs that is like getSubField except that it throws exception
--------------------

<b>PVStructure</b> has a new template member <b>getAs(const char *name)</b>
that is like <b>getSubField</b> except that it throws a runtime_error
instead of returning null.

Release 4.0
===========

The main changes since release 3.0.2 are:

* array semantics now enforce Copy On Write.
* String no longer defined.
* timeStamp and valueAlarm name changes
* toString replaced by stream I/O 
* union is new type.
* copy is new.
* monitorPlugin is new.

New Semantics for Arrays
--------

PVScalarArray, PVStructureArray, and PVUnionArray all enforce COW (Copy On Write) Semantics.
In order to limit memory usage the storage for raw data is managed via a new shared_vector facility.
This allows multiple instances of array data to use the shared raw data.
COW is implemented via shared_vectors of const data, i. e. data that can not be modified.


String no longer defined
---------

This is replaced by std::string.


timeStamp and valueAlarm name changes
--------------

In timeStamp nanoSeconds is changed to nanoseconds.

In valueAlarm hystersis is changed to hysteresis


toString replaced by stream I/O
---------

pvData.h and pvIntrospect no longer defines toString
Instead they have stream support.
pvIntrospect uses method dump and pvData uses dumpValue.
For example:

      PVDoublePtr pvValue;
      String buffer;
      pvValue->toString(&buffer);
      cout << buffer << endl;
      buffer.clear();
      pvValue->getField()->toString(&buffer);
      cout << buffer << evdl;

is replaced by

      PVDoublePtr pvValue;
      cout << *pvValue << endl
      cout << *pvValue->getField() << endl;


union is a new basic type.
------------

There are two new basic types: union_t and unionArray.

A union is like a structure that has a single subfield.
There are two flavors:

* <b>varient union</b> The field can have any type.
* <b>union</b> The field can any of specified set of types.

The field type can be dynamically changed.

copy 
----

This consists of createRequest and pvCopy.
createRequest was moved from pvAccess to here.
pvCopy is moved from pvDatabaseCPP and now depends
only on pvData, i. e. it no longer has any knowledge of PVRecord.

monitorPlugin
-------------

This is for is for use by code that implements pvAccess monitors.
This is prototype and is subject to debate.

Release 3.0.2
==========
This was the starting point for RELEASE_NOTES
