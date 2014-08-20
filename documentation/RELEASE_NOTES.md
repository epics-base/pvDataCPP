Release release/3.1 IN DEVELOPMENT
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
      cout << pvValue=>dumpValue(cout) << endl
      cout << pvValue->getField()->dump(cout) << endl;


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
