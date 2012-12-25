CC			= g++

ARFLAGS		= rv

# c++ compiler flagsut
#CXXFLAGS	= -static -m64 -g -Wno-deprecated -Wall
CXXFLAGS	= -m64 -g -Wno-deprecated -Wall

# compiler only flags
CPPFLAGS	= 

# linker only flags
#LDFLAGS		= -Map=loadmap.out --cref -t
LDFLAGS		= 

SSTM_LIBS	= -lpthread -lnsl -lm -lrt
