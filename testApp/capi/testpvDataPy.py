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
