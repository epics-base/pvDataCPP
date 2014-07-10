TODO
===========

printer
------------

pv/printer.h is not used.

doxygen
-------

There is a lot of public code that does not have doxygen tags.


postMonitor: PVUnion, PVUnionArray, and PVStructureArray
--------

PVUnion, PVUnionArray, and PVStructureArray all have elements
that are treated like a top level field.
The implementation should be changed so that it implements PostHandler.
Thus when an element is modified it will call postPut for itself.


monitorPlugin
-------------

A debate is on-going about what semantics should be.

PVFieldConstPtr etc
-------------------

In addition to PVFieldPtr should PVFieldConstPtr exist.
But this means that all methods defined in pvDataCPP must be checked.
This is a BIG job.

