////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : FileEnumerForDirectory.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-5-18 10:42:03
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileEnumerForDirectory.h"
#include "Core.h"

////////////////////////////////////////////////////////////////////////////////

class CFileEnumerForDirectory : public ICFileEnumer
{
public:
	CFileEnumerForDirectory(LPCTSTR strDirectory)
		:m_strDirectory(strDirectory)
		,m_hFileFind(INVALID_HANDLE_VALUE)
		,m_strFullPath(strDirectory)
	{
		int nLen = m_strDirectory.GetLength();
		if (nLen > 0)	//如果是/或者\\结尾的，要减少一点字符长度
		{
			TCHAR chEnd = m_strDirectory.GetAt(nLen - 1);
			if (chEnd == _T('/') || chEnd == _T('\\'))
			{
				m_strDirectory.Truncate(nLen - 1);
			}
		}

	}
	~CFileEnumerForDirectory()
	{
		if(INVALID_HANDLE_VALUE != m_hFileFind)
		{
			::FindClose(m_hFileFind);
			m_hFileFind = INVALID_HANDLE_VALUE;
		}
	}
	virtual em_get_file_ret GetFirstFile(WIN32_FIND_DATA* pfileData)
	{
		CString csTemp = m_strDirectory;
		csTemp += _T("\\*.*");

		WIN32_FIND_DATA tempData;
		WIN32_FIND_DATA* pDataUse = (NULL == pfileData) ? &tempData : pfileData;

		m_hFileFind = ::FindFirstFile(csTemp, pDataUse);
		BOOL bRet = (INVALID_HANDLE_VALUE != m_hFileFind);

		if (bRet)
		{
			m_strFullPath = m_strDirectory;
			m_strFullPath += _T('\\');
			m_strFullPath += pDataUse->cFileName;
		}

		return bRet ? em_file_correct : em_no_next_file;
	}
	virtual em_get_file_ret GetNextFile(WIN32_FIND_DATA* pfileData)
	{
		if(INVALID_HANDLE_VALUE != m_hFileFind)
		{
			WIN32_FIND_DATA tempData;
			WIN32_FIND_DATA* pDataUse = (NULL == pfileData) ? &tempData : pfileData;

			BOOL bRet = ::FindNextFile(m_hFileFind, pDataUse);
			if (bRet)
			{
				m_strFullPath = m_strDirectory;
				m_strFullPath += _T('\\');
				m_strFullPath += pDataUse->cFileName;
			}
			return bRet ? em_file_correct : em_no_next_file;
		}
		return em_no_next_file;
	}
	virtual LPCTSTR FindFileAndGetFullPath()
	{
		return m_strFullPath;
	}
	virtual UINT GetFileCount()
	{
		HANDLE hFileFindTmp = 0;
		CString csTemp = m_strDirectory;
		csTemp += _T("\\*.*");

		WIN32_FIND_DATA tempData;
		hFileFindTmp = ::FindFirstFile(csTemp, &tempData);
		BOOL bRet = (INVALID_HANDLE_VALUE != hFileFindTmp);

		UINT uCount = 0;
		for(BOOL bRet = TRUE; bRet && uCount <= Core::em_file_limit; bRet = ::FindNextFile(hFileFindTmp, &tempData))
		{
			++uCount;
		}
		FindClose(hFileFindTmp);
		return uCount;
	}
private:
	CString	m_strDirectory;
	CString m_strFullPath;
	HANDLE m_hFileFind;
};

ICFileEnumer* CreateFileEnumerForDirectory( LPCTSTR strDirectory )
{
	return new CFileEnumerForDirectory(strDirectory);
}