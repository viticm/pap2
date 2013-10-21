// stdafx.cpp : source file that includes just the standard includes
//	SceneEditor.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

//(by dengzhihui 2006Äê9ÔÂ18ÈÕ 17:44:41
//ÄÚ´æÐ¹Â©²âÊÔ°ïÖú

#ifdef _DEBUG
void* __cdecl operator new(size_t nSize, const char* lpszFileName, int nLine)
{
	return ::operator new(nSize, _NORMAL_BLOCK, lpszFileName, nLine);
}

#if _MSC_VER >= 1200
void __cdecl operator delete(void* p, LPCSTR lpszFileName , int  nLine )
{
#if !defined(_AFX_NO_DEBUG_CRT) && defined(_DEBUG)
	_free_dbg(p, _NORMAL_BLOCK);
#else
	free(p);
#endif
}
#endif // _MSC_VER >= 1200

#undef DEBUG_NEW
#define DEBUG_NEW new(THIS_FILE, __LINE__)
#define new DEBUG_NEW
#endif

//)