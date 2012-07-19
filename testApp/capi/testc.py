#!/usr/bin/env python

import os
from ctypes import *

libname = "../../lib/%s/libtestc.so" % os.environ["EPICS_HOST_ARCH"]
lib = CDLL(libname)
lib.createTest.restype = c_void_p
lib.createTest.argtypes = []
lib.getNumberFields.restype = c_int
lib.getNumberFields.argtypes = [c_void_p]
lib.getFieldName.restype = c_char_p
lib.getFieldName.argtypes = [c_void_p, c_int]
lib.getFieldType.restype = c_int
lib.getFieldType.argtypes = [c_void_p, c_char_p]
lib.getScalarType.restype = c_int
lib.getScalarType.argtypes = [c_void_p, c_char_p]
lib.getElementType.restype = c_int
lib.getElementType.argtypes = [c_void_p, c_char_p]

type_enum = ["scalar", "scalarArray", "structure", "structureArray"]
struct = lib.createTest()

l = lib.getNumberFields(struct)
print "pvData structure has %d fields" % l

for n in range(l):
    name = lib.getFieldName(struct, n)
    type_ = lib.getFieldType(struct, name)
    sc_type = lib.getScalarType(struct, name)
    el_type = lib.getElementType(struct, name)
    print name, type_enum[type_], sc_type, el_type
    
