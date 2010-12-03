# Microsoft Developer Studio Project File - Name="S75_DLL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=S75_DLL - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "S75_DLL.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "S75_DLL.mak" CFG="S75_DLL - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "S75_DLL - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "S75_DLL - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "S75_DLL - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "S75_DLL_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "S75_DLL_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 iphlpapi.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "S75_DLL - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "S75_DLL_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "S75_DLL_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 iphlpapi.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /incremental:no /map

!ENDIF 

# Begin Target

# Name "S75_DLL - Win32 Release"
# Name "S75_DLL - Win32 Debug"
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

SOURCE=.\NovraDevice.cpp
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

SOURCE=.\ProgramList.cpp
# End Source File
# Begin Source File

SOURCE=.\S75_DLL.cpp
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

SOURCE=.\S75CAStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\S75functions.cpp
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

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\A75PesMaker.h
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

SOURCE=.\PesMakerPIDList.h
# End Source File
# Begin Source File

SOURCE=.\PesMakerStatus.h
# End Source File
# Begin Source File

SOURCE=.\PIDList.h
# End Source File
# Begin Source File

SOURCE=.\ProgramList.h
# End Source File
# Begin Source File

SOURCE=.\S75CA.h
# End Source File
# Begin Source File

SOURCE=.\S75CAStatus.h
# End Source File
# Begin Source File

SOURCE=.\S75V2Status.h
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

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
