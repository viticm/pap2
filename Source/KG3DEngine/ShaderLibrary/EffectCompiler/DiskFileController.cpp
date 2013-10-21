////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : DiskFileController.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-5-11 17:19:06
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "DiskFileController.h"
#include "Core.h"
#include "Helpers.h"
#include <iostream>
#include <direct.h>

////////////////////////////////////////////////////////////////////////////////


class CDiskFileController : public ICCompileFileController
{
	
public:
	virtual BOOL IsFileUnChanged(LPCTSTR fileName, const FILETIME& ftLastWriteTime);
	virtual BOOL SaveFileOutput(LPCTSTR fileName, const FILETIME& ftLastWriteTime, LPD3DXEFFECT pEffect,BOOL bIsIncludedFile);
	virtual BOOL CopyFileToDes(LPCTSTR fileFullPath, BOOL* pSkipped);
	virtual VOID SetPaths(LPCTSTR strOutputDir, LPCTSTR strCopytoDir);
	virtual VOID SetForceRebuild(BOOL bForceRebuild);

	CDiskFileController()
		:m_bForceRebuild(FALSE)
	{
		std::locale::global(std::locale(""));	//把环境设为系统环境，不然不支持中文
		m_StartTime = CTime::GetCurrentTime();
	}
private:
	CString m_strOutputDir;
	CString m_strCopytoDir;
	BOOL m_bForceRebuild;
	CTime m_StartTime;

	static void ProcessDirToStandard( CString& strDir)
	{
		int nLen = strDir.GetLength();
		TCHAR cLast = strDir.GetAt(nLen - 1);
		if (cLast != _T('\\') || cLast != _T('/'))
		{
			strDir += _T("\\");
		}
	}
	static VOID GetFullPath(LPCTSTR strDir, LPCTSTR fileName, LPCTSTR strExtToReplace, CString* pRet);//_T("ext要以.为开始，如.fxo"),Ext可以为NULL

	static LPCTSTR GetOutputExt()
	{
		return _T(".fxo");
	}
};

VOID CDiskFileController::GetFullPath(LPCTSTR strDir, LPCTSTR fileName, LPCTSTR strExtToReplace, CString* pRet)
{
	_ASSERTE(NULL == _tcschr(fileName, _T('\\'))  && NULL == _tcschr(fileName, _T('/')));
	_ASSERTE(NULL != pRet && NULL != fileName && NULL != strDir);

#if defined(DEBUG) | defined(_DEBUG)
	{
		size_t uLen = _tcslen(strDir);
		if (uLen != 0)
		{
			_ASSERTE(strDir[uLen-1] == _T('\\'));
		}
	}
#endif

	*pRet = strDir;
	
	if (NULL == strExtToReplace)
	{
		*pRet += fileName;
	}
	else
	{
		//如果是有扩展名的，替换扩展名，否则直接加上扩展
		LPCTSTR pPoint = _tcschr(fileName, _T('.'));
		if (NULL == pPoint)
		{
			*pRet += fileName;
		}
		else
		{
			CString csNameWithoutExt = fileName;
			INT nNewLength = (INT)((TCHAR*)pPoint - (TCHAR*)fileName);

			INT nOldLength = csNameWithoutExt.GetLength();
			_ASSERTE(nOldLength >= nNewLength);
			nOldLength;

			csNameWithoutExt.Truncate(nNewLength);
			*pRet += csNameWithoutExt;
		}
		*pRet += strExtToReplace;
	}
}

BOOL CDiskFileController::IsFileUnChanged( LPCTSTR fileName, const FILETIME& ftLastWriteTime )
{
	if (m_strOutputDir.GetLength() == 0)
	{
		return FALSE;
	}

	CString csTemp;
	
	this->GetFullPath(m_strOutputDir, fileName, this->GetOutputExt(), &csTemp);

	FILETIME fileTimeTemp;
	if (::GetSingleFileLastWriteTime(csTemp, &fileTimeTemp))
	{
		LONG nRet = ::CompareFileTime(&fileTimeTemp, &ftLastWriteTime);
		if (nRet >= 0)
		{
			if (CTime(fileTimeTemp) < m_StartTime)	//必须在程序创建之前创建的，程序内部写的不算
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CDiskFileController::SaveFileOutput( LPCTSTR fileName, const FILETIME& ftLastWriteTime, LPD3DXEFFECT pEffect, BOOL bIsIncludedFile)
{
	if (0 == m_strOutputDir.GetLength())
	{
		return FALSE;
	}
	
	CString csTemp;
	this->GetFullPath(m_strOutputDir, fileName, this->GetOutputExt(), &csTemp);

	LPD3DXBUFFER pBuffer = NULL;

	if(! bIsIncludedFile)
	{
		_ASSERTE(NULL != pEffect);
		HRESULT hr = D3DXDisassembleEffect(pEffect, FALSE, &pBuffer);
		COM_PROCESS_ERROR(hr);

		/*errno_t err = ::fopen_s(&pFile, csTemp, _T("w"));
		PROCESS_ERROR(0 == err);

		DWORD dwBufferSize = pBuffer->GetBufferSize();
		LPVOID pBufferPtr = pBuffer->GetBufferPointer();

		if(_tcsstr(fileName, "ModelShadow.fx"))
		{
		_ASSERTE(NULL);
		}
		::fwrite(pBuffer, (size_t)dwBufferSize, 1, pFile);*/	//用C的方式写会死机，不知何故

		std::ofstream of((LPCTSTR)csTemp);

		of << (char*)pBuffer->GetBufferPointer() << std::endl;
		of.close();
	}
	else
	{
		std::ofstream of((LPCTSTR)csTemp);
		of << _T("empty") << std::endl;
		of.close();
	}

	SAFE_RELEASE(pBuffer);
	
	return TRUE;
Exit0:
	return FALSE;
	SAFE_RELEASE(pBuffer);
	
}

BOOL CDiskFileController::CopyFileToDes(LPCTSTR fileFullPath, BOOL* pSkipped)
{
	_ASSERTE(NULL != fileFullPath);

	TCHAR fileName[MAX_PATH];
	TCHAR strExt[MAX_PATH];
	errno_t err = _tsplitpath_s(fileFullPath, NULL, 0, NULL, 0, fileName, _countof(fileName), strExt, _countof(strExt));
	if (0 != err)
	{
		return FALSE;
	}

	CString strCopytoFullPath;
	this->GetFullPath(m_strCopytoDir, fileName, strExt, &strCopytoFullPath);

	FILETIME ftSrc = {0,1};
	FILETIME ftDes = {0,0};

	BOOL bRet = ::GetSingleFileLastWriteTime(fileFullPath, &ftSrc);
	if (! bRet)
	{
		return FALSE;
	}

	bRet = ::GetSingleFileLastWriteTime(strCopytoFullPath, &ftDes);//目标文件可能不存在
	if (bRet)
	{
		if (0 == ::CompareFileTime(&ftSrc, &ftDes))
		{
			if (NULL != pSkipped)
			{
				*pSkipped = TRUE;
			}
			return TRUE;
		}
	}

	if (NULL != pSkipped)
	{
		*pSkipped = FALSE;
	}

	return this->CopyFileToDesDefaultImp(fileFullPath, strCopytoFullPath);
}

VOID CDiskFileController::SetPaths( LPCTSTR strOutputDir, LPCTSTR strCopytoDir )
{
	if(NULL == strOutputDir || 0 == _tcslen(strOutputDir))
	{
		m_strOutputDir = _T("");
	}
	else
	{
		m_strOutputDir = strOutputDir;
		ProcessDirToStandard(m_strOutputDir);
	}

	if(NULL == strCopytoDir || 0 == _tcslen(strCopytoDir))
	{
		m_strCopytoDir = _T("");
	}
	else
	{
		m_strCopytoDir = strCopytoDir;
		ProcessDirToStandard(m_strCopytoDir);
	}

	//不存在的临时目录要创建出来
	if(m_strOutputDir.GetLength() > 0)
	{
		HANDLE hFile = ::FindFirstFile(m_strOutputDir, NULL);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			_tmkdir(m_strOutputDir);
		}
		CloseHandle(hFile);
	}
}

VOID CDiskFileController::SetForceRebuild( BOOL bForceRebuild )
{
	m_bForceRebuild = bForceRebuild;
}
ICCompileFileController* CreateDiskFileController()
{
	return new CDiskFileController;
}