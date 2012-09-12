#!/usr/bin/env python

"pvData Python ctypes test with numpy array sharing"

import os
from ctypes import *
from numpy import *

# enums

type_enum = ["scalar", "scalarArray", "structure", "structureArray"]

scalar_enum = ["boolean", "byte", "short", "int",
               "long", "byte", "ushort", "uint",
               "ulong", "float", "double", "string"]

pvBoolean, pvByte, pvShort, pvInt, \
pvLong, pvUByte, pvUShort, pvUInt, \
pvULong, pvFloat, pvDouble, pvString = range(12)

# ctypes declarations (incomplete)

libname = "../../lib/%s/libpvDataCAPI.so" % os.environ["EPICS_HOST_ARCH"]
lib = CDLL(libname)
lib.pvCAPICreateScalar.argtypes = [c_int, c_char_p]
lib.pvCAPICreateScalar.restype = c_void_p

lib.pvCAPIGetSubfield.argtypes = [c_void_p, c_char_p]
lib.pvCAPIGetSubfield.restype = c_int

lib.pvCAPIGetFieldName.restype = c_char_p

lib.pvCAPIGetPVScalarCAPI.argtypes = [c_void_p, c_int]
lib.pvCAPIGetPVScalarCAPI.restype = c_void_p

lib.pvCAPIScalarPutDouble.argtypes = [c_void_p, c_double]
lib.pvCAPIScalarPutDouble.restype = None

lib.pvCAPIFieldCreateStructure.argtypes = [POINTER(c_char_p),
                                           POINTER(c_void_p),
                                           c_int]

lib.pvCAPIFieldCreateStructure.restype = c_void_p

# test creating an arbitrary structure from Python

def createStructure():
    one = lib.pvCAPIFieldCreateScalarArray(pvDouble)
    two = lib.pvCAPIFieldCreateScalar(pvString)
    three = lib.pvCAPIFieldCreateScalar(pvInt)
    four = lib.pvCAPIFieldCreateScalar(pvString)

    N = 2
    fields = (c_void_p * N)()
    fields[0] = three
    fields[1] = four

    names = (c_char_p * N)()
    names[0] = "three"
    names[1] = "four"

    middle = lib.pvCAPIFieldCreateStructure(names, fields, N)
    middleArray = lib.pvCAPIFieldCreateStructureArray(middle)

    N = 3
    fields = (c_void_p * N)()
    fields[0] = three
    fields[1] = four
    fields[2] = middleArray

    names = (c_char_p * N)()
    names[0] = "one"
    names[1] = "two"
    names[2] = "middle"

    top = lib.pvCAPIFieldCreateStructure(names, fields, N)
    lib.pvCAPIDumpStructure(top)

createStructure()

# test creating a scalar and filling the value field

addrTop = lib.pvCAPICreateScalar(pvFloat, "alarm,timeStamp")
lib.pvCAPIDumpPVStructureCAPI(addrTop)
ijunk = lib.pvCAPIGetSubfield(addrTop,"junk")
print "index of field 'junk' %d" % ijunk
ind = lib.pvCAPIGetSubfield(addrTop,"value")
type_ = lib.pvCAPIGetFieldType(addrTop,ind)
fieldName = lib.pvCAPIGetFieldName(addrTop,ind)
print "value ind %d type %s fieldName %s" % (ind,type_enum[type_],fieldName)
addrValue = lib.pvCAPIGetPVScalarCAPI(addrTop,ind)
value = 1.234
lib.pvCAPIScalarPutDouble(addrValue, value)
lib.pvCAPIDumpPVStructureCAPI(addrTop)
lib.pvCAPIDestroyPVStructureCAPI(addrTop)
