// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

#include "kiscom_v2/KISCOM_AdvRegistery.h"

// TODO: reference additional headers your program requires here
#include <string>
#include <algorithm>
#include <Tlhelp32.h>
#include "psapi.h"
#include "shlwapi.h"
#include "KGPublic.h"
#include <tchar.h>

#include <atlexcept.h>      //kiscom_v2÷–”√“Ï≥£

#pragma warning(disable: 4996)

#if defined(UNICODE) | defined(_UNICODE)
#define TSTRING std::wstring
#define TSTRING_NPOS std::wstring::npos
#define TSPRINTF swprintf
#else
#define TSTRING std::string
#define TSTRING_NPOS std::string::npos
#define TSPRINTF sprintf
#endif

