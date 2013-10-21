////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : Helpers.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-6-2 17:32:17
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Helpers.h"


////////////////////////////////////////////////////////////////////////////////

bool StrReplaceExtension( LPCTSTR lpIn, LPCTSTR NewExtension, LPTSTR lpOut, size_t Size )
{
	if(NULL == lpOut || NULL == NewExtension || NULL == lpIn)
		return false;
	size_t InSize = _tcslen(lpIn);
	size_t NewExtSize = _tcslen(NewExtension);

	if(InSize+1 > Size)
		return false;

	bool bExternsionHasDotHead = false;
	if(NewExtension[0] == '.')
	{
		bExternsionHasDotHead = true;
		NewExtSize -= 1;
	}
	if(NewExtSize > Size)
		return false;

	if (lpIn != lpOut)
	{
		if(_tcscpy_s(lpOut, Size, lpIn) != 0)
			return false;
	}

	TCHAR* pRet = _tcsrchr(lpOut, '.');
	if(pRet == 0)
		return false;

	size_t SizeLeft = Size - (pRet - lpOut) - 1;
	if(SizeLeft > Size)//溢出的时候会这样
		return false;

	if(SizeLeft < NewExtSize+1)
		return false;

	errno_t err = _tcscpy_s(pRet+1, SizeLeft, bExternsionHasDotHead ? NewExtension+1 : NewExtension);
	if(err != 0)
		return false;
	return true;	
}

BOOL GetSingleFileLastWriteTime( LPCTSTR filePath, FILETIME* pFileTime )
{
	_ASSERTE(NULL != filePath && NULL != pFileTime);
	WIN32_FIND_DATA findData;
	HANDLE hFile = ::FindFirstFile(filePath, &findData);
	if (INVALID_HANDLE_VALUE != hFile)
	{
		*pFileTime = findData.ftLastWriteTime;
		::FindClose(hFile);
		return TRUE;
	}
	return FALSE;
}