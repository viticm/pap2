// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#pragma warning(disable: 4102)
#pragma warning(disable: 4244)
#pragma warning(disable: 4290)
#pragma warning(disable: 4996)
#pragma warning(disable: 4800)

#include "KGStdAfx.h"
#ifdef WIN32
#include <conio.h>
#include <tchar.h>
#else
#include "Ksdef.h"
#endif

#ifndef STRICT
#define STRICT
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

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include <afxwin.h>
#include <afxcmn.h>
#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include "comutil.h" // _bstr_t

using namespace ATL;

#define BSTR_TO_STRING(bstr) (_com_util::ConvertBSTRToString(bstr))
#define STRING_TO_BSTR(str) (_com_util::ConvertStringToBSTR(str))

#ifdef _DESIGN
#define KG_INTERFACE_ROOT		: public IUnknown
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

int ShowProgress(LPCTSTR pName, float fPencent);



//====================================================================================================
//=====================================这就是华丽的分割线=============================================


#ifndef SAFE_RENDER
#define SAFE_RENDER(p, u, w)    { if (p) { p->Render(u, w); } }
#endif // #ifndef SAFE_RENDER

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif // #ifndef SAFE_DELETE

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif // #ifndef SAFE_DELETE_ARRAY

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif // #ifndef SAFE_RELEASE

#define WIN32_LEAN_AND_MEAN

#define STRSAFE_NO_DEPRECATE
#define NOMINMAX

#include <tchar.h>
#include <strsafe.h>

#include "Ksdef.h"
#include "KGPublic.h"
#include "Engine/KGLog.h"

#ifdef _DEBUG
#undef KGLOG_PROCESS_ERROR
#define KGLOG_PROCESS_ERROR(Condition) \
	do  \
	{   \
		if (!(Condition))       \
		{                       \
			KGLogPrintf(        \
			KGLOG_DEBUG,		\
			"KGLOG_PROCESS_ERROR(%s) at line %d in %s\n", #Condition, __LINE__, KG_FUNCTION  \
		);                  \
		DebugBreak();		\
		goto Exit0;         \
	}                       \
} while (false)
#endif

#include "Engine/KG_Time.h"
#include "Engine.h"


#include <mmsystem.h>
#include <shlwapi.h>

#include <list>
#include <vector>
#include <string>
#include <cctype>
#include <queue>
#include <set>
#include <bitset>
#include <map>
#include <hash_map>
#include <algorithm>
#include <functional>
#include <limits>
#include <new>
#include <iostream>

#ifdef _UNICODE
#define STRING wstring
#else
#define STRING string
#endif

#include <assert.h>
#include <malloc.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <memory.h>

#include <atltrace.h>
#include <atlbase.h>

#include <d3d9.h>
#include <d3dx9.h>
#include <d3d9types.h>
#include <d3dx9math.h>
#include <ddraw.h>
#include <d3d9caps.h>



#include "IEOther.h"

// 逻辑拆分
#include "Stolen/KSceneInterfaceGetters.h"
#include "SO3GlobalDef.h"
#include "KSO3World.h"
#include "ProtocolBasic.h"
#include "Common/KG_Socket.h"
#include "KGTester.h"
#include "KGMathFunctions.h"
#include "KGCommonWidgets.h"
#include "KGHelperClasses.h"
#include "KGCommonBuffer.h"
#include "KGBitSet.h"
#include "KG3DLifeTimeDependedPointer.h"
#include "KGStringFunctions.h"
#include "mathtool.h"
#include "KG3DGraphiceEngine.h"
#include "KOutputDebug.h"
#include "KGSTLWrappers.h"
#include "KG3DTypes.h"
#include "KG3DEngineTypes.h"

#define _ConvertBindToLower

#ifndef _NORMAL_BLOCK
#define _NORMAL_BLOCK 1
#endif

#ifdef _DEBUG
void* __cdecl operator new(size_t nSize, const char* lpszFileName, int nLine);
void* __cdecl operator new[](size_t nSize, int nType, LPCSTR lpszFileName, int nLine);
#undef DEBUG_NEW_3DENGINE
#define DEBUG_NEW_3DENGINE new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif

using namespace std;
