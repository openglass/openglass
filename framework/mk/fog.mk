MAKE_ROOT=/appl/framework/mk
FW_ROOT=/appl

include $(MAKE_ROOT)/linux64.mk
include $(MAKE_ROOT)/third_party.mk
RM			= /bin/rm -f

OBJS			= $(SRCS:.cpp=.o)
CREATE_LIB		= ar $(ARFLAGS) $@ $(OBJS)

.SUFFIXES: .cpp .o

INCLUDE := -I/appl/framework/
INCLUDE := $(INCLUDE)
INCLUDE := $(INCLUDE) $(THIRD_PARTY_INCLUDE) $(USER_INCLUDE)

COMM_LIBS  :=  -L/appl/framework/common -lcommon
NET_LIBS   :=  -L/appl/framework/network -lnetwork
LIBS       =  $(NET_LIBS) $(COMM_LIBS) $(SSTM_LIBS) $(THIRD_PARTY_LIBS) $(USER_LIBS)

.cpp.o:
	$(CC) -c $(CXXFLAGS) $(INCLUDE) $< -o $@

%.a: 
	ar $(ARFLAGS) $@ $(OBJS)
