// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#include <stdio.h>
#include <tchar.h>

#include <iostream>
#include <list>
#include <vector>
#include <atlstr.h>
#include <string>
#include <fstream>

#ifndef tcout
#if defined(UNICODE) || defined(_UNICODE)
#define tcout	wcout
#define tstring	wstring
#define tifstream wifstream
#define tofstream wofstream
#define tfstream wfstream
#define tcerr	wcerr
#else
#define tcout	cout
#define tstring	string
#define tifstream ifstream
#define tofstream ofstream
#define tfstream	fstream
#define tcerr	cerr
#endif
#endif

struct TypeShaderFile 
{
	CString strFilePath;
	CString strFileName;
	CString	strFileMacro;
};
typedef std::list<TypeShaderFile> TypeShaderFileCt;
// TODO: reference additional headers your program requires here
