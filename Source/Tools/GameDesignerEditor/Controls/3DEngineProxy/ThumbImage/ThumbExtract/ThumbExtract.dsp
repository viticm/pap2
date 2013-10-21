# Microsoft Developer Studio Project File - Name="ThumbExtract" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ThumbExtract - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ThumbExtract.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ThumbExtract.mak" CFG="ThumbExtract - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ThumbExtract - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ThumbExtract - Win32 Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ThumbExtract - Win32 Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ThumbExtract - Win32 Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ThumbExtract - Win32 Unicode Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ThumbExtract - Win32 Unicode Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ThumbExtract - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x408 /d "_DEBUG"
# ADD RSC /l 0x408 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 shlwapi.lib jpeg.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /libpath:"jpeg\Debug"
# Begin Custom Build - Performing registration
OutDir=.\Debug
TargetPath=.\Debug\ThumbExtract.dll
InputPath=.\Debug\ThumbExtract.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugU"
# PROP BASE Intermediate_Dir "DebugU"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugU"
# PROP Intermediate_Dir "DebugU"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x408 /d "_DEBUG"
# ADD RSC /l 0x408 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib jpeg.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /libpath:"jpeg\Debug"
# Begin Custom Build - Performing registration
OutDir=.\DebugU
TargetPath=.\DebugU\ThumbExtract.dll
InputPath=.\DebugU\ThumbExtract.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Release MinSize"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMinSize"
# PROP BASE Intermediate_Dir "ReleaseMinSize"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMinSize"
# PROP Intermediate_Dir "ReleaseMinSize"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_DLL" /D "_ATL_MIN_CRT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_DLL" /D "_ATL_MIN_CRT" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x408 /d "NDEBUG"
# ADD RSC /l 0x408 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib jpeg.lib /nologo /subsystem:windows /dll /machine:I386 /libpath:"jpeg\Release"
# Begin Custom Build - Performing registration
OutDir=.\ReleaseMinSize
TargetPath=.\ReleaseMinSize\ThumbExtract.dll
InputPath=.\ReleaseMinSize\ThumbExtract.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Release MinDependency"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMinDependency"
# PROP BASE Intermediate_Dir "ReleaseMinDependency"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMinDependency"
# PROP Intermediate_Dir "ReleaseMinDependency"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /FR /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x408 /d "NDEBUG"
# ADD RSC /l 0x408 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 shlwapi.lib jpeg.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /libpath:"jpeg\Release"
# Begin Custom Build - Performing registration
OutDir=.\ReleaseMinDependency
TargetPath=.\ReleaseMinDependency\ThumbExtract.dll
InputPath=.\ReleaseMinDependency\ThumbExtract.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Release MinSize"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseUMinSize"
# PROP BASE Intermediate_Dir "ReleaseUMinSize"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUMinSize"
# PROP Intermediate_Dir "ReleaseUMinSize"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /D "_ATL_DLL" /D "_ATL_MIN_CRT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /D "_ATL_DLL" /D "_ATL_MIN_CRT" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x408 /d "NDEBUG"
# ADD RSC /l 0x408 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib jpeg.lib /nologo /subsystem:windows /dll /machine:I386 /libpath:"jpeg\Release"
# Begin Custom Build - Performing registration
OutDir=.\ReleaseUMinSize
TargetPath=.\ReleaseUMinSize\ThumbExtract.dll
InputPath=.\ReleaseUMinSize\ThumbExtract.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Release MinDependency"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseUMinDependency"
# PROP BASE Intermediate_Dir "ReleaseUMinDependency"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUMinDependency"
# PROP Intermediate_Dir "ReleaseUMinDependency"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /FR /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x408 /d "NDEBUG"
# ADD RSC /l 0x408 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib jpeg.lib /nologo /subsystem:windows /dll /machine:I386 /libpath:"jpeg\Release"
# Begin Custom Build - Performing registration
OutDir=.\ReleaseUMinDependency
TargetPath=.\ReleaseUMinDependency\ThumbExtract.dll
InputPath=.\ReleaseUMinDependency\ThumbExtract.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "ThumbExtract - Win32 Debug"
# Name "ThumbExtract - Win32 Unicode Debug"
# Name "ThumbExtract - Win32 Release MinSize"
# Name "ThumbExtract - Win32 Release MinDependency"
# Name "ThumbExtract - Win32 Unicode Release MinSize"
# Name "ThumbExtract - Win32 Unicode Release MinDependency"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "CxImage"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=CxImage\ximaenc.cpp

!IF  "$(CFG)" == "ThumbExtract - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Release MinSize"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Release MinDependency"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Release MinSize"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Release MinDependency"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=CxImage\ximage.cpp

!IF  "$(CFG)" == "ThumbExtract - Win32 Debug"

# ADD CPP /I ".\..\CxImage\CxImage" /I "..\CxImage\jpeg"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Release MinSize"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Release MinDependency"

# ADD CPP /I ".\..\CxImage\CxImage"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Release MinSize"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Release MinDependency"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=CxImage\ximage.h
# End Source File
# Begin Source File

SOURCE=CxImage\ximajpg.cpp

!IF  "$(CFG)" == "ThumbExtract - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Release MinSize"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Release MinDependency"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Release MinSize"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Release MinDependency"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CxImage\ximalpha.cpp

!IF  "$(CFG)" == "ThumbExtract - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Release MinSize"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Release MinDependency"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Release MinSize"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Release MinDependency"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CxImage\ximapal.cpp

!IF  "$(CFG)" == "ThumbExtract - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Release MinSize"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Release MinDependency"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Release MinSize"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Release MinDependency"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CxImage\ximawnd.cpp

!IF  "$(CFG)" == "ThumbExtract - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Release MinSize"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Release MinDependency"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Release MinSize"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Release MinDependency"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=CxImage\xmemfile.cpp

!IF  "$(CFG)" == "ThumbExtract - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Release MinSize"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Release MinDependency"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Release MinSize"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Release MinDependency"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\bitmap.cpp

!IF  "$(CFG)" == "ThumbExtract - Win32 Debug"

# ADD CPP /GX

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Release MinSize"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Release MinDependency"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Release MinSize"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Release MinDependency"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FileThumbExtract.cpp

!IF  "$(CFG)" == "ThumbExtract - Win32 Debug"

# ADD CPP /GX

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Release MinSize"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Release MinDependency"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Release MinSize"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Release MinDependency"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "ThumbExtract - Win32 Debug"

# ADD CPP /GX /Yc"stdafx.h"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Debug"

# ADD CPP /Yc"stdafx.h"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Release MinSize"

# ADD CPP /Yc"stdafx.h"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Release MinDependency"

# ADD CPP /Yc"stdafx.h"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Release MinSize"

# ADD CPP /Yc"stdafx.h"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Release MinDependency"

# ADD CPP /Yc"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ThumbExtract.cpp

!IF  "$(CFG)" == "ThumbExtract - Win32 Debug"

# ADD CPP /GX

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Release MinSize"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Release MinDependency"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Release MinSize"

!ELSEIF  "$(CFG)" == "ThumbExtract - Win32 Unicode Release MinDependency"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ThumbExtract.def
# End Source File
# Begin Source File

SOURCE=.\ThumbExtract.idl
# ADD MTL /tlb ".\ThumbExtract.tlb" /h "ThumbExtract.h" /iid "ThumbExtract_i.c" /Oicf
# End Source File
# Begin Source File

SOURCE=.\ThumbExtract.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\FileThumbExtract.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\FileThumbExtract.rgs
# End Source File
# End Group
# End Target
# End Project
