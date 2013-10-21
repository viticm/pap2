// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes


#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif

using namespace std;


#include "d3d9.h"
#include "dxerr9.h"


//#include "stdio.h"
//#include <mmsystem.h>
#include <D3dx9math.h>

#include "KGStdAfx.h"

//#include "Engine.h"
//#include "Engine/KG_Time.h"
//#include "Engine/KGLog.h"
//
#include "KG3DEngine/KG3DTypes.h"
#include "KG3DEngine/KG3DInterface.h"
#include "IEKG3DRecPlayer.h"
#include "IEOther.h"



#include "Engine/KGLog.h"
#include "KGPublic.h"
#include "SO3GlobalDef.h"
#include "Engine.h"
//#include "../../KG3DEngine/KG3DEngine/KOutputDebug.h"	//<Refactor>这个文件在Engine中没有用到，却在引擎的目录中，以后是不是该移动
#include "../../KG3DEngine/KG3DEngine/KG3DStructs.h"	//<Refactor>这个文件在SceneEditor项目中被多次编译，放到预编译头比较合适
//#include "../../KG3DEngine/KG3DEngine/KG3DToolsImpl.h"	//<Refactor>同上

#include "KGCommonWidgets.h"
//#include "KGHelperClasses.h"
#include "KGStringFunctions.h"
#include "KGStringFunctionsEx.h"
#include "KGMathFunctions.h"


#include <dbghelp.h>

#include <conio.h>
#include <atltrace.h>
#include <cassert>

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif // #ifndef SAFE_DELETE

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif // #ifndef SAFE_DELETE_ARRAY

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif // #ifndef SAFE_RELEASE

#include <windows.h>

#include <afxdlgs.h>

extern IKG3DEngineManager *g_p3DEngine;
extern TCHAR g_szDefWorkDirectory[MAX_PATH];
extern BOOL g_bSlider;


extern IEKG3DGraphicsTool* g_p3DTools;
#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


