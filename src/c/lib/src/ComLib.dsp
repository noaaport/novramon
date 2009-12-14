# Microsoft Developer Studio Project File - Name="ComLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=COMLIB - WIN32 RELEASE
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ComLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ComLib.mak" CFG="COMLIB - WIN32 RELEASE"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ComLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ComLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "ComLib - Win32 JSATDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "ComLib - Win32 JSATRelease" (based on "Win32 (x86) Static Library")
!MESSAGE "ComLib - Win32 JSATBroadcastPollRelease" (based on "Win32 (x86) Static Library")
!MESSAGE "ComLib - Win32 JSATBroadcastPollDebug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ComLib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "ComLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ComLib___Win32_Debug1"
# PROP BASE Intermediate_Dir "ComLib___Win32_Debug1"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_LIB" /D "JSAT" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"libtest\ComLib.lib"
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "ComLib - Win32 JSATDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ComLib___Win32_JSATDebug"
# PROP BASE Intermediate_Dir "ComLib___Win32_JSATDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ComLib___Win32_JSATDebug"
# PROP Intermediate_Dir "ComLib___Win32_JSATDebug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_LIB" /D "JSAT" /D "_AFXDLL" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "ComLib - Win32 JSATRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ComLib___Win32_JSATRelease"
# PROP BASE Intermediate_Dir "ComLib___Win32_JSATRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ComLib___Win32_JSATRelease"
# PROP Intermediate_Dir "ComLib___Win32_JSATRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "JSAT" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "ComLib - Win32 JSATBroadcastPollRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ComLib___Win32_JSATBroadcastPollRelease"
# PROP BASE Intermediate_Dir "ComLib___Win32_JSATBroadcastPollRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ComLib___Win32_JSATBroadcastPollRelease"
# PROP Intermediate_Dir "ComLib___Win32_JSATBroadcastPollRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "JSAT" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "JSAT.BROADCAST_POLL_FOR_STATUS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "ComLib - Win32 JSATBroadcastPollDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ComLib___Win32_JSATBroadcastPollDebug"
# PROP BASE Intermediate_Dir "ComLib___Win32_JSATBroadcastPollDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ComLib___Win32_JSATBroadcastPollDebug"
# PROP Intermediate_Dir "ComLib___Win32_JSATBroadcastPollDebug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_LIB" /D "JSAT" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_LIB" /D "JSAT" /D "BROADCAST_POLL_FOR_STATUS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "ComLib - Win32 Release"
# Name "ComLib - Win32 Debug"
# Name "ComLib - Win32 JSATDebug"
# Name "ComLib - Win32 JSATRelease"
# Name "ComLib - Win32 JSATBroadcastPollRelease"
# Name "ComLib - Win32 JSATBroadcastPollDebug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\A75.cpp
# End Source File
# Begin Source File

SOURCE=.\A75PesMaker.cpp
# End Source File
# Begin Source File

SOURCE=.\A75PesMakerStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\A75Status.cpp
# End Source File
# Begin Source File

SOURCE=.\ATSCDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\ATSCStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\DeviceIdentifier.cpp
# End Source File
# Begin Source File

SOURCE=.\NetworkConfigPacket.cpp
# End Source File
# Begin Source File

SOURCE=.\NovraDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\Password.cpp
# End Source File
# Begin Source File

SOURCE=.\PesMaker.cpp
# End Source File
# Begin Source File

SOURCE=.\PesMakerPIDList.cpp
# End Source File
# Begin Source File

SOURCE=.\PesMakerStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\PIDList.cpp
# End Source File
# Begin Source File

SOURCE=.\PIDPacket.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgramList.cpp
# End Source File
# Begin Source File

SOURCE=.\RFConfigPacket.cpp
# End Source File
# Begin Source File

SOURCE=.\s75_jleo.cpp
# End Source File
# Begin Source File

SOURCE=.\s75_v2.cpp
# End Source File
# Begin Source File

SOURCE=.\s75_v3.cpp
# End Source File
# Begin Source File

SOURCE=.\S75CA.cpp
# End Source File
# Begin Source File

SOURCE=.\S75CAPro.cpp
# End Source File
# Begin Source File

SOURCE=.\S75CAStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\S75FK.cpp
# End Source File
# Begin Source File

SOURCE=.\S75FKPro.cpp
# End Source File
# Begin Source File

SOURCE=.\S75functions.cpp
# End Source File
# Begin Source File

SOURCE=.\S75IPRemap.cpp
# End Source File
# Begin Source File

SOURCE=.\S75JLEOStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\S75Plus.cpp
# End Source File
# Begin Source File

SOURCE=.\S75PlusPro.cpp
# End Source File
# Begin Source File

SOURCE=.\S75PlusStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\S75V2Status.cpp
# End Source File
# Begin Source File

SOURCE=.\S75V3Status.cpp
# End Source File
# Begin Source File

SOURCE=.\SatelliteDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\SatelliteStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\StandardPIDList.cpp
# End Source File
# Begin Source File

SOURCE=.\Status.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusListner.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusPacket.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\A75.h
# End Source File
# Begin Source File

SOURCE=.\A75MSGS.h
# End Source File
# Begin Source File

SOURCE=.\A75PesMaker.h
# End Source File
# Begin Source File

SOURCE=.\A75PesMakerPackets.h
# End Source File
# Begin Source File

SOURCE=.\A75PesMakerStatus.h
# End Source File
# Begin Source File

SOURCE=.\A75Status.h
# End Source File
# Begin Source File

SOURCE=.\ATSCDevice.h
# End Source File
# Begin Source File

SOURCE=.\ATSCStatus.h
# End Source File
# Begin Source File

SOURCE=.\DeviceIdentifier.h
# End Source File
# Begin Source File

SOURCE=.\DiscoveryStats.h
# End Source File
# Begin Source File

SOURCE=.\linux_windows_types.h
# End Source File
# Begin Source File

SOURCE=.\NetworkConfigPacket.h
# End Source File
# Begin Source File

SOURCE=.\NovraDevice.h
# End Source File
# Begin Source File

SOURCE=.\NovraDevicePackets.h
# End Source File
# Begin Source File

SOURCE=.\Password.h
# End Source File
# Begin Source File

SOURCE=.\PesMaker.h
# End Source File
# Begin Source File

SOURCE=.\PesMakerPackets.h
# End Source File
# Begin Source File

SOURCE=.\PesMakerPIDList.h
# End Source File
# Begin Source File

SOURCE=.\PesMakerStatus.h
# End Source File
# Begin Source File

SOURCE=.\PIDList.h
# End Source File
# Begin Source File

SOURCE=.\PIDPacket.h
# End Source File
# Begin Source File

SOURCE=.\ProgramList.h
# End Source File
# Begin Source File

SOURCE=.\RFConfigPacket.h
# End Source File
# Begin Source File

SOURCE=.\s75.h
# End Source File
# Begin Source File

SOURCE=.\s75_jleo.h
# End Source File
# Begin Source File

SOURCE=.\s75_v2.h
# End Source File
# Begin Source File

SOURCE=.\s75_v3.h
# End Source File
# Begin Source File

SOURCE=.\S75CA.h
# End Source File
# Begin Source File

SOURCE=.\S75CAPackets.h
# End Source File
# Begin Source File

SOURCE=.\S75CAPro.h
# End Source File
# Begin Source File

SOURCE=.\S75CAProPackets.h
# End Source File
# Begin Source File

SOURCE=.\S75CAStatus.h
# End Source File
# Begin Source File

SOURCE=.\S75FK.h
# End Source File
# Begin Source File

SOURCE=.\S75FKPackets.h
# End Source File
# Begin Source File

SOURCE=.\S75FKPro.h
# End Source File
# Begin Source File

SOURCE=.\S75FKProPackets.h
# End Source File
# Begin Source File

SOURCE=.\S75IPRemap.h
# End Source File
# Begin Source File

SOURCE=.\S75IPRemapPackets.h
# End Source File
# Begin Source File

SOURCE=.\S75JLEOPackets.h
# End Source File
# Begin Source File

SOURCE=.\S75JLEOStatus.h
# End Source File
# Begin Source File

SOURCE=.\S75Plus.h
# End Source File
# Begin Source File

SOURCE=.\S75PlusPackets.h
# End Source File
# Begin Source File

SOURCE=.\S75PlusPro.h
# End Source File
# Begin Source File

SOURCE=.\S75PlusProPackets.h
# End Source File
# Begin Source File

SOURCE=.\S75PlusStatus.h
# End Source File
# Begin Source File

SOURCE=.\S75Structures.h
# End Source File
# Begin Source File

SOURCE=.\S75V2Packets.h
# End Source File
# Begin Source File

SOURCE=.\S75V2Status.h
# End Source File
# Begin Source File

SOURCE=.\S75V3Packets.h
# End Source File
# Begin Source File

SOURCE=.\S75V3Status.h
# End Source File
# Begin Source File

SOURCE=.\SatelliteDevice.h
# End Source File
# Begin Source File

SOURCE=.\SatelliteStatus.h
# End Source File
# Begin Source File

SOURCE=.\StandardPIDList.h
# End Source File
# Begin Source File

SOURCE=.\Status.h
# End Source File
# Begin Source File

SOURCE=.\StatusListner.h
# End Source File
# Begin Source File

SOURCE=.\StatusPacket.h
# End Source File
# End Group
# End Target
# End Project
