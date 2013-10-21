////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DBaseHelpers.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-3-13 17:34:25
/*  Comment     : Base库里面有很多历史代码没有使用微软的安全系列字符操作函数，
如 g_GetFilePathFromFullPath行为非常诡异，因为里面会检查目标的大小，所以要写目标的字符串的结尾才能正常工作
如TCHAR pathTemp[MAX_PATH] = _T("");
g_GetFilePathFromFullPath(pathTemp, pFullPath);这样
居然是返回FALSE的，反而没有初始化的pathTemp能得到正确结果，原因就是里面会判断pathTemp的长度，而不是让外部
传进去。
未免以后Base库的变动造成引擎的严重错误(奇怪的存盘失败，然后造成文件丢失)，最好还是自己检查一下这些奇怪的行为。
*/
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KG3DBaseHelpers.h"

namespace KG3DBASE
{

	
	__forceinline BOOL IsSeparative(TCHAR cTemp)
	{
		return cTemp == _T('/') || cTemp == _T('\\');
	}
	BOOL GetFilePathFromFullPathPrivate( LPTSTR pBuffer, size_t uSize, LPCTSTR pFullPath, LPCTSTR pRootPath)
	{
		_ASSERTE(NULL != pBuffer && NULL != pFullPath && NULL != pRootPath);
	
		size_t uFullPathSize = _tcslen(pFullPath);
		size_t uRootPathSize = _tcslen(pRootPath);

		BOOL IsColonFinded = FALSE;
		size_t i = 0;
		for(; i < uFullPathSize && i < uRootPathSize; ++i)
		{
			if (pFullPath[i] == _T(':'))
			{
				IsColonFinded = TRUE;
			}
			if(IsSeparative(pFullPath[i]) && IsSeparative(pRootPath[i]))
				continue;

			if (tolower(pFullPath[i]) != tolower(pRootPath[i]))
			{
				break;
			}
		}

		LPCTSTR pRelativePath = NULL; 
		if (! IsColonFinded)	
		{
			pRelativePath = pFullPath;	//如果没有冒号的话，那么就认为不是绝对目录
		}
		else
		{
			pRelativePath = pFullPath + i;
		}
		if(IsSeparative(pRelativePath[0]))
		{
			++pRelativePath;
		}

		errno_t errTemp = _tcscpy_s(pBuffer, uSize, pRelativePath);
		return errTemp == 0;
	}
	BOOL GetFilePathFromFullPath( LPTSTR pBuffer, size_t uSize, LPCTSTR pFullPath )
	{
		_ASSERTE(NULL != pBuffer && NULL != pFullPath);
		KG_PROCESS_ERROR(NULL != pBuffer && NULL != pFullPath);
		{
			TCHAR rootPath[MAX_PATH] = _T("");
			g_GetRootPath(rootPath);//里面没有检查Buffer长度的，直接用strcpy。
			_ASSERTE(0 != _tcscmp(rootPath, _T("")));			

			return GetFilePathFromFullPathPrivate(pBuffer, uSize, pFullPath, rootPath);
		}
Exit0:
		return FALSE;		
	}



#ifdef KG_ENABLE_TDD
KG_TEST_BEGIN(KG3DBaseHelpers)
{
	{
		struct CaseType 
		{
			TCHAR fullPath[MAX_PATH];
			TCHAR resultPath[MAX_PATH];
		};

		TCHAR rootPath1[] = _T("d:\\temp");
		TCHAR rootPath2[] = _T("d:\\temp/");
		CaseType casesTemp[] = 
		{
			{_T("D:/Temp/HelloWorld/a.fx"),	_T("HelloWorld/a.fx")},
			{_T("D:\\Temp\\HelloWorld\\a.fx"),	_T("HelloWorld\\a.fx")},
			{_T("D:/Temp\\HelloWorld/a.fx"),	_T("HelloWorld/a.fx")},
			{_T("D:/Temp/HelloWorld\\a.fx"),	_T("HelloWorld\\a.fx")},
			{_T("HelloWorld\\a.fx"),	_T("HelloWorld\\a.fx")},
			{_T("HelloWorld/a.fx"),	_T("HelloWorld/a.fx")},
		};

		TCHAR bufferTemp[MAX_PATH] = _T("");
		BOOL bRet = FALSE;
		for (size_t i = 0; i < _countof(casesTemp); ++i)
		{
			bRet = GetFilePathFromFullPathPrivate(bufferTemp, _countof(bufferTemp)
				, casesTemp[i].fullPath, rootPath1);
			_ASSERTE(bRet);
			_ASSERTE(0 == _tcscmp(casesTemp[i].resultPath, bufferTemp));

			bRet = GetFilePathFromFullPathPrivate(bufferTemp, _countof(bufferTemp)
				, casesTemp[i].fullPath, rootPath2);
			_ASSERTE(bRet);
			_ASSERTE(0 == _tcscmp(casesTemp[i].resultPath, bufferTemp));
		}
	}
}
KG_TEST_END(KG3DBaseHelpers)
#endif

};
////////////////////////////////////////////////////////////////////////////////

