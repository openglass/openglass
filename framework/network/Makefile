include ../mk/fog.mk

USER_LIBS= -lboost_system -lboost_thread -lboost_signals

SRCS	=  TCPClient.cpp \
		   Packetizer.cpp \
		   TCPConnection.cpp \
		   TCPClient.cpp \
		   IterativeTCPServer.cpp \
		   MultiServer.cpp \
		   Rmi.cpp \
		   RmiConnection.cpp \
		   Request.cpp

all: libnetwork.a

libnetwork.a: $(OBJS)

clean: 
		$(RM) *.o libnetwork.a
