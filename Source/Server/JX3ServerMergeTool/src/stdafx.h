// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef _STD_AFX_H_
#define _STD_AFX_H_
#include "KGStdAfx.h"		

#include <stdio.h>
#ifdef WIN32
#include <tchar.h>
#endif
#include "string.h"
//#include "Common/KG_Socket.h"
#include "Engine.h"
// #include "Engine/KG_Time.h"
#include "Engine/KGLog.h"
#include "Engine/KMemory.h"
#include "KGPublic.h"
// #include "KGD_Interface.h"
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
