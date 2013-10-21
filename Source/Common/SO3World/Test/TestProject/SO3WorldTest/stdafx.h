// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef _STD_AFX_H_
#define _STD_AFX_H_

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#pragma warning(disable:4100)
#pragma warning(disable:4189)
#pragma warning(disable:4127)
//disable:  warning C4201: nonstandard extension used : nameless struct/union
#pragma warning(disable:4201)

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "Global.h"
#include "Common/KG_Socket.h"
#include "Engine/KGLog.h"
#include "KGPublic.h"
#include "SO3GlobalDef.h"
#include "Engine.h"

#ifdef _UNIT_TEST
#include "KSUnitTest/KingsoftUnitTest.h"
#endif

#endif	//_STD_AFX_H_