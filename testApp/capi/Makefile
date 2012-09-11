TOP=../..

include $(TOP)/configure/CONFIG

#LIBRARY_HOST += testc
#testc_SRCS += testc.cpp
#testc_LIBS += pvData Com

LIBRARY_HOST += pvDataCAPI
INC += pvDataCAPI.h
INC += pvDataCAPICreateTest.h
pvDataCAPI_SRCS += pvDataCAPI.cpp
pvDataCAPI_SRCS += pvDataCAPICreateTest.cpp
pvDataCAPI_LIBS += pvData Com

PROD_HOST += testPVDataCAPI
testPVDataCAPI_SRCS += testPVDataCAPI.cpp
testPVDataCAPI_LIBS += pvDataCAPI pvData Com

PROD_HOST += testPVDataCAPIViaC
testPVDataCAPIViaC_SRCS += testPVDataCAPIViaC.c
testPVDataCAPIViaC_LIBS += pvDataCAPI pvData Com

-include $(TOP)/configure/RULES
