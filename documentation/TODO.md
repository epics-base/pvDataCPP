TODO
===========

toString, dumpValue, printer, and streams
------------

The way to print introspection and data instances is not consistent.
This needs work.
Some items that need work are:

* Introspection  has no support for streams. Only for toString.
* data has problems. toString is implemented by calling Convert::getString.
It look like this was intended to use printer but that did nor properly indent fields of structures.

The current implementation is:

     void Convert::getString(StringBuilder buf,PVField const *pvField,int /*indentLevel*/)
     {
         // TODO indextLevel ignored
         std::ostringstream strm;
         strm << pvField->dumpValue(strm) << std::endl;
     //    PrinterPlain p;
     //    p.setStream(strm);
     //    p.print(*pvField);
         strm.str().swap(*buf);
     }

Thus it just uses dumpValue.
What should it do?
If printer is used it must do proper indentation.

doxygen
-------

There is a lot of public code that does not have doxygen tags.


monitorPlugin
-------------

A debate is on-going about what semantics should be.

PVFieldConstPtr etc
-------------------

In pvDataCPP.html look at the monoitorPlugin example.
It has a discussion of a possible need for shared pointers that can not be used to modify data.
This in addition to PVFieldPtr should PVFieldConstPtr exist.
But this means that all methods defined in pvDataCPP must be checked.
This is a BIG job.

Release 3.0.2
==========
This was the starting point for RELEASE_NOTES
