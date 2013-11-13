TOP := $(dir $(lastword $(MAKEFILE_LIST)))
include $(TOP)/framework/mk/fog.mk

OPENG_LIB_DIR=$(TOP)/lib

INCLUDE += -I$(TOP) 
LIBS_PATH := -L$(OPENG_LIB_DIR)
DS_LIBS  :=  -ldatasource
LIBS     :=  $(LIBS_PATH) $(DS_LIBS) $(LIBS)

