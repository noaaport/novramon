#
# $Id$
#
.include "../../Makefile.inc"

INSTALL = install
CC = cc
CXX = c++
EXTRA_CFLAGS = -c -Wall -g -DLINUX
EXTRA_CXXFLAGS = -c -Wall -g -DLINUX
LDFLAGS = -Llib

NOVRA_LIBS = -lS75
EXTRA_LIBS = -lncurses

HEADERS = lib/s75.h lib/linux_windows_types.h \
	status.h util.h version.h \
	s75.h s75p.h s75_private.h err.h logfile.h
COMMON_SOURCES = status.cc err.c util.c s75.cc s75p.cc logfile.c 
COMMON_OBJECTS = status.o err.o util.o s75.o s75p.o logfile.o

MON_SOURCE = novramon.cc
MON_TARGET = novramon
MON_OBJECTS = novramon.o

POLL_SOURCE = novrapoll.cc
POLL_OBJECTS = novrapoll.o
POLL_TARGET = novrapoll

TARGET = ${MON_TARGET} ${POLL_TARGET} 

all: ${TARGET}

${HEADERS}:
	cd lib/src; ${MAKE}

.c.o: ${HEADERS}
	${CC} ${CFLAGS} ${EXTRA_CFLAGS} $<

.cc.o: ${HEADERS}
	${CXX} ${CXXFLAGS} ${EXTRA_CXXFLAGS} $<

${MON_TARGET}: ${MON_OBJECTS} ${COMMON_OBJECTS}
	${CXX} -o ${MON_TARGET} ${MON_OBJECTS} ${COMMON_OBJECTS} \
	${LDFLAGS} ${NOVRA_LIBS} ${EXTRA_LIBS}

${POLL_TARGET}: ${POLL_OBJECTS} ${COMMON_OBJECTS}
	${CXX} -o ${POLL_TARGET} ${POLL_OBJECTS} ${COMMON_OBJECTS} \
	${LDFLAGS} ${NOVRA_LIBS} ${EXTRA_LIBS}

install: all
	${INSTALL} -m 0755 ${MON_TARGET} ${BINDIR}
	${INSTALL} -m 0755 ${POLL_TARGET} ${MYLIBEXECDIR}

clean:
	cd lib/src; ${MAKE} clean
	rm -f *~ ${TARGET} ${MON_OBJECTS} ${POLL_OBJECTS} ${COMMON_OBJECTS}
