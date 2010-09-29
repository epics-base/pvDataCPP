TOP=../..

include $(TOP)/configure/CONFIG

INC += factory.h
INC += AbstractPVField.h
INC += AbstractPVScalar.h
INC += AbstractPVArray.h
INC += AbstractPVScalarArray.h
INC += BasePVStructure.h
INC += BasePVDouble.h
INC += BasePVDoubleArray.h
INC += BasePVStructure.h
INC += BasePVStructureArray.h
LIBSRCS += TypeFunc.cpp
LIBSRCS += PVAuxInfoImpl.cpp
LIBSRCS += FieldCreateFactory.cpp
LIBSRCS += PVDataCreateFactory.cpp
LIBSRCS += Convert.cpp
LIBSRCS += StandardField.cpp

LIBRARY=pvFactory

pvFactory_LIBS += Com

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

