TOP=../..

include $(TOP)/configure/CONFIG

LIBRARY_HOST += testc
testc_SRCS += testc.cpp
testc_LIBS += pvData Com

LIBRARY_HOST += pvDataPy
INC += pvDataPy.h
INC += pvDataPyCAPI.h
INC += pvDataPyCreateTest.h
pvDataPy_SRCS += pvDataPy.cpp
pvDataPy_SRCS += pvDataPyCAPI.cpp
pvDataPy_SRCS += pvDataPyCreateTest.cpp
pvDataPy_LIBS += pvData Com

PROD_HOST += testPVDataPy
testPVDataPy_SRCS += testPVDataPy.cpp
testPVDataPy_LIBS += pvDataPy pvData Com

PROD_HOST += testPVDataPyCAPI
testPVDataPyCAPI_SRCS += testPVDataPyCAPI.cpp
testPVDataPyCAPI_LIBS += pvDataPy pvData Com

include $(TOP)/configure/RULES
