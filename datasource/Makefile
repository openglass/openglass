include ../openg.mk

USER_LIBS= -lboost_system -lboost_thread -lboost_signals
SRCS	=  	Connection.cpp \
			Protocol.cpp \
			Message.cpp \
			Tag.cpp \
			TestCaseMapper.cpp \
			DataSourceMgr.cpp

DS_LIB=libdatasource.a

$(OPENG_LIB_DIR)/$(DS_LIB): $(OBJS)
	mkdir -p $(OPENG_LIB_DIR)
	cd $(OPENG_LIB_DIR)
	ar $(ARFLAGS) $@ $(OBJS)

clean: 
		$(RM) $(OBJS) $(OPENG_LIB_DIR)/$(DS_LIB)
