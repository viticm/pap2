//---------------------------------------------------------------------------
// Blade Engine (c) 1999-2000 by Kingsoft
//
// File:	KWin32.h
// Date:	2001.10.10
// Code:	Daphnis
// Desc:	Percompiled header files of Win32 Platform
//---------------------------------------------------------------------------
#ifndef KWin32_H
#define KWin32_H 

#pragma warning (disable: 4786)

#ifdef _STANDALONE
	#define ENGINE_API
	#define _ASSERT(x)
#else
	#ifdef ENGINE_EXPORTS
		#define ENGINE_API __declspec(dllexport)
	#else
		#define ENGINE_API __declspec(dllimport)
	#endif
#endif

#ifndef __linux
	#ifdef _STANDALONE
		#include <winsock2.h>
	#endif
	#define _WIN32_WINNT  0x0400
	#include <windows.h>
	#define DIRECTINPUT_VERSION 0x800
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	#if _MSC_VER > 1000
	#pragma once
	#endif // _MSC_VER > 1000
        #include <windows.h>
        #include <mmsystem.h>

		#ifndef	NO_DIRECT_X
			#include <dsound.h>
			#include <dinput.h>
			#include <ddraw.h>
		#endif

#else
        #define LONG long
		#define INT int
		#define LPINT int *
        #define HWND unsigned long
        #define HANDLE unsigned long
        #define LPSTR char *
        #define LPTSTR char *
        #define LPCSTR const char *
		#define LPCTSTR const char *
        #define DWORD unsigned long
		#define INT64 long long
		#define INT16 short
        #define LPVOID void *
        #define PVOID void *
        #define BOOL int
        #define TRUE 1
        #define FALSE 0
        #define BYTE unsigned char
        #define WORD unsigned short
	#define UINT unsigned int
	#define PBYTE unsigned char *
	#define LPBYTE unsigned char *
        #define LONG long
	#define strcmpi strcasecmp
	#define stricmp strcasecmp
#define IN
#define OUT
typedef struct tagRECT
{
    LONG    left;
    LONG    top;
    LONG    right;
    LONG    bottom;
} RECT, *LPRECT;

typedef struct tagPOINT
{
    LONG  x;
    LONG  y;
} POINT, *PPOINT;

typedef struct _GUID {          // size is 16
    DWORD Data1;
    WORD   Data2;
    WORD   Data3;
    BYTE  Data4[8];
} GUID;

#define MAX_PATH 300
#define CALLBACK
#include <stdarg.h> 

#define ZeroMemory(x,y) memset(x, 0, y)
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#define FILE_CURRENT	1
#define FILE_END		2
#define FILE_BEGIN		0

#include <pthread.h>
#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))

#ifdef WIN32
	#define SUCCEEDED(x)		((x) > 0)
#else
	#define SUCCEEDED(x)		((long)x >= 0)
#endif


#include <unistd.h>
#define __stdcall
#define LPDWORD unsigned long *
#define SOCKET_ERROR -1
#endif
//---------------------------------------------------------------------------
// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>


#ifndef ITOA
#define ITOA(NUMBER)  #NUMBER
#endif

#ifndef __TEXT_LINE__
#define __TEXT_LINE__(LINE) ITOA(LINE)
#endif

#ifndef KSG_ATTENTION
#define KSG_ATTENTION(MSG) __FILE__"("__TEXT_LINE__(__LINE__)") : ATTENTION "#MSG
#endif

//±‡“Î–≈œ¢ ‰≥ˆ
#ifndef __linux
	#define __STR2__(x) #x
	#define __STR1__(x) __STR2__(x)
	#define __LOC__ __FILE__ "("__STR1__(__LINE__)") : Warning: "
	#define COMPILE_MSG(msg) message(__LOC__ msg)
#else
	#define COMPILE_MSG(msg) message(msg)
#endif //#ifndef __linux

//---------------------------------------------------------------------------
#endif
