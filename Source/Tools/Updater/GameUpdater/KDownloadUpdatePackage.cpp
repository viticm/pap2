#include "StdAfx.h"
#include "KDownloadUpdatePackage.h"
#include "UpdateDefine.h"
#include "KConfig.h"
#include "KFileMD5Maker.h"
#include "IDownloader.h"
#include "IUpdateEvent.h"
#include "KGU_Public.h"

KDownloadUpdatePackage::KDownloadUpdatePackage(void)
{
	m_PackCount     = 0;
	m_nStop         = false;
	m_nDownloadVersionFlag = false;

	memset(m_pZipFileList, 0, sizeof(KZipFileInfo*) * MAX_PATH);
}

KDownloadUpdatePackage::~KDownloadUpdatePackage(void)
{
	m_nDownloadVersionFlag = false;
	CleanUp();
}

int KDownloadUpdatePackage::GetLastVersion(
	const TCHAR cpszRemotePath[], 
	const TCHAR cpszUpdatePath[], 
	const TCHAR cpszClientVersion[], 
	TCHAR *pszLastVersion
)
{
	int nRetCode = false;
	int nResult  = false;
	int nStrLen  = 0;
	TCHAR szVersion[MAX_VERSION_LENGTH];

	KGLOG_PROCESS_ERROR(cpszRemotePath);
	KGLOG_PROCESS_ERROR(cpszUpdatePath);
	KGLOG_PROCESS_ERROR(cpszClientVersion);
	KGLOG_PROCESS_ERROR(pszLastVersion);

	nStrLen = (int)_tcslen(cpszRemotePath);
	_tcsncpy(m_szRemotePath, cpszRemotePath, nStrLen + 1);
	m_szRemotePath[nStrLen] = _T('\0');

	nStrLen = (int)_tcslen(cpszUpdatePath);
	_tcsncpy(m_szUpdatePath, cpszUpdatePath, nStrLen + 1);
	m_szUpdatePath[nStrLen] = _T('\0');

	nStrLen = (int)_tcslen(cpszClientVersion);
	_tcsncpy(m_szClientLang, cpszClientVersion, nStrLen + 1);
	m_szClientLang[nStrLen] = _T('\0');	

	nRetCode = DownLoadVersion();
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = GetPrivateProfileString(
		SECTION_ENTRY_VERSION, KEY_ENTRY_VERSION, _T(""), 
		szVersion, sizeof(szVersion) / sizeof(TCHAR),
		m_strUpdateEntryFile.c_str()
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);

	_tcscpy(pszLastVersion, szVersion);

	m_nDownloadVersionFlag = true;

	nResult = true;
Exit0:
	if (!nResult)
	{
		KLastError::SetErrorCode(UPDATE_ERR_NO_AVAILABLE_UPDATE_PACK);
	}
	
	return nResult;
}


int KDownloadUpdatePackage::DownloadPacks(
								  IDownloaderNotification* pNotify, 
								  const TCHAR cpszRemotePath[], 
								  const TCHAR cpszUpdatePath[], 
								  const TCHAR cpszClientVersion[], 
								  const TCHAR cpszCurrentVersion[]
)
{
	int nRetCode = false;
	int nResult  = false;
	int nStrLen  = false;

	KGLOG_PROCESS_ERROR(pNotify);
	KGLOG_PROCESS_ERROR(cpszRemotePath);
	KGLOG_PROCESS_ERROR(cpszUpdatePath);
	KGLOG_PROCESS_ERROR(cpszClientVersion);
	KGLOG_PROCESS_ERROR(cpszCurrentVersion);

	m_pNotify = pNotify;

	nStrLen = (int)_tcslen(cpszRemotePath);
	_tcsncpy(m_szRemotePath, cpszRemotePath, nStrLen + 1);
	m_szRemotePath[nStrLen] = _T('\0');

	nStrLen = (int)_tcslen(cpszUpdatePath);
	_tcsncpy(m_szUpdatePath, cpszUpdatePath, nStrLen + 1);
	m_szUpdatePath[nStrLen] = _T('\0');

	nStrLen = (int)_tcslen(cpszClientVersion);
	_tcsncpy(m_szClientLang, cpszClientVersion, nStrLen + 1);
	m_szClientLang[nStrLen] = _T('\0');

	nStrLen = (int)_tcslen(cpszCurrentVersion);
	_tcsncpy(m_szCurrentVersion, cpszCurrentVersion, nStrLen + 1);
	m_szCurrentVersion[nStrLen] = _T('\0');

	if (!m_nDownloadVersionFlag)
	{
		nRetCode = DownLoadVersion();
		KGLOG_PROCESS_ERROR(nRetCode);
	}	

	nRetCode = ParseVersion();
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = DownloadUpdatePackage();
	KGLOG_PROCESS_ERROR(nRetCode);	

	nResult = true;
Exit0:
	return nResult;
}

const TCHAR* KDownloadUpdatePackage::GetNextPackName()
{
	static int nIndex = 0;
	const TCHAR* pFileName = NULL;
	static tstring stLocalFile;

	if (nIndex >= m_PackCount)
	{
		nIndex = 0;
		pFileName = NULL;
		goto Exit0;
	}

	stLocalFile = tstring(m_szUpdatePath) + tstring(m_pZipFileList[nIndex]->szZipFileName);
	pFileName = stLocalFile.c_str();

	nIndex++;
Exit0:
	return pFileName;
}

int KDownloadUpdatePackage::DownLoadVersion()
{
	int nRetCode = false;
	int nResult  = false;
	tstring strURL;

    m_strUpdateEntryFile  = m_szUpdatePath;
    m_strUpdateEntryFile += UPDATE_VERSION_FILE;

    strURL  = tstring(m_szRemotePath) + UPDATE_VERSION_FILE;

    nRetCode = KGU_DownloadHttpFile(strURL.c_str(), m_strUpdateEntryFile.c_str());
    KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
    if (!nResult)
    {
        KLastError::SetErrorCode(UPDATE_ERR_UPDATE_FAILED);
    }
	return nResult;
}

int KDownloadUpdatePackage::ParseVersion()
{
	int nRetCode = false;
	int nResult  = false;
	KZipFileInfo** ZipFileList;

	nRetCode = m_VersionParser.Parse(m_strUpdateEntryFile, m_szCurrentVersion);
	KGLOG_PROCESS_ERROR(nRetCode);

	m_PackCount = m_VersionParser.GetPackFileListCount();
	ZipFileList = m_VersionParser.GetPackList();

	for (int i = 0; i < m_PackCount; i++)
	{
		if (m_pZipFileList[i])
		{
			delete m_pZipFileList[i];
			m_pZipFileList[i] = NULL;
		}

		m_pZipFileList[i] = new KZipFileInfo;
		KGLOG_PROCESS_ERROR(m_pZipFileList[i]);
		
		memcpy(m_pZipFileList[i], ZipFileList[i], sizeof(KZipFileInfo));
	}

	nResult = true;
Exit0:
	
	if (!nResult)
	{
		for (int j = 0; j < m_PackCount; j++)
		{
			if (m_pZipFileList[j])
			{
				delete m_pZipFileList[j];
				m_pZipFileList[j] = NULL;
			}
		}
		KLastError::SetErrorCode(UPDATE_ERR_NO_AVAILABLE_UPDATE_PACK);
	}
	
	_tremove(m_strUpdateEntryFile.c_str());
	return nResult;
}

void KDownloadUpdatePackage::CleanUp()
{
	for (int j = 0; j < m_PackCount; j++)
	{
		if (m_pZipFileList[j])
		{
			delete m_pZipFileList[j];
			m_pZipFileList[j] = NULL;
		}
	}
	m_VersionParser.CleanUp();
	m_PackCount     = 0;
}

int KDownloadUpdatePackage::DownloadUpdatePackage()
{
	int nRetCode = false;
	int nResult  = false;
	
	tstring strLocalPath;
	tstring strRemoteFile;

	KG_PROCESS_SUCCESS(m_PackCount == 0);
	m_HttpClient.SetNotification(m_pNotify);

	for (int i = 0; i < m_PackCount; i++)
	{
		strRemoteFile = tstring(m_szRemotePath) + m_pZipFileList[i]->szZipFileName;

		m_HttpClient.AddFile(strRemoteFile.c_str(), m_szUpdatePath);
	}

	while (!m_nStop)
	{
		IHttpFile* pDownloader = m_HttpClient.StartAndWait();
		if (!pDownloader)
			break;

		ProcessDownloadError(pDownloader->FileName(), pDownloader->Error());

		if (pDownloader->Error() != dec_err_success)
		{			
			m_HttpClient.Abort();
			goto Exit0;
		}

		nRetCode = CompareMD5(pDownloader->FileName());
		if (!nRetCode)
		{
			m_HttpClient.Abort();
			KLastError::SetErrorCode(UPDATE_ERR_UPDATE_FAILED);
			KGLogPrintf(KGLOG_ERR, "%s md5 check error!\n", pDownloader->FileName());
			_tremove(pDownloader->FileName());
			goto Exit0;
		}
	}

Exit1:
	nResult = true;
Exit0:
	m_HttpClient.SetNotification(NULL);
	return nResult;
}

int KDownloadUpdatePackage::CompareMD5(const TCHAR* cpszFileName)
{
	int nRetCode = false;
	int nResult  = false;
	int nIsCompare = false;
	TCHAR szTempMd5[MD5_LENGTH];
	int nRootLen = 0;

	KGLOG_PROCESS_ERROR(cpszFileName);
	
	nRootLen = (int)_tcslen(m_szUpdatePath);

	nRetCode = MakdFileMD5(szTempMd5, cpszFileName);
	KGLOG_PROCESS_ERROR(nRetCode);

	for (int i = 0; i < m_PackCount; i++)
	{
		nRetCode = _tcsicmp(cpszFileName + nRootLen, m_pZipFileList[i]->szZipFileName);
		if (nRetCode == 0)
		{
			nRetCode = _tcsicmp(szTempMd5, m_pZipFileList[i]->szZipFileMD5);
			if (nRetCode == 0)
			{
				nIsCompare = true;
			}
			else
			{
				KGLogPrintf(KGLOG_INFO, "Check MD5, Server MD5 = %s, Local MD5 = %s, File = %s",  m_pZipFileList[i]->szZipFileMD5, szTempMd5, m_pZipFileList[i]->szZipFileName);
				goto Exit0;
			}

			break;	
		}
	}

	nResult = nIsCompare;
Exit0:
	return nResult;
}

void KDownloadUpdatePackage::ProcessDownloadError(const TCHAR szFile[], int nError)
{
	KGLogPrintf(KGLOG_INFO, "Download File: %s , errcode = %d", szFile, nError);

	switch (nError)
	{
	case dec_err_success:
		break;
	case dec_err_notstart:
		break;
	case dec_err_stop:
		break;
	case dec_err_url:
	case dec_err_cannotconnect:
		KLastError::SetErrorCode(UPDATE_ERR_CANNOT_CONNECT_SERVER);
		break;
	case dec_err_disconnection:
		KLastError::SetErrorCode(UPDATE_ERR_DISCONNECTION);
		break;
	case dec_err_noenoughdiskspace:
		KLastError::SetErrorCode(UPDATE_ERR_NO_ENOUGH_DISK_SPACE);
		break;
	case dec_err_cannotdownload:
		KLastError::SetErrorCode(UPDATE_ERR_NOT_FOUND_DOWNLOAD_FILE);
		break;
	case dec_err_writefileerror:
		KLastError::SetErrorCode(UPDATE_ERR_WRITE_FILE_ERROR);
		break;
	default:
		break;
	}
}
