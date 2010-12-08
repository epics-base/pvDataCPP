#Makefile at top of application tree
TOP = .
include $(TOP)/configure/CONFIG
DIRS += configure
DIRS += pvDataApp
pvDataApp_DEPEND_DIRS = configure
DIRS += testApp
testApp_DEPEND_DIRS = pvDataApp

include $(TOP)/configure/RULES_TOP


