##
##************************************************************************
##
##	File:	Makefile
##
##************************************************************************
##


############################# COMPILE MACROS #############################

CCC=g++
CCFLAGS=-c -O -Wall
CPPFLAGS=-I/usr/local/include -DLINUX

COMPILE=$(CCC) $(CCFLAGS) $(CPPFLAGS)


############################# LINKER MACROS #############################

LIBDIR=
LDFLAGS=-L/usr/local/lib
LIBS=S75
LDLIBS=


################################ OBJECTS ################################

include ofiles.incl



##################### EXECUTABLES (BUILDING RULES) ######################

TESTHASH : $(OFILES)
	$(CCC) $(OFILES) $(LDLIBS) -o TESTHASH; strip --strip-unneeded TESTHASH


####################### OBJECTS (BUILDING RULES) ########################

.cc.o: $<
	$(COMPILE) $< -o $@

############################### TARGETS #################################

all : TESTHASH

clean : 
	rm $(OFILES) TESTHASH 
