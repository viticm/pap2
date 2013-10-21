# Microsoft Developer Studio Project File - Name="Maths" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Maths - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Maths.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Maths.mak" CFG="Maths - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Maths - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Maths - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Maths - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MATHS_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MATHS_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "Maths - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MATHS_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MATHS_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 d3dx9.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Maths - Win32 Release"
# Name "Maths - Win32 Debug"
# Begin Group "Libs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Bin\Common.lib
# End Source File
# End Group
# Begin Source File

SOURCE=.\AxisAlignedBoundingBox.cpp
# End Source File
# Begin Source File

SOURCE=.\AxisAlignedBoundingBox.h
# End Source File
# Begin Source File

SOURCE=.\Camera.cpp
# End Source File
# Begin Source File

SOURCE=.\Camera.h
# End Source File
# Begin Source File

SOURCE=.\Cone.cpp
# End Source File
# Begin Source File

SOURCE=.\Cone.h
# End Source File
# Begin Source File

SOURCE=.\LineSeg.cpp
# End Source File
# Begin Source File

SOURCE=.\LineSeg.h
# End Source File
# Begin Source File

SOURCE=.\Maths.h
# End Source File
# Begin Source File

SOURCE=.\MathsCommon.h
# End Source File
# Begin Source File

SOURCE=.\Matrix.cpp
# End Source File
# Begin Source File

SOURCE=.\Matrix.h
# End Source File
# Begin Source File

SOURCE=.\Movable.cpp
# End Source File
# Begin Source File

SOURCE=.\Movable.h
# End Source File
# Begin Source File

SOURCE=.\OrientedBox.cpp
# End Source File
# Begin Source File

SOURCE=.\OrientedBox.h
# End Source File
# Begin Source File

SOURCE=.\Plane.cpp
# End Source File
# Begin Source File

SOURCE=.\Plane.h
# End Source File
# Begin Source File

SOURCE=.\Polygon.cpp
# End Source File
# Begin Source File

SOURCE=.\Polygon.h
# End Source File
# Begin Source File

SOURCE=.\Polyhedron.cpp
# End Source File
# Begin Source File

SOURCE=.\Polyhedron.h
# End Source File
# Begin Source File

SOURCE=.\Quaternion.cpp
# End Source File
# Begin Source File

SOURCE=.\Quaternion.h
# End Source File
# Begin Source File

SOURCE=.\Rect.cpp
# End Source File
# Begin Source File

SOURCE=.\Rect.h
# End Source File
# Begin Source File

SOURCE=.\Sphere.cpp
# End Source File
# Begin Source File

SOURCE=.\Sphere.h
# End Source File
# Begin Source File

SOURCE=.\Vector.cpp
# End Source File
# Begin Source File

SOURCE=.\Vector.h
# End Source File
# Begin Source File

SOURCE=.\Vector2F.cpp
# End Source File
# Begin Source File

SOURCE=.\Vector2F.h
# End Source File
# Begin Source File

SOURCE=.\Vector2I.cpp
# End Source File
# Begin Source File

SOURCE=.\Vector2I.h
# End Source File
# Begin Source File

SOURCE=.\Vector3F.cpp
# End Source File
# Begin Source File

SOURCE=.\Vector3F.h
# End Source File
# Begin Source File

SOURCE=.\Vector3I.cpp
# End Source File
# Begin Source File

SOURCE=.\Vector3I.h
# End Source File
# Begin Source File

SOURCE=.\ViewFrustum.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewFrustum.h
# End Source File
# End Target
# End Project
