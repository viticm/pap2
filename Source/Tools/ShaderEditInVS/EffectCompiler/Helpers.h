////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : Helpers.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-6-2 17:32:19
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_HELPERS_H_
#define _INCLUDE_HELPERS_H_

////////////////////////////////////////////////////////////////////////////////
bool StrReplaceExtension(LPCTSTR lpIn, LPCTSTR NewExtension, LPTSTR lpOut, size_t Size);//Ìæ»»ºó×º
template<size_t Size>
bool StrReplaceExtension(LPCTSTR lpIn, LPCTSTR NewExtension, TCHAR (&lpOut)[Size])
{
	return StrReplaceExtension(lpIn, NewExtension, lpOut, Size);
}

BOOL GetSingleFileLastWriteTime(LPCTSTR filePath, FILETIME* pFileTime);

#endif //_INCLUDE_HELPERS_H_
