include /appl/framework/mk/fog.mk
# will include all framework

OPENG_MAKE_ROOT=/appl/openglass
OPENG_LIB_DIR=/appl/openglass/lib

INCLUDE += -I$(OPENG_MAKE_ROOT)
LIBS_PATH := -L$(OPENG_LIB_DIR)
DS_LIBS  :=  -ldatasource
LIBS     :=  $(LIBS_PATH) $(DS_LIBS) $(LIBS)

