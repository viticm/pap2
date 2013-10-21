# Microsoft Developer Studio Project File - Name="Common" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Common - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Common.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Common.mak" CFG="Common - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Common - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Common - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Common - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Bin"
# PROP Intermediate_Dir "../Bin"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMMON_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMMON_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "Common - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../Bin"
# PROP Intermediate_Dir "../Bin"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMMON_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "COMMON_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Common - Win32 Release"
# Name "Common - Win32 Debug"
# Begin Group "Date Time System"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Date.cpp
# End Source File
# Begin Source File

SOURCE=.\Date.h
# End Source File
# Begin Source File

SOURCE=.\DateTimeSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\DateTimeSystem.h
# End Source File
# Begin Source File

SOURCE=.\Time.cpp
# End Source File
# Begin Source File

SOURCE=.\Time.h
# End Source File
# End Group
# Begin Group "File System"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\File.cpp
# End Source File
# Begin Source File

SOURCE=.\File.h
# End Source File
# Begin Source File

SOURCE=.\FileError.cpp
# End Source File
# Begin Source File

SOURCE=.\FileError.h
# End Source File
# Begin Source File

SOURCE=.\INIFile.cpp
# End Source File
# Begin Source File

SOURCE=.\INIFile.h
# End Source File
# Begin Source File

SOURCE=.\ROFile.cpp
# End Source File
# Begin Source File

SOURCE=.\ROFile.h
# End Source File
# Begin Source File

SOURCE=.\RWFile.cpp
# End Source File
# Begin Source File

SOURCE=.\RWFile.h
# End Source File
# Begin Source File

SOURCE=.\WOFile.cpp
# End Source File
# Begin Source File

SOURCE=.\WOFile.h
# End Source File
# End Group
# Begin Group "GEVS"

# PROP Default_Filter ""
# End Group
# Begin Source File

SOURCE=.\Assert.cpp
# End Source File
# Begin Source File

SOURCE=.\Assert.h
# End Source File
# Begin Source File

SOURCE=.\BitMask.cpp
# End Source File
# Begin Source File

SOURCE=.\BitMask.h
# End Source File
# Begin Source File

SOURCE=.\Common.cpp
# End Source File
# Begin Source File

SOURCE=.\Common.h
# End Source File
# Begin Source File

SOURCE=.\CommonDLLFuncs.h
# End Source File
# Begin Source File

SOURCE=.\ConsoleInput.cpp
# End Source File
# Begin Source File

SOURCE=.\ConsoleInput.h
# End Source File
# Begin Source File

SOURCE=.\DisableWarns.h
# End Source File
# Begin Source File

SOURCE=.\DLLAPI.h
# End Source File
# Begin Source File

SOURCE=.\ErrorReporter.cpp
# End Source File
# Begin Source File

SOURCE=.\ErrorReporter.h
# End Source File
# Begin Source File

SOURCE=.\Exception.cpp
# End Source File
# Begin Source File

SOURCE=.\Exception.h
# End Source File
# Begin Source File

SOURCE=.\ReferenceCounter.cpp
# End Source File
# Begin Source File

SOURCE=.\ReferenceCounter.h
# End Source File
# Begin Source File

SOURCE=.\Singleton.h
# End Source File
# Begin Source File

SOURCE=.\String.cpp
# End Source File
# Begin Source File

SOURCE=.\String.h
# End Source File
# Begin Source File

SOURCE=.\StringVector.cpp
# End Source File
# Begin Source File

SOURCE=.\StringVector.h
# End Source File
# End Target
# End Project
