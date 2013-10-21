// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#pragma warning(disable:4100)
#pragma warning(disable:4189)
#pragma warning(disable:4127)
#pragma warning(disable:4290)

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


#include "KGStdAfx.h"

#include "Common/KG_Socket.h"
#include "Engine.h"
#include "Engine/KG_Time.h"
#include "Engine/KGLog.h"

#include "SO3GlobalDef.h"

#include "KG3DEngine/KG3DTypes.h"
#include "KG3DEngine/KG3DInterface.h"

#include <dbghelp.h>

#include <conio.h>
#include <atltrace.h>
#include <cassert>
#include <tchar.h>

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
#include "Ksdef.h"
#include "KGPublic.h"
#include "KGLog.h"
#include "Engine.h"
#include "Engine/KG_InterlockedVariable.h"
#include "Engine/KMutex.h"
#include "Engine/KMemory.h"
#include "Common/KG_Memory.h"
//<base libray include>

//<system libray include>
#include <mmsystem.h>
#include <shlwapi.h>

#include <tchar.h>
#include <strsafe.h>

#include <list>
#include <vector>
#include <string>
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
#include <sstream>

#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <memory.h>
#include <stdlib.h>

#include <atltrace.h>
#include <atlbase.h>
//</system libray include>

//<d3d include>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <d3d9types.h>
#include <d3dx9math.h>
#include <ddraw.h>
#include <d3d9caps.h>
//</d3d include>

//<third part include>
//#include "NxPhysics.h"
//</third part include>

//<memory checking>
/*下面凡是有_KG3D_ENGINE_USE_DEBUG_NEW的都是内存泄露检查机制所需要的，
不要打乱他们之间的顺序，应该_CRTDBG_MAP_ALLOC先定义，然后加载
#include <crtdbg.h>
#include <dbghelp.h>
然后定义DEBUG_NEW*/
#if defined(DEBUG) | defined(_DEBUG)
#if _KG3D_ENGINE_USE_DEBUG_NEW == 1

#define _CRTDBG_MAP_ALLOC	//打开微软内存分配库中的内存检查策略 include Microsoft memory leak detection procedures
//#define _INC_MALLOC	     //这个查不到 exclude standard memory alloc procedures
#endif

#include <crtdbg.h>
#include <dbghelp.h>

#endif

#define _ConvertBindToLower
#ifndef _NORMAL_BLOCK
#define _NORMAL_BLOCK 1
#endif

//重载new函数，转为DebugNew的版本

#ifndef DEBUG_NEW_3DENGINE
#define DEBUG_NEW_3DENGINE new
#endif

#if defined(DEBUG) | defined(_DEBUG)
#if _KG3D_ENGINE_USE_DEBUG_NEW == 1
void* __cdecl operator new(size_t nSize, const char* lpszFileName, int nLine);
void* __cdecl operator new[](size_t nSize, int nType, LPCSTR lpszFileName, int nLine);

#ifdef DEBUG_NEW_3DENGINE
#undef DEBUG_NEW_3DENGINE
#define DEBUG_NEW_3DENGINE new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif
#endif

//</memory check>

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

//<engine global variables>
extern LPDIRECT3DDEVICE9 g_pd3dDevice;
//</engine global variables>

//#endif //_INCLUDE_STDAFX_H_

