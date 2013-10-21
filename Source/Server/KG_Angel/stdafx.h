////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : stdafx.h
//  Version     : 1.0
//  Creater     : Wu Caizhong
//  Create Date : 2005-5-8 14:33:46
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_STDAFX_H_
#define _INCLUDE_STDAFX_H_

////////////////////////////////////////////////////////////////////////////////
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include "KGStdAfx.h"
// Windows Header Files:
#include <windows.h>
#endif

// TODO: reference additional headers your program requires here
#include "KGPublic.h"
#include "Engine/KGLog.h"
#include "Engine.h"
#include "SO3GlobalDef.h"
#include "SO3ProtocolBasic.h"


#endif //_INCLUDE_STDAFX_H_
