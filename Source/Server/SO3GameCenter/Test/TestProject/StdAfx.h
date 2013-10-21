// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef _STD_AFX_H_
#define _STD_AFX_H_
#include "KGStdAfx.h"

#ifdef WIN32
#pragma warning(disable:4100)
#pragma warning(disable:4189)
#pragma warning(disable:4127)
// disable: warning C4200: nonstandard extension used : zero-sized array in struct/union
#pragma warning(disable:4200)

// warning C4201: 使用了非标准扩展
#pragma warning(disable:4201)
#include <tchar.h>
#endif

#ifdef __GNUC__
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <limits.h>
#include <dirent.h>
#endif

#ifdef WIN32
#include <conio.h>
#endif

#include "Common/KG_Socket.h"
#include "Engine.h"
#include "Engine/KG_Time.h"
#include "Engine/KGLog.h"
#include "Engine/KMemory.h"
#include "KGPublic.h"
#include "KGD_Interface.h"
#include "SO3GlobalDef.h"

#define KMEMORY_FREE(pvData)        \
	do                              \
	{                               \
	if (pvData)                 \
		{                           \
		KMemory::Free(pvData);  \
		(pvData) = NULL;        \
		}                           \
	} while (false)

#endif	//_STD_AFX_H_
