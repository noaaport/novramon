CXX = c++
CXXFLAGS += -c -I lib -DLINUX

LDFLAGS = -Llib -lRecCom

SOURCES = main.cc
OBJECTS = main.o

TARGET = test

${TARGET}: ${OBJECTS}
	${CXX} -o ${TARGET} ${OBJECTS} ${LDFLAGS}

.cc.o:
	${CXX} ${CXXFLAGS} $<
