### Do not edit -- Generated by 'configure --with-whatever' from Makefile.in
### STK eguitar Makefile - for various flavors of unix

PROGRAMS = 
RM = /bin/rm
SRC_PATH = ../../src
OBJECT_PATH = Release
vpath %.o $(OBJECT_PATH)

OBJECTS	=	Stk.o Filter.o Fir.o Delay.o DelayL.o DelayA.o OnePole.o \
					Effect.o JCRev.o Twang.o \
					Guitar.o Noise.o Cubic.o \
					FileRead.o WvIn.o FileWvIn.o FileWrite.o FileWvOut.o \
					Skini.o Messager.o utilities.o

INCLUDE = 
ifeq ($(strip $(INCLUDE)), )
	INCLUDE = ../../include
endif
vpath %.h $(INCLUDE)

CC       = g++
DEFS     =   -DHAVE_GETTIMEOFDAY -D__MACOSX_CORE__
DEFS    += -D__LITTLE_ENDIAN__
CFLAGS   = -O3 -Wall
CFLAGS  += -I$(INCLUDE) -I$(INCLUDE)/../src/include
LIBRARY = -lpthread -framework CoreAudio -framework CoreFoundation -framework CoreMIDI

REALTIME = yes
ifeq ($(REALTIME),yes)
  PROGRAMS += eguitar
	OBJECTS += RtMidi.o RtAudio.o Thread.o Mutex.o Socket.o TcpServer.o 
endif

RAWWAVES = 
ifeq ($(strip $(RAWWAVES)), )
	RAWWAVES = ../../rawwaves/
endif
DEFS    += -DRAWWAVE_PATH=\"$(RAWWAVES)\"

%.o : $(SRC_PATH)/%.cpp $(OBJECT_PATH)/.placeholder
	$(CC) $(CFLAGS) $(DEFS) -c $(<) -o $(OBJECT_PATH)/$@

%.o : ../../src/include/%.cpp $(OBJECT_PATH)/.placeholder
	$(CC) $(CFLAGS) $(DEFS) -c $(<) -o $(OBJECT_PATH)/$@

all : $(PROGRAMS)

$(OBJECT_PATH)/.placeholder:
	mkdir -vp $(OBJECT_PATH)
	touch $(OBJECT_PATH)/.placeholder

eguitar: eguitar.cpp $(OBJECTS)
	$(CC) $(LDFLAGS) $(CFLAGS) $(DEFS) -o eguitar eguitar.cpp $(OBJECT_PATH)/*.o $(LIBRARY)

libeguitar: eguitar.cpp
	$(CC) $(LDFLAGS) $(CFLAGS) $(DEFS) -o eguitar utilities.cpp eguitar.cpp -L../../src -lstk $(LIBRARY)

$(OBJECTS) : Stk.h

clean : 
	$(RM) -f $(OBJECT_PATH)/*.o
	$(RM) -f $(PROGRAMS) *.exe
	$(RM) -fR *~ *.dSYM

distclean: clean
	$(RM) Makefile

strip : 
	strip $(PROGRAMS)

# Project specific objects:

utilities.o: utilities.cpp
	$(CC) $(CFLAGS) $(DEFS) -c utilities.cpp -o $(OBJECT_PATH)/$@
