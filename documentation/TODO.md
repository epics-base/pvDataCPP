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

Currently if a subField of any of these is changed postMonitor is not called for the field itself.

David asked if this could be changed so that it is called.
Marty thinks this may not be a good idea.

timeStamp, display, control, and valueAlarm
----------

normativeTypes.html defines time_t,  display_t, control_t, and alarmlimit_t.
The definitions are not compatible with how property defined timeStamp, display, control, and valueAlarm.
The definition of alarm_t does match the definition of property alarm.

monitorPlugin
-------------

A debate is on-going about what semantics should be.

