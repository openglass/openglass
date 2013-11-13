include ../openg.mk

USER_LIBS = -lboost_system -lboost_thread -lboost_signals

all: openGlass

SRCS	=  	BaseGlass.cpp \
			OpenGlass.cpp \
			Tag.cpp \
			Message.cpp \
			TestCase.cpp \
			main.cpp

openGlass : $(OBJS)
		$(CC) $(CXXFLAGS) $(LDFLAGS) $(OBJS) $(LIBS) $(USER_LIBS) -o $@

clean: 
		$(RM) *.o openGlass
