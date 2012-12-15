include /appl/openglass/openg.mk

USER_LIBS = -lboost_system -lboost_thread -lboost_signals

SRCS	= 	TestMessageMain.cpp
			
all: TestMessageMain

TestMessageMain: TestMessageMain.o 
		$(CC) $(CXXFLAGS) $(LDFLAGS) TestMessageMain.o ../Tag.o ../Message.o ../TestCase.o $(LIBS) $(USER_LIBS) -o $@

clean: 
		$(RM) *.o TestMessageMain
