#
# $Id$
#
# Must be invoked defining GMAKE in the cmd line (for tinyxml):
#
# Freebsd: make GMAKE=gmake
# Linux:   make GMAKE=make
#
CXX = c++

#
# As of 3.24, none of the source files -DAPPLE or -DLINUX
#
CXXFLAGS += -c -Wall -g -DJSAT -iquote tinyxml -I /usr/local/include
CXXFLAGS += -Wno-deprecated

LDFLAGS = -L /usr/local/lib
LIBS = libRecCom.a

TARGET = ${LIBS}
PUBLIC = ${HEADERS} ${TARGET}

SOURCES = sha1.cpp A75.cpp A75V.cpp Keycrypt.cpp ParameterValue.cpp \
	EnumeratedReceiverParameter.cpp Password.cpp Receiver.cpp \
	ReceiverFactory.cpp ReceiverList.cpp ReceiverSearch.cpp S200.cpp \
	S200Base.cpp S200CA.cpp S200CAPRO.cpp S200CAPROCS.cpp S200FK.cpp \
	S200FKPRO.cpp S200PRO.cpp S200V.cpp S200VCA.cpp S200J.cpp S75CA.cpp \
	S75CAPRO.cpp S75FK.cpp S75FKPRO.cpp S75IPREMAP.cpp S75J.cpp \
	S75JPLUS.cpp S75PLUS.cpp S75PLUSPRO.cpp S75PlusBase.cpp S75V.cpp \
	S75V21.cpp S75V3.cpp SJ300.cpp SJ300Base.cpp SJBase.cpp \
	SPlusBase.cpp version.cpp timestamp.cpp itoa.cpp

TINY_SOURCES = tinyxml/tinystr.cpp \
	tinyxml/tinyxmlerror.cpp \
	tinyxml/tinyxml.cpp \
	tinyxml/tinyxmlparser.cpp

HEADERS = sha1.h A75.h A75V.h ConfigPorts.h DataTypes.h \
	EnumeratedReceiverParameter.h Keycrypt.h ParameterValue.h \
	Password.h RecComLib.h Receiver.h ReceiverFactory.h ReceiverList.h \
	ReceiverParameter.h ReceiverSearch.h S200.h S200Base.h S200CA.h \
	S200CAPRO.h S200CAPROCS.h S200FK.h S200FKPRO.h S200J.h S200PRO.h \
	S200V.h S200VCA.h S75CA.h S75CAPRO.h S75FK.h S75FKPRO.h \
	S75IPREMAP.h S75J.h S75JPLUS.h S75PLUS.h S75PLUSPRO.h S75PlusBase.h \
	S75V.h S75V21.h S75V3.h SBase.h SJ300.h SJ300Base.h SJBase.h \
	SPlusBase.h

TINY_HEADERS = tinyxml/tinystr.h \
	tinyxml/tinyxmlerror.h \
	tinyxml/tinyxml.h \
	tinyxml/tinyxmlparser.h

OBJECTS = sha1.o A75.o A75V.o Keycrypt.o ParameterValue.o \
	EnumeratedReceiverParameter.o Password.o Receiver.o \
	ReceiverFactory.o ReceiverList.o ReceiverSearch.o S200.o \
	S200Base.o S200CA.o S200CAPRO.o S200CAPROCS.o S200FK.o S200FKPRO.o \
	S200PRO.o S200V.o S200VCA.o S200J.o S75CA.o S75CAPRO.o S75FK.o \
	S75FKPRO.o S75IPREMAP.o S75J.o S75JPLUS.o S75PLUS.o S75PLUSPRO.o \
	S75PlusBase.o S75V.o S75V21.o S75V3.o SJ300.o SJ300Base.o SJBase.o \
	SPlusBase.o version.o timestamp.o itoa.o

TINY_OBJECTS = tinyxml/tinystr.o \
	tinyxml/tinyxmlerror.o \
	tinyxml/tinyxml.o \
	tinyxml/tinyxmlparser.o

all: ${TARGET}

.cc.o: ${HEADERS} ${TINY_HEADERS}
	${CXX} ${CXXFLAGS} $<

TINY_XML:
	cd tinyxml; ${GMAKE} all

${TARGET}: ${OBJECTS} TINY_XML
	ar -r -c ${TARGET} ${OBJECTS} ${TINY_OBJECTS}
	ranlib ${TARGET}
	cp ${PUBLIC} ..

clean : 
	cd ..; rm -rf ${PUBLIC}
	rm -f *~ ${OBJECTS} ${TARGET}
	cd tinyxml; ${GMAKE} clean
