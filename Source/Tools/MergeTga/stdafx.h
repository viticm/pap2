// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <GdiPlus.h>
#include <WinGDI.h>
#include <shlobj.h> 
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <commctrl.h>
#include <Commdlg.h>

// TODO: reference additional headers your program requires here
#include "KGPublic.h"
#include "tstring.h"

#pragma warning(push)
#pragma warning(disable:4819)
#include <d3d9.h>
#include <d3dx9math.h>
#pragma warning(pop)

#ifndef     WM_MOUSEWHEEL 
#define     WM_MOUSEWHEEL 0x020A
#endif