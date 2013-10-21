# Microsoft Developer Studio Project File - Name="Render" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Render - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Render.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Render.mak" CFG="Render - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Render - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Render - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Render - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RENDER_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RENDER_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 d3d9.lib d3dx9.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "Render - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RENDER_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RENDER_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 d3d9.lib d3dx9.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Render - Win32 Release"
# Name "Render - Win32 Debug"
# Begin Group "Libs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Bin\Maths.lib
# End Source File
# Begin Source File

SOURCE=..\Bin\Common.lib
# End Source File
# End Group
# Begin Group "Color"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\A8R8G8B8Color.cpp
# End Source File
# Begin Source File

SOURCE=.\A8R8G8B8Color.h
# End Source File
# Begin Source File

SOURCE=.\Color.h
# End Source File
# Begin Source File

SOURCE=.\ColorValue.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorValue.h
# End Source File
# Begin Source File

SOURCE=.\R5G6B5Color.cpp
# End Source File
# Begin Source File

SOURCE=.\R5G6B5Color.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\BlendMode.cpp
# End Source File
# Begin Source File

SOURCE=.\BlendMode.h
# End Source File
# Begin Source File

SOURCE=.\D3DEnumeration.cpp
# End Source File
# Begin Source File

SOURCE=.\D3DEnumeration.h
# End Source File
# Begin Source File

SOURCE=.\Render.cpp
# End Source File
# Begin Source File

SOURCE=.\Render.h
# End Source File
# Begin Source File

SOURCE=.\Renderable.cpp
# End Source File
# Begin Source File

SOURCE=.\Renderable.h
# End Source File
# Begin Source File

SOURCE=.\RenderCommon.h
# End Source File
# Begin Source File

SOURCE=.\RenderCore.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderCore.h
# End Source File
# Begin Source File

SOURCE=.\RenderDLLFuncs.h
# End Source File
# Begin Source File

SOURCE=.\RenderState.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderState.h
# End Source File
# End Target
# End Project
