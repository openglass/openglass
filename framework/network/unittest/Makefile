include ../../mk/fog.mk

USER_LIBS = -lboost_system -lboost_thread -lboost_signals

all: main main_client main_rmiclient main_rmiserver

main_client: main_client.o
		$(CC) $(CXXFLAGS) $(LDFLAGS) main_client.o $(LIBS) -o $@

main_rmiclient: BaseGlass.o OpenGlassClient.o main_rmiclient.o
		$(CC) $(CXXFLAGS) $(LDFLAGS) BaseGlass.o OpenGlassClient.o main_rmiclient.o $(LIBS) -o $@

main: main.o
		$(CC) $(CXXFLAGS) $(LDFLAGS) main.o $(LIBS) -o $@

main_rmiserver: BaseGlass.o OpenGlass.o main_rmiserver.o
		$(CC) $(CXXFLAGS) $(LDFLAGS) BaseGlass.o OpenGlass.o main_rmiserver.o $(LIBS) -o $@
clean: 
		$(RM) *.o main main_client main_rmiclient main_rmiserver
