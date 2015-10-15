pvaDataCPP
==========

pvDataCPP is a set of data types and utilities that form part of the EPICS V4 project.


Further Info
------------

Consult the documents in the documentation directory, in particular

* pvDataCPP.html
* RELEASE_NOTES.md

Also see the [EPICS Version 4 website](http://epics-pvdata.sourceforge.net)

Prerequisites
-------------

The pvDataCPP  requires recent versions of the following software:

1. EPICS Base (v3.14.12.3 or later)
2. EPICS4 pvCommonCPP (4.1.0 or later)

(pvCommonCPP may not be needed depending on host/compiler.)


Building
--------

Building uses the make utility and the EPICS base build system.

The build system needs the location of the prerequisites, e.g. by placing the
lines of the form

    PVCOMMON = /home/install/epicsV4/pvCommonCPP
    EPICS_BASE = /home/install/epics/base

pointing to the locations in a file called RELEASE.local
in the configure directory or the parent directory of pvDataCPP.

With this in place, to build type make

    make

To perform a clean build type

    make clean uninstall

To run the unit tests type

    make runtests

For more information on the EPICS build system consult the
[Application Development guide](http://www.aps.anl.gov/epics/base/R3-14/12-docs/AppDevGuide.pdf).


