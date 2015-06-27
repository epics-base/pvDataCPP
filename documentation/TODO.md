TODO
===========

doxygen
-------

There is a lot of public code that does not have doxygen tags.


postMonitor: PVUnion, PVUnionArray, and PVStructureArray
--------

PVUnion, PVUnionArray, and PVStructureArray all have elements
that are treated like a top level field.

Currently if a subField of any of these is changed postMonitor is not called for the field itself.

David asked if this could be changed so that it is called.
Marty thinks this may not be a good idea.


valueAlarm
---------

normativeTypes.html describes valueAlarm only for a value field that has type
double.
The implementation also supports all the numeric scalar types.

monitorPlugin
-------------

A debate is on-going about what semantics should be.

