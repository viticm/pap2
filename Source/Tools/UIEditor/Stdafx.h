// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#include "KGStdAfx.h"

#include "Common/KG_Socket.h"
#include "Engine/KGLog.h"
#include "Common/KG_Memory.h"
#include "Engine.h"

#include "ProtocolBasic.h"

#include "SO3GlobalDef.h"

#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#include <mmsystem.h>
#include <shlobj.h>
#include <GdiPlus.h>
#include <WinGDI.h>

#pragma warning(push)
#pragma warning(disable:4819)
#include <d3d9.h>
#include <d3dx9math.h>
#pragma warning(pop)

#include <crtdbg.h>
#include <stdarg.h>
#include <stdio.h>
#include <conio.h>

#include <atltrace.h>
#include <atlconv.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <hash_map>

#include "tstring.h"

// TODO: reference additional headers your program requires here

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif // #ifndef SAFE_DELETE

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif // #ifndef SAFE_DELETE_ARRAY

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif // #ifndef SAFE_RELEASE





