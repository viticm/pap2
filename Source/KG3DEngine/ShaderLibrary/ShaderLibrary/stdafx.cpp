////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : stdafx.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-7-30 16:36:46
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////

// stdafx.cpp : source file that includes just the standard includes
// LibTmp.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
namespace KGCH
{
	BOOL StrPathSplit( LPCTSTR path, StrPathSplited& retStruct, DWORD dwMask /*= em_path_split_all*/)
	{
		_ASSERTE(NULL != path);
		errno_t errCode = _tsplitpath_s(path
			,(dwMask & em_path_split_driver) ? retStruct.driver : NULL
			, (dwMask & em_path_split_driver) ? _countof(retStruct.driver) : 0
			, (dwMask & em_path_split_dir) ? retStruct.dir : NULL
			, (dwMask & em_path_split_dir) ?_countof(retStruct.dir) : 0
			, (dwMask & em_path_split_filename) ? retStruct.fileName : NULL
			, (dwMask & em_path_split_filename) ? _countof(retStruct.fileName) : 0
			, (dwMask & em_path_split_ext) ? retStruct.ext : NULL
			, (dwMask & em_path_split_ext) ? _countof(retStruct.ext) : 0);
		bool bRet = errCode == 0;
		return bRet;
	}
	BOOL StrPathGetFullFileName(LPCTSTR strFilePathOrFileName, std::tstring& retString)
	{
		_ASSERTE(NULL != strFilePathOrFileName);
		StrPathSplited pathSplited;
		BOOL bRet = StrPathSplit(strFilePathOrFileName, pathSplited, em_path_split_filename | em_path_split_ext);
		if (bRet)
		{
			retString = pathSplited.fileName;
			retString += pathSplited.ext;
			_ASSERTE(NULL != _tcschr(pathSplited.ext, _T('.')) && _T("后缀中应该有点号的"));
		}
		return bRet;
	}
};
