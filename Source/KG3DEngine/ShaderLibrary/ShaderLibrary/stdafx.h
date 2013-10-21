////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : stdafx.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-7-30 16:36:42
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

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// TODO: reference additional headers your program requires here


#include <windows.h>

#ifndef tstring
#if defined(UNICODE) | defined(_UNICODE)
#define tstring wstring
#else
#define tstring string
#endif
#endif

#include <crtdbg.h>
#include <map>
#include <tchar.h>
#include <string>
#include <algorithm>

#define KG_PROCESS_ERROR(Condition) \
	do  \
	{   \
	if (!(Condition))   \
	goto Exit0;     \
	} while (false)


namespace KGCH
{
	struct StrPathSplited 
	{
		TCHAR driver[MAX_PATH];
		TCHAR dir[MAX_PATH];
		TCHAR fileName[MAX_PATH];
		TCHAR ext[MAX_PATH];
	};
	enum
	{
		em_path_split_driver = 0x01,
		em_path_split_dir = 0x02,
		em_path_split_filename = 0x04,
		em_path_split_ext = 0x08,
		em_path_split_all_without_driver = em_path_split_dir | em_path_split_filename | em_path_split_ext,
		em_path_split_all = em_path_split_driver | em_path_split_dir | em_path_split_filename | em_path_split_ext,
	};
	BOOL StrPathSplit( LPCTSTR path, StrPathSplited& retStruct, DWORD dwMask /*= em_path_split_all*/);
	BOOL StrPathGetFullFileName(LPCTSTR strFilePathOrFileName, std::tstring& retString);
};

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)	{if(NULL != (p)){delete (p); (p) = NULL;}}NULL
#endif

#endif //_INCLUDE_STDAFX_H_
