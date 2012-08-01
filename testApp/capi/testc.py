#!/usr/bin/env python

"pvData Python ctypes test with numpy array sharing"

import os
from ctypes import *
from numpy import *

# ctypes API

libname = "../../lib/%s/libtestc.so" % os.environ["EPICS_HOST_ARCH"]
lib = CDLL(libname)
lib.createTest.restype = c_void_p
lib.createTest.argtypes = []
lib.getDoubleArray.argtypes = [c_void_p, c_char_p]
lib.getDoubleArray.restype = POINTER(c_double)
lib.getLength.argtypes = [c_void_p, c_char_p]
lib.getLength.restype = c_int
lib.setLength.argtypes = [c_void_p, c_char_p, c_int]
lib.setLength.restype = None
lib.getInt.argtypes = [c_void_p, c_char_p]
lib.getInt.restype = c_int
lib.putField_int.argtypes = [c_void_p, c_char_p, c_int]
lib.putField_int.restype = None
lib.putField_double.argtypes = [c_void_p, c_char_p, c_double]
lib.putField_double.restype = None
lib.PVStructuretoString.restype = None
lib.PVStructuretoString.argtypes = [c_void_p]
lib.createStructureVariant.restype = c_void_p
lib.createStructureVariant.argtypes = [c_void_p, c_int]
lib.createPVStructure.restype = c_void_p
lib.createPVStructure.argtypes = [c_void_p]
lib.getNumberFields.restype = c_int
lib.getNumberFields.argtypes = [c_void_p]
lib.getFieldName.restype = c_char_p
lib.getFieldName.argtypes = [c_void_p, c_int]
lib.getFieldType.restype = c_int
lib.getFieldType.argtypes = [c_void_p, c_int]
lib.getScalarType.restype = c_int
lib.getScalarType.argtypes = [c_void_p, c_int]
lib.getElementType.restype = c_int
lib.getElementType.argtypes = [c_void_p, c_int]

class FieldVariant(Structure):
    _fields_ = (("name", c_char_p),
                ("type", c_int),
                ("scalarType", c_int),
                ("structure", c_void_p))

# Python API

scalar, scalarArray, structure, structureArray = range(4)
type_enum = ["scalar", "scalarArray", "structure", "structureArray"]
scalar_enum = ["pvBoolean", "pvByte", "pvShort", "pvInt",
               "pvLong", "pvUByte", "pvUShort", "pvUInt",
               "pvULong", "pvFloat", "pvDouble", "pvString"]

pvBoolean, pvByte, pvShort, pvInt, \
           pvLong, pvUByte, pvUShort, pvUInt, \
           pvULong, pvFloat, pvDouble, pvString = range(12)

class Structure4(object):
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
        print "destroying Structure handle"
        lib.destroy(self.handle)
    def init(self):
        s = self
        self.fields = []
        # cache introspection in python
        l = s.getNumberFields()
        for n in range(l):
            name = s.getFieldName(n)
            type_ = s.getFieldType(n)
            if type_enum[type_] == "scalar":
                subtype = s.getScalarType(n)
            elif type_enum[type_] == "scalarArray":
                subtype = s.getElementType(n)
            self.fields.append((name, type_, subtype))
        
    def show(self):
        s = self
        l = s.getNumberFields()
        print "structure"
        for n in range(l):
            name = s.getFieldName(n)
            type_ = s.getFieldType(n)
            if type_enum[type_] == "scalar":
                sc_type = s.getScalarType(n)
                print "  ", scalar_enum[sc_type], name
            elif type_enum[type_] == "scalarArray":
                el_type = s.getElementType(n)
                print "  ", scalar_enum[el_type], "[]", name
            else:
                print "TODO"


class MyStructure(Structure4):
    def __init__(self, fields = None):
        if fields is None:
            fields = self._fields_
        N = len(fields)
        fs = (FieldVariant * N)()
        for n, (name, type_, subType) in enumerate(fields):
            fs[n].name = name
            fs[n].type = type_
            if type_ == structure or type_ == structureArray:
                fs[n].structure = subType().handle
            else:
                fs[n].scalarType = subType
        self.handle = lib.createStructureVariant(fs, N)
        self.init()

class PVStructure(object):
    def __init__(self, s):
        self.handle = lib.createPVStructure(s.handle)
    def show(self):
        lib.PVStructuretoString(self.handle)
    def __enter__(self):
        return self
    def __exit__(self, a, b, c):
        print "destroying PVStructure handle (TODO)"
        pass
    def getInt(self, name):
        return lib.getInt(self.handle, name)
    def putInt(self, name, value):
        return lib.putField_int(self.handle, name, value)
    def putDouble(self, name, value):
        return lib.putField_double(self.handle, name, value)
    def setLength(self, name, length):
        lib.setLength(self.handle, name, length)
    def getDoubleArray(self, name):
        sz = lib.getLength(self.handle, name)
        ptr = lib.getDoubleArray(self.handle, name)
        # use the library routine 'as_array' in numpy 1.6
        ptr.__array_interface__ = {'typestr': '<f8',
                                   'version': 3,
                                   'data': (addressof(ptr.contents), False),
                                   'shape': (sz,)}
        a = array(ptr, copy = False)
        return a
    
# create Structure from Python (ctypes style)

class MyStructure1(MyStructure):
    _fields_ = (("name", scalar, pvDouble),)

class MyStructure2(MyStructure):
    _fields_ = (("hello", scalar, pvDouble),
                ("world", scalar, pvInt),
                ("epics", scalarArray, pvUShort),
                ("diamond", scalarArray, pvDouble),
                ("xyz", structureArray, MyStructure1))

with MyStructure2() as s:
    s.show()
    # create PVStructure from Structure
    with PVStructure(s) as pv:
        pv.setLength("diamond", 10)
        a = pv.getDoubleArray("diamond")
        a[:5] = -0.1
        a[-5:] = [2,4,6,8,10]
        a *= 100
        pv.putInt("world", 99)
        pv.putDouble("hello", 102.5)
        pv.show()
        
