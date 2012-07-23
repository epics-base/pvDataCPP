#!/usr/bin/env python

import os
from ctypes import *

# cytpes API

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

# Python API

type_enum = ["scalar", "scalarArray", "structure", "structureArray"]

class Structure:
    def getNumberFields(self):
        return lib.getNumberFields(self.handle)
    def getFieldName(self, n):
        return lib.getFieldName(self.handle, n)
    def getFieldType(self, n):
        return lib.getFieldType(self.handle, n)
    def getScalarType(self, n):
        return lib.getScalarType(self.handle, n)
    def getElementType(self, n):
        return lib.getElementType(self.handle, n)
    def __enter__(self):
        return self
    def __exit__(self, a, b, c):
        print "destroying handle"
        lib.destroy(self.handle)

class TestStructure(Structure):
    def __init__(self):
        self.handle = lib.createTest()

with TestStructure() as s:
    l = s.getNumberFields()
    print "pvData structure has %d fields" % l
    for n in range(l):
        name = s.getFieldName(n)
        type_ = s.getFieldType(name)
        sc_type = s.getScalarType(name)
        el_type = s.getElementType(name)
        print name, type_enum[type_], sc_type, el_type
        
