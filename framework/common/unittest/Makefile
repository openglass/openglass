include ../../mk/fog.mk

USER_LIBS=

SRCS	= 	test2.cpp \
			main_command.cpp
			
all: test2 main_command

test2: test2.o
		$(CC) $(CXXFLAGS) $(LDFLAGS) test2.o $(LIBS) -o $@

main_command: main_command.o
		$(CC) $(CXXFLAGS) $(LDFLAGS) main_command.o $(LIBS) -o $@

clean: 
		$(RM) *.o test main_command
