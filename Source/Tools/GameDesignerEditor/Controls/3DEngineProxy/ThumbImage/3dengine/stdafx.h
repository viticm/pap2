// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

#define _CRT_SECURE_NO_WARNINGS

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

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include <afxole.h>         // MFC OLE classes
#include <afxwin.h>
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT

//#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>



#include <list>
#include <vector>
#include <map>
#include <string>
#include <cctype>
#include <set>
#include <algorithm>

using namespace std;
using namespace ATL;

#ifdef _UNICODE
#define STRING wstring
#else
#define STRING string
#endif



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#include "comutil.h"
#include <comdef.h> 
// string convertion
//#define BSTR_TO_STRING(bstr) ((TCHAR*)((_bstr_t)bstr))
//#define STRING_TO_BSTR(str) ((BSTR)((_bstr_t)str))
#define BSTR_TO_STRING(bstr) (_com_util::ConvertBSTRToString(bstr))
#define STRING_TO_BSTR(str) (_com_util::ConvertStringToBSTR(str))

#ifdef _DESIGN
#define KG_INTERFACE_ROOT	: public IUnknown
#define KG_BEGIN_INTERFACE 	STDMETHOD_(ULONG, AddRef)();\
	STDMETHOD_(ULONG, Release)();\
	STDMETHOD(QueryInterface)(REFIID iid, void ** ppvObject) throw();
#define KG_METHODCALLTYPE	STDMETHODCALLTYPE
#else
#define KG_INTERFACE_ROOT
#define KG_BEGIN_INTERFACE
#define KG_METHODCALLTYPE
#endif

#define SAFE_RENDER(p)       { if (p) { p->Render(); } }
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }



#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <d3d9types.h>
#include <d3dx9math.h>
#include <ddraw.h>
#include <d3d9caps.h>

#include "Engine/KGLog.h"
#include "KGPublic.h"
#include "SO3GlobalDef.h"
#include "Engine.h"
#include "../../KG3DEngine/KG3DEngine/KOutputDebug.h"	//<Refactor>这个文件在Engine中没有用到，却在引擎的目录中，以后是不是该移动
#include "../../KG3DEngine/KG3DEngine/KG3DStructs.h"	//<Refactor>这个文件在SceneEditor项目中被多次编译，放到预编译头比较合适
#include "../../KG3DEngine/KG3DEngine/KG3DToolsImpl.h"	//<Refactor>同上

//<engine helpers>
#include "KGTester.h"
#include "KGMathFunctions.h"
#include "KGCommonWidgets.h"
#include "KGHelperClasses.h"
#include "KGCommonBuffer.h"
#include "KGDesignPatternWidgets.h"
#include "KG3DGraphicHelpers.h"
#include "KGBitSet.h"
#include "KG3DLifeTimeDependedPointer.h"
#include "KGStringFunctions.h"
#include "mathtool.h"
#include "KOutputDebug.h"
#include "KGSTLWrappers.h"
//</engine helpers>

//<engine global includes>
#include "KG3DTypes.h"
//</engine global includes>


#include "KGCommonWidgets.h"
//#include "KGHelperClasses.h"
#include "KGStringFunctions.h"
#include "KGStringFunctionsEx.h"
#include "KGMathFunctions.h"
#include <afxext.h>









