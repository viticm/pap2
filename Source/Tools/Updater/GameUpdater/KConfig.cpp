#include "StdAfx.h"
#include "KConfig.h"
#include "KLastError.h"
#include <direct.h>
#include <errno.h>
#include "KRegInfo.h"
#include "KVersionNumber.h"
#include "IUpdateEvent.h"

namespace
{
	void MkDirEx(const TCHAR cszPathDir[])
	{
		int nRetCode = FALSE;
		TCHAR szDir[MAX_PATH]    = {0};
		TCHAR szTemp[MAX_PATH]   = {0};
		TCHAR *pszPos            = NULL;
		TCHAR *pszTemp           = NULL;
		int nLen                 = 0;

		nRetCode = _tmkdir(cszPathDir);
		if (nRetCode == 0)
		{
			goto Exit0;
		}
		else if (nRetCode == -1)
		{
			if (errno == EEXIST)
				goto Exit0;
		}

		_tcsncpy(szTemp, cszPathDir, sizeof(szTemp) / sizeof(TCHAR));
		szTemp[sizeof(szTemp) / sizeof(TCHAR) - 1] = _T('\0');
		pszPos = _tcschr(szTemp, _T('\\'));
		pszTemp = szTemp;

		while(pszPos != NULL)
		{
			if (*szDir == _T('\0'))
				_tcsncpy(szDir, pszTemp, pszPos - pszTemp);
			else
				_tcsncat(szDir, pszTemp, pszPos - pszTemp);

			pszTemp = pszPos + 1;        

			nLen = (int)_tcslen(szDir);
			if (szDir[nLen - 1] != _T(':'))//such as "c:"
			{	
				_tmkdir(szDir);
			}
			_tcscat(szDir, _T("\\"));

			pszPos = _tcschr(pszTemp, _T('\\'));
		}

		if (*szDir == _T('\0'))
			_tcscpy(szDir, pszTemp);
		else
			_tcscat(szDir, pszTemp);

		_tmkdir(szDir);

Exit0:
		return;
	}
}


KConfig::KConfig(void)
{	
	memset(&m_UpdateServer, 0, sizeof(UPDATE_SERVER));

	m_szAppPath[0]         = _T('\0');
	m_szSavePath[0]        = _T('\0');
	m_szCurrVersion[0]     = _T('\0');
	m_szVersionLineName[0] = _T('\0');
	m_szVersionFullName[0] = _T('\0');
	m_szVersionEx[0]       = _T('\0');
	m_szVersionName[0]     = _T('\0');
	m_szBulletinURL[0]     = _T('\0');

    m_szDownloadURL[0]     = _T('\0');
}

KConfig::~KConfig(void)
{
}

int KConfig::LoadConfig()
{
	int nRetCode = false;
	int nResult  = false;

	TCHAR *pSlash = NULL;

	nRetCode = GetModuleFileName(NULL, m_szAppPath, sizeof(m_szAppPath) / sizeof(TCHAR));
	KGLOG_PROCESS_ERROR(nRetCode);

	pSlash = _tcsrchr(m_szAppPath, _T('\\'));
	KGLOG_PROCESS_ERROR(pSlash);
	*(pSlash + 1) = _T('\0');

	_tcscpy(m_szSavePath, m_szAppPath);
	_tcscat(m_szSavePath, _T("update\\"));

	nRetCode = _ReadVersionInfo();
	KGLOG_PROCESS_ERROR(nRetCode && "version.cfg");

	nRetCode = _ReadProfile();
	KGLOG_PROCESS_ERROR(nRetCode && "gameupdater.ini");

	MkDirEx(m_szSavePath);

	nResult = true;
Exit0:
	if (!nResult)
		KLastError::SetErrorCode(UPDATE_ERR_LACKING_NECESSARY_FILE);
	
	return nResult;
}

int KConfig::_ReadVersionInfo()
{
	int nRetCode = false;
	int nResult  = false;
	FILE* pVerFile = NULL;
	TCHAR szVersionFile[MAX_PATH];    
	TCHAR szBuffer[256];   

	_tcsncpy(szVersionFile, m_szAppPath, sizeof(szVersionFile) / sizeof(TCHAR));
	szVersionFile[sizeof(szVersionFile) / sizeof(TCHAR) - 1] = _T('\0');
	nRetCode = (int)(sizeof(szVersionFile) / sizeof(TCHAR) - _tcslen(szVersionFile));
	_tcsncat(szVersionFile, VERSION_FILE_NAME, nRetCode);
	szVersionFile[sizeof(szVersionFile) / sizeof(TCHAR) - 1] = _T('\0');

	pVerFile = _tfopen(szVersionFile, _T("r"));
	KGLOG_PROCESS_ERROR(pVerFile);

	while (TRUE)
	{
		size_t uLength = 0;
		TCHAR *pRet = _fgetts(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), pVerFile);
		if (!pRet)
		{
			if (ferror(pVerFile))
				goto Exit0;
			else
				goto Exit1;
		}
		szBuffer[sizeof(szBuffer) / sizeof(TCHAR) - 1] = _T('\0');

		_StringTrim(szBuffer);

		if (szBuffer[0] == _T('#'))
			continue;

		uLength = _tcslen(PRODUCT_VERSION_KEY);
		nRetCode = _tcsncicmp(szBuffer, PRODUCT_VERSION_KEY, uLength);
		if (!nRetCode && szBuffer[uLength] == _T('='))
		{
			_tcsncpy(m_szCurrVersion, szBuffer + uLength + 1, sizeof(m_szCurrVersion) / sizeof(TCHAR));
			m_szCurrVersion[sizeof(m_szCurrVersion) / sizeof(TCHAR) - 1] = _T('\0');

			_FormatVersion(m_szCurrVersion, _T('.'));
		}		

		uLength = _tcslen(PRODUCT_VERSION_NAME_KEY);
		nRetCode = _tcsncicmp(szBuffer, PRODUCT_VERSION_NAME_KEY, uLength);
		if (!nRetCode && szBuffer[uLength] == _T('='))
		{
			_tcsncpy(m_szVersionLineName, szBuffer + uLength + 1, sizeof(m_szVersionLineName) / sizeof(TCHAR));
			m_szVersionLineName[sizeof(m_szVersionLineName) / sizeof(TCHAR) - 1] = _T('\0');
		}

		uLength = _tcslen(PRODUCT_VERSION_EX_KEY);
		nRetCode = _tcsncicmp(szBuffer, PRODUCT_VERSION_EX_KEY, uLength);
		if (!nRetCode && szBuffer[uLength] == _T('='))
		{
			_tcsncpy(m_szVersionEx, szBuffer + uLength + 1, sizeof(m_szVersionEx) / sizeof(TCHAR));
			m_szVersionEx[sizeof(m_szVersionEx) / sizeof(TCHAR) - 1] = _T('\0');
		}

		uLength = _tcslen(PRODUCT_VERSION_PRODUCT_NAME_KEY);
		nRetCode = _tcsncicmp(szBuffer, PRODUCT_VERSION_PRODUCT_NAME_KEY, uLength);
		if (!nRetCode && szBuffer[uLength] == _T('='))
		{
			_tcsncpy(m_szVersionName, szBuffer + uLength + 1, sizeof(m_szVersionName) / sizeof(TCHAR));
			m_szVersionName[sizeof(m_szVersionName) / sizeof(TCHAR) - 1] = _T('\0');
		}
	}

Exit1:
	KGLOG_PROCESS_ERROR(m_szCurrVersion[0] != _T('\0'));
	KGLOG_PROCESS_ERROR(m_szVersionLineName[0] != _T('\0'));

	if (m_szVersionEx[0] != _T('\0'))
	{
		_tcscat(m_szVersionLineName, _T("_"));
		_tcscat(m_szVersionLineName, m_szVersionEx);
	}

	nResult = true;
Exit0:
	if (pVerFile)
	{
		fclose(pVerFile);
		pVerFile = NULL;
	}
	return nResult;
}

void KConfig::_StringTrim(TCHAR szString[])
{
	TCHAR *pSrc = szString;
	TCHAR *pDes = szString;

	while (pSrc && *pSrc)
	{
		if (
			(*pSrc != _T(' '))  &&
			(*pSrc != _T('\n')) &&
			(*pSrc != _T('\r')) &&
			(*pSrc != _T('\t'))
			)
		{
			*pDes = *pSrc;
			++pDes;
		}
		++pSrc;
	}
	*pDes = _T('\0');
}

void KConfig::_FormatVersion(TCHAR szVersion[], TCHAR cFormat)
{
	TCHAR *pVersion = szVersion;
	while (pVersion && *pVersion)
	{
		if (
			(*pVersion == _T('.')) ||
			(*pVersion == _T('-'))
			)
		{
			*pVersion = cFormat;
		}
		++pVersion;
	}
}

int KConfig::_ReadProfile()
{
	int nRetCode = false;
	int nResult  = false;
	TCHAR szServerFile[MAX_PATH];
	TCHAR szSection[MAX_PATH];
    tstring strDownloadURL;

	_tcsncpy(szServerFile, m_szAppPath, sizeof(szServerFile) / sizeof(TCHAR));
	szServerFile[sizeof(szServerFile) / sizeof(TCHAR) - 1] = _T('\0');

	nRetCode = (int)(sizeof(szServerFile) / sizeof(TCHAR) - _tcslen(szServerFile));
	_tcsncat(szServerFile, CONFIG_FILE_NAME, nRetCode);
	szServerFile[sizeof(szServerFile) / sizeof(TCHAR) - 1] = _T('\0');

	_sntprintf(szSection, sizeof(szSection) / sizeof(TCHAR),
		_T("%s%d"), SECTION_SERVER_LIST_PREFIX, 1);
	szSection[sizeof(szSection) /  sizeof(TCHAR) - 1] = _T('\0');

	nRetCode = GetPrivateProfileString(
		szSection, 
		KEY_IP,
		_T(""), 
		m_UpdateServer.szIP, 
		sizeof(m_UpdateServer.szIP) / sizeof(TCHAR),
		szServerFile
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);

	KGLogPrintf(KGLOG_INFO, "Get UpdateSever Is (%s)\n", (m_UpdateServer.szIP));

	nRetCode = GetPrivateProfileString(
		szSection, 
		KEY_BULLETIN_URL,
		_T(""), 
		m_szBulletinURL, 
		sizeof(m_szBulletinURL) / sizeof(TCHAR),
		szServerFile
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);

    nRetCode = GetPrivateProfileString(
        szSection, _T("URL"), NULL,
        m_szDownloadURL, sizeof(m_szDownloadURL) / sizeof(m_szDownloadURL[0]),
        szServerFile
    );
    if (nRetCode <= 0)
    {
        strDownloadURL = tstring(m_UpdateServer.szIP) + m_szVersionLineName + _T('/');

        _tcsncpy(m_szDownloadURL, strDownloadURL.c_str(), sizeof(m_szDownloadURL) / sizeof(m_szDownloadURL[0]));
        m_szDownloadURL[sizeof(m_szDownloadURL) / sizeof(m_szDownloadURL[0]) - 1] = _T('\0');
    }

	nResult = true;
Exit0:
	return nResult;
}

int KConfig::SaveVersionAndPath()
{
	int nRetCode = false;
	int nResult  = false;

	nRetCode = KRegInfo::WriteCurrentVersion(m_szVersionLineName, m_szCurrVersion);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = KRegInfo::WriteInstallPath(m_szVersionLineName, m_szAppPath);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int KConfig::ReLoadVersion()
{
	return _ReadVersionInfo();
}