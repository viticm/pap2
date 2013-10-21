// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <string>

#if defined(UNICODE) || defined(_UNICODE)
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

#include "kglog.h"
#include "UpdateDefine.h"
#include "KLastError.h"

// TODO: reference additional headers your program requires here
