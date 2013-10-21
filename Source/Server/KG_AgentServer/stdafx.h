////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : stdafx.h
//  Version     : 1.0
//  Creator     : tongxuehu, zoukewei
//  Create Date : 01/11/2007
//  Comment     : 
//  
//  Copyright (C) 2007 - All Rights Reserved
////////////////////////////////////////////////////////////////////////////////

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef _INCLUDE_STDAFX_H_7246D7BB_
#define _INCLUDE_STDAFX_H_7246D7BB_  1

//#pragma warning(disable: 4996)
#pragma warning(disable:4100)
#pragma warning(disable:4189)
#pragma warning(disable:4127)

#include "KGStdAfx.h"

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#include <mbstring.h>
#include "Engine.h"
#else
#include <fcntl.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h> 
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <vector>
#include <map>
#include <list>
#include <string>

#ifdef WIN32
#include <mbstring.h>
#endif

#include "KSUnknown.h"
#include "KGPublic.h"
#include "Engine.h"
#include "Engine/KGCRT.h"
#include "Engine/KGLog.h"
#include "Engine/KG_InterlockedVariable.h"
#include "Engine/KG_Time.h"
#include "Engine/KMutex.h"
#include "Engine/KThread.h"

#include "Common/Cipher.h"
#include "Common/KG_Memory.h"
#include "Common/KG_Socket.h"
#include "Common/KG_Package.h"


#endif // _INCLUDE_STDAFX_H_7246D7BB_