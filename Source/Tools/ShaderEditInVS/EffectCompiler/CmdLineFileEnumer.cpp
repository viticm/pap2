#include "StdAfx.h"
#include "CmdLineFileEnumer.h"


#include "Core.h"
#include "../CommandLineParser/CmdLineParser.h"

class CCmdLineFileEnumer : public ICFileEnumer 
{
	friend ICFileEnumer* CreateCmdLineFileEnumer( LPCTSTR cmdLine, LPCTSTR strDirectory);
public:
	virtual em_get_file_ret GetFirstFile(WIN32_FIND_DATA* pfileData)
	{
		m_curIndex = 0;
		if (m_vecFilePaths.empty())
		{
			return em_no_next_file;
		}
		
		BOOL bRet = GetFileAttribute(m_vecFilePaths[m_curIndex], pfileData);
		if (! bRet)
		{
			if(NULL != pfileData)
			{
				_sntprintf_s(pfileData->cFileName, _countof(pfileData->cFileName), m_vecFilePaths[m_curIndex]);
			}
			return em_file_name_not_correct;
		}
		_ASSERTE(bRet);
		return em_file_correct;
	}
	virtual em_get_file_ret GetNextFile(WIN32_FIND_DATA* pfileData)
	{
		++m_curIndex;
		if (m_curIndex >= m_vecFilePaths.size())
		{
			return em_no_next_file;
		}
		BOOL bRet = GetFileAttribute(m_vecFilePaths[m_curIndex], pfileData);
		if (! bRet)
		{
			if(NULL != pfileData)
			{
				_sntprintf_s(pfileData->cFileName, _countof(pfileData->cFileName), m_vecFilePaths[m_curIndex]);
			}
			return em_file_name_not_correct;
		}
		_ASSERTE(bRet);
		return em_file_correct;
	}
	virtual LPCTSTR FindFileAndGetFullPath() 
	{
		if (m_curIndex >= m_vecFilePaths.size())
		{
			return NULL;
		}
		return Core::FindFileAndGetFullPath(m_vecFilePaths[m_curIndex], &m_strFullPath);
	}
	virtual UINT GetFileCount()
	{
		return (UINT)m_vecFilePaths.size();
	}
private:
	CCmdLineFileEnumer(LPCTSTR strDirectory):m_curIndex(0){}
	BOOL	GetFileAttribute(LPCTSTR strFilePath, WIN32_FIND_DATA* pfileData)
	{
		BOOL bRet = FALSE;
		/*HANDLE hHandle = ::CreateFile(strFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		PROCESS_ERROR(INVALID_HANDLE_VALUE != hHandle);*/
		HANDLE hHandle = ::FindFirstFile(strFilePath, pfileData);
		return INVALID_HANDLE_VALUE != hHandle;

		BY_HANDLE_FILE_INFORMATION infoCur;
		bRet = ::GetFileInformationByHandle(hHandle, &infoCur);
		PROCESS_ERROR(bRet);
		
		if (NULL != pfileData)
		{
			pfileData->dwFileAttributes = infoCur.dwFileAttributes;
			pfileData->ftCreationTime = infoCur.ftCreationTime;
			pfileData->ftLastAccessTime = infoCur.ftLastAccessTime;
			pfileData->ftLastWriteTime = infoCur.ftLastWriteTime;
			pfileData->nFileSizeHigh = infoCur.nFileSizeHigh;
			pfileData->nFileSizeLow = infoCur.nFileIndexLow;
		}

		CloseHandle(hHandle);
Exit0:
		return bRet;
	}
	std::vector<CString> m_vecFilePaths;
	size_t m_curIndex;
	CString m_strFullPath;
};

namespace
{
	LPCTSTR PathGoToNextBegin(LPCTSTR strPath)
	{
		while(*strPath == _T(' ') || *strPath == _T('\"') || *strPath == _T('\t'))
		{
			++strPath;
		}
		if(*strPath == _T('\n'))
			return NULL;
		return strPath;
	}
};

ICFileEnumer* CreateCmdLineFileEnumer( LPCTSTR strCmdLine, LPCTSTR strDirectory )
{
	CCmdLineFileEnumer* pRet = new CCmdLineFileEnumer(strDirectory);

	std::vector<CString>& vecFiles = pRet->m_vecFilePaths;
	vecFiles.reserve(Core::em_file_limit);

	LPCTSTR strFileKey = _tcsstr(strCmdLine, _T("/F:"));
	PROCESS_ERROR(strFileKey);

	TCHAR strBrk[] = _T("\" \t");	//ÒÔ·ÖºÅÎª·Ö¸ô·û£¬Ñ°ÕÒ×Ö·û´®
	strFileKey = _tcspbrk(strFileKey, strBrk);

	TCHAR pathFileNameTmp[MAX_PATH * 2] = _T("");
	for (size_t i = 0; i < Core::em_file_limit; ++i)
	{
		LPCTSTR strValue = strFileKey;
	
		strValue = PathGoToNextBegin(strValue);
		if (NULL == strValue)
		{
			break;
		}

		LPCTSTR strEnd = _tcsstr(strValue, _T(".fx"));//_tcspbrk(strValue, strBrk);
		if (NULL == strValue)
		{
			break;
		}

		strEnd += _countof(_T("fx"));

		size_t uStringLen = strEnd - strValue;
		if (uStringLen >= _countof(pathFileNameTmp) - 1)
		{
			break;
		}

		errno_t err = _tcsncpy_s(pathFileNameTmp, strValue, uStringLen);
		if (0 != err)
		{
			break;
		}

		pathFileNameTmp[_countof(pathFileNameTmp)-1] = _T('\0');

		vecFiles.push_back(CString());
		vecFiles[vecFiles.size() - 1] = pathFileNameTmp;

		strFileKey = strEnd;
		++strFileKey;
	}
	return pRet;
Exit0:
	SAFE_DELETE(pRet);
	return NULL;
}