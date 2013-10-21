// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef _STD_AFX_H_
#define _STD_AFX_H_

#ifdef WIN32
#pragma warning(disable:4100)
#pragma warning(disable:4189)
#pragma warning(disable:4127)

// 声明匿名结构，联合时导致警告"nonstandard extension used : nameless struct/union"
#pragma warning(disable:4201) 

// warning C4819: The file contains a character that cannot be represented in the current code page (936).
#pragma warning(disable:4819)
#endif

#include "KGStdAfx.h"

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#include <conio.h>
#include <WinCon.h>
#include <tchar.h>
#include <conio.h>
#endif

#include <string.h>

#ifdef __GNUC__
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <limits.h>
#endif

#include "Ksdef.h"

#include "Engine.h"
#include "Engine/KG_Time.h"
#include "Engine/KGLog.h"
#include "KGPublic.h"
#include "SO3GlobalDef.h"

#endif	//_STD_AFX_H_

// TODO: reference additional headers your program requires here
