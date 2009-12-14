#
# $Id$
#
CXX = c++

CXXFLAGS = -c -Wall -g -DLINUX -DJSAT
CXXFLAGS += -Wno-deprecated

TARGET = libS75.a
PUBLIC = s75.h linux_windows_types.h ${TARGET}

SOURCES = A75.cc A75PesMaker.cc A75PesMakerStatus.cc A75Status.cc \
ATSCDevice.cc ATSCStatus.cc DeviceIdentifier.cc NetworkConfigPacket.cc \
NovraDevice.cc PIDList.cc PIDPacket.cc Password.cc PesMaker.cc \
PesMakerPIDList.cc PesMakerStatus.cc ProgramList.cc RFConfigPacket.cc \
S75CA.cc S75CAPro.cc S75CAStatus.cc S75FK.cc S75FKPro.cc S75JLEOStatus.cc \
S75Plus.cc S75PlusPro.cc S75PlusStatus.cc S75IPRemap.cc \
S75V2Status.cc S75V3Status.cc \
S75functions.cc SatelliteDevice.cc SatelliteStatus.cc StandardPIDList.cc \
Status.cc StatusListner.cc StatusPacket.cc s75_jleo.cc s75_v2.cc s75_v3.cc


OBJECTS = A75.o A75PesMaker.o A75PesMakerStatus.o A75Status.o \
ATSCDevice.o ATSCStatus.o DeviceIdentifier.o NetworkConfigPacket.o \
NovraDevice.o PIDList.o PIDPacket.o Password.o PesMaker.o PesMakerPIDList.o \
PesMakerStatus.o ProgramList.o RFConfigPacket.o S75CA.o S75CAPro.o \
S75CAStatus.o S75FK.o S75FKPro.o S75JLEOStatus.o S75Plus.o S75PlusPro.o \
S75PlusStatus.o S75IPRemap.o \
S75V2Status.o S75V3Status.o S75functions.o SatelliteDevice.o \
SatelliteStatus.o StandardPIDList.o Status.o StatusListner.o StatusPacket.o \
s75_jleo.o s75_v2.o s75_v3.o

HEADERS = A75.h \
A75MSGS.h \
A75PesMaker.h \
A75PesMakerPackets.h \
A75PesMakerStatus.h \
A75Status.h \
ATSCDevice.h \
ATSCStatus.h \
DeviceIdentifier.h \
NetworkConfigPacket.h \
NovraDevice.h \
NovraDevicePackets.h \
PIDList.h \
PIDPacket.h \
Password.h \
PesMaker.h \
PesMakerPIDList.h \
PesMakerPackets.h \
PesMakerStatus.h \
ProgramList.h \
RFConfigPacket.h \
S75CA.h \
S75CAPackets.h \
S75CAPro.h \
S75CAProPackets.h \
S75CAStatus.h \
S75FK.h \
S75FKPackets.h \
S75FKPro.h \
S75FKProPackets.h \
S75IPRemap.h \
S75IPRemapPackets.h \
S75JLEOPackets.h \
S75JLEOStatus.h \
S75Plus.h \
S75PlusPackets.h \
S75PlusPro.h \
S75PlusProPackets.h \
S75PlusStatus.h \
S75V2Packets.h \
S75V2Status.h \
S75V3Packets.h \
S75V3Status.h \
S75_PRT.h \
SatelliteDevice.h \
SatelliteStatus.h \
StandardPIDList.h \
Status.h \
StatusListner.h \
StatusPacket.h \
linux_windows_types.h \
resource.h \
s75.h \
s75_jleo.h \
s75_v2.h \
s75_v3.h \
stdafx.h

all: ${TARGET}

.cc.o: ${HEADERS}
	${CXX} ${CXXFLAGS} $<

${TARGET}: ${OBJECTS}
	ar -r ${TARGET} ${OBJECTS}
	ranlib ${TARGET}
	cp ${PUBLIC} ..

clean : 
	cd ..; rm -rf ${PUBLIC}
	rm -f *~ ${OBJECTS} ${TARGET}
