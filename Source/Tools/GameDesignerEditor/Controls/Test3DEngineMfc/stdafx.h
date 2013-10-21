// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once
#include "KGStdAfx.h"
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





#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT









#define SAFE_RENDER(p)       { if (p) { p->Render(); } }
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }

#include <list>
#include <vector>
#include <map>
#include <string>
#include <cctype>
#include <set>
#include <algorithm>

using namespace std;

#ifdef _UNICODE
#define STRING wstring
#else
#define STRING string
#endif

#ifdef _DESIGN
#define KG_INTERFACE_ROOT	: public IUnknown
#define KG_BEGIN_INTERFACE_PURE STDMETHOD_(ULONG, AddRef)() PURE;\
	STDMETHOD_(ULONG, Release)() PURE;\
	STDMETHOD(QueryInterface)(REFIID iid, void ** ppvObject) PURE;
#define KG_BEGIN_INTERFACE      STDMETHOD_(ULONG, AddRef)();\
	STDMETHOD_(ULONG, Release)();\
	STDMETHOD(QueryInterface)(REFIID iid, void ** ppvObject);
#define KG_METHODCALLTYPE		STDMETHODCALLTYPE
#else
#define KG_INTERFACE_ROOT
#define KG_BEGIN_INTERFACE_PURE
#define KG_BEGIN_INTERFACE
#define KG_METHODCALLTYPE
#endif


#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <d3d9types.h>
#include <d3dx9math.h>
#include <ddraw.h>
#include <d3d9caps.h>

#include "KGMathFunctions.h"
#include "KGCommonWidgets.h"
#include "KGHelperClasses.h"
#include "KGCommonBuffer.h"
#include "KGBitSet.h"
#include "KG3DLifeTimeDependedPointer.h"
#include "KGStringFunctions.h"
#include "mathtool.h"
#include "KG3DGraphiceEngine.h"
#include "KG3DProfileGraph.h"
#include "KOutputDebug.h"





#include "Engine/KGLog.h"
#include "KGPublic.h"
#include "SO3GlobalDef.h"
#include "Engine.h"
#include "../../KG3DEngine/KG3DEngine/KOutputDebug.h"	//<Refactor>这个文件在Engine中没有用到，却在引擎的目录中，以后是不是该移动
#include "../../KG3DEngine/KG3DEngine/KG3DStructs.h"	//<Refactor>这个文件在SceneEditor项目中被多次编译，放到预编译头比较合适
#include "../../KG3DEngine/KG3DEngine/KG3DToolsImpl.h"	//<Refactor>同上

#include "KGTester.h"

#include "KGCommonWidgets.h"
//#include "KGHelperClasses.h"
#include "KGStringFunctions.h"
#include "KGStringFunctionsEx.h"
#include "KGMathFunctions.h"
#include <afxext.h>

#include "comutil.h"
// string convertion
//#define BSTR_TO_STRING(bstr) ((TCHAR*)((_bstr_t)bstr))
//#define STRING_TO_BSTR(str) ((BSTR)((_bstr_t)str))
#define BSTR_TO_STRING(bstr) (_com_util::ConvertBSTRToString(bstr))
#define STRING_TO_BSTR(str) (_com_util::ConvertStringToBSTR(str))

