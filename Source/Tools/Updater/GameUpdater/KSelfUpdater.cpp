#include "StdAfx.h"

#include "KSelfUpdater.h"
#include "KFileMD5Maker.h"
#include "KFileVersionInfo.h"
#include "KConfig.h"
#include "KVersionNumber.h"
#include "KVersionParser.h"
#include "IDownloader.h"
#include "IUpdateEvent.h"

KSelfUpdater::KSelfUpdater(void)
{
    m_nNeedUpdateFlag = false;
}

KSelfUpdater::~KSelfUpdater(void)
{
	m_SelfFileList.clear();
}

int KSelfUpdater::Start(								 
					  const TCHAR szRemotePath[], 
					  const TCHAR szUpdatePath[], 
					  const TCHAR szClientVersion[],
					  HANDLE hWindows
)
{
    int    nRetCode = FALSE;
    int    nResult  = FALSE;
	int    nStrLen  = 0;
	TCHAR *pSlash   = NULL;
    TCHAR  szAppPath[MAX_PATH];

	KGLOG_PROCESS_ERROR(szRemotePath);
	KGLOG_PROCESS_ERROR(szUpdatePath);
	KGLOG_PROCESS_ERROR(szClientVersion);

	nStrLen = (int)_tcslen(szRemotePath);
	_tcsncpy(m_szRemotePath, szRemotePath, nStrLen + 1);
	m_szRemotePath[nStrLen] = _T('\0');

	nStrLen = (int)_tcslen(szUpdatePath);
	_tcsncpy(m_szUpdatePath, szUpdatePath, nStrLen + 1);
	m_szUpdatePath[nStrLen] = _T('\0');

	nStrLen = (int)_tcslen(szClientVersion);
	_tcsncpy(m_szClientVersion, szClientVersion, nStrLen + 1);
	m_szClientVersion[nStrLen] = _T('\0');

    nRetCode = GetModuleFileName(NULL, szAppPath, sizeof(szAppPath) / sizeof(TCHAR));
    KGLOG_PROCESS_ERROR(nRetCode);

    m_strApp = szAppPath;
    pSlash = _tcsrchr(szAppPath, _T('\\'));
    KGLOG_PROCESS_ERROR(pSlash);
    *(pSlash + 1) = _T('\0');

    m_strPath = tstring(szAppPath);
	KGLOG_PROCESS_ERROR(m_strPath.c_str());

	nRetCode = DownloadVersionFile();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = ParseVersionFile();
    KGLOG_PROCESS_ERROR(nRetCode);

    if (m_nNeedUpdateFlag)
    {
        nRetCode = DownloadUpdateFile();
        KGLOG_PROCESS_ERROR(nRetCode);

		//关闭界面
		if (hWindows != INVALID_HANDLE_VALUE)
			TerminateProcess(hWindows, 0);

		nRetCode = _MoveOldUpdaterToTemp();
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = _CopyNewFileToClientPath();
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = _StartupFlashWindow();
		KGLOG_PROCESS_ERROR(nRetCode);
    }    

    nResult = TRUE;
Exit0:
    return nResult;
}

int KSelfUpdater::Stop()
{
    return TRUE;
}

int KSelfUpdater::DownloadVersionFile()
{
    int     nResult        = FALSE;
    tstring strRemoteFile;

    strRemoteFile = tstring(m_szRemotePath) + m_szClientVersion +
        _T("/") + REMOTE_SELF_UPDATE_FOLDER + _T("/") + REMOTE_SELF_UPDATE_FILE;
    m_strLocalVersionFile = tstring(m_szUpdatePath) + REMOTE_SELF_UPDATE_FILE;
	
    _tremove(m_strLocalVersionFile.c_str());

	m_HttpClient.AddFile(strRemoteFile.c_str(), m_szUpdatePath, false);
	
	while (true)
	{
		IHttpFile* pDownloadData = m_HttpClient.StartAndWait();
		if (!pDownloadData)
			break;
		ProcessDownloadError(pDownloadData->FileName(), pDownloadData->Error());
		if (pDownloadData->Error() != dec_err_success)
		{
			m_HttpClient.Abort();
			goto Exit0;
		}
	}

    nResult = TRUE;
Exit0:
    return nResult;
}
int KSelfUpdater::ParseVersionFile()
{
    int              nRetCode                = false;
    int              nResult                 = false;
    TCHAR            szKey[MAX_PATH];
    TCHAR            szRetmoteVer[MAX_PATH];
	TCHAR            szFileName[MAX_PATH];
	TCHAR*           pszFileVersion = NULL;
	KVersionNumber   VerClient;
	KVersionNumber   VerRemote;
	KFileVersionInfo FileVersion(m_strApp.c_str());	

	pszFileVersion = FileVersion.FileVersion();
	KGLOG_PROCESS_ERROR(pszFileVersion[0] != _T('\0'));

	VerClient = pszFileVersion;

	KGLOG_PROCESS_ERROR(!m_strLocalVersionFile.empty());

	nRetCode = GetPrivateProfileString(
		SECTION_SELF_VERSION, 
		KEY_SELF_VERSION_PREFIX, 
		_T(""), 
		szRetmoteVer,
		sizeof(szRetmoteVer) / sizeof(TCHAR),
		m_strLocalVersionFile.c_str()
		);

	KGLOG_PROCESS_ERROR(nRetCode > 0);
	
	VerRemote = szRetmoteVer;

	//判断版本，决定是否更新
	if (VerRemote > VerClient)
	{
		for (int i = 1; true; ++i)
		{
			_sntprintf(szKey, sizeof(szKey) / sizeof(TCHAR), _T("%s%d"), KEY_SELF_FILE_PREFIX, i);
			szKey[sizeof(szKey) /  sizeof(TCHAR) - 1] = _T('\0');

			nRetCode = GetPrivateProfileString(
				SECTION_SELF_FILE, 
				szKey, 
				_T(""), 
				szFileName,
				sizeof(szFileName) / sizeof(TCHAR),
				m_strLocalVersionFile.c_str()
				);
			if (nRetCode <= 0)
				break;
			
			_tcslwr(szFileName);

			m_SelfFileList.push_back(tstring(szFileName));
		}
	}
	
	m_nNeedUpdateFlag = (m_SelfFileList.size() > 0);  //升级标记

    nResult = true;
Exit0:
    _tremove(m_strLocalVersionFile.c_str());
    m_strLocalVersionFile.clear();

    return nResult;
}

int KSelfUpdater::DownloadUpdateFile()
{
    int                   nRetCode      = FALSE;
    int                   nResult       = FALSE;
    tstring               strRemoteFile;
    tstring               strLocalFile;
    tstring               strUpdateInf;
	STRING_VECT::iterator it;

    for (it = m_SelfFileList.begin(); it != m_SelfFileList.end(); ++it)
    {
        tstring &strFile = *it;
        tstring strDesFile;

        strRemoteFile = tstring(m_szRemotePath) + m_szClientVersion +
            _T("/") + REMOTE_SELF_UPDATE_FOLDER + _T("/") + strFile;
        strLocalFile = tstring(m_szUpdatePath) + strFile;
	
		_tremove(strLocalFile.c_str());
        nRetCode = m_HttpClient.AddFile(strRemoteFile.c_str(), m_szUpdatePath, false);
		while (true)
		{
			IHttpFile* pDownloadData = m_HttpClient.StartAndWait();
			if (!pDownloadData)
				break;
			ProcessDownloadError(pDownloadData->FileName(), pDownloadData->Error());
			if (pDownloadData->Error() != dec_err_success)
			{
				m_HttpClient.Abort();
				goto Exit0;
			}
		}
    }

    nResult = TRUE;
Exit0:
    return nResult;
}

void KSelfUpdater::ProcessDownloadError(const TCHAR szFile[], int nError)
{
	KGLogPrintf(KGLOG_ERR, "Download File: %s , errcode = %d", szFile, nError);

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

int KSelfUpdater::_MoveOldUpdaterToTemp()
{
	int          nRetCode        = false;
	int          nResult         = false;
	TCHAR        szTemp[MAX_PATH];
	tstring      szOldAppName;
	const TCHAR* pSlash = NULL;

	STRING_VECT::iterator it;

	pSlash = _tcsrchr(m_strApp.c_str(), _T('\\'));
	KGLOG_PROCESS_ERROR(pSlash);
	pSlash++;

	szOldAppName = tstring(m_szUpdatePath);// + pSlash;

	for (it = m_SelfFileList.begin(); it != m_SelfFileList.end(); ++it)
	{		
		for (int i = 0; i < INT_MAX; ++i)
		{
			_itoa(i, szTemp, 10);

			szOldAppName = tstring(m_szUpdatePath);
			szOldAppName += *it + tstring(szTemp);

			nRetCode = _tremove(szOldAppName.c_str());
			nRetCode = _trename(m_strApp.c_str(), szOldAppName.c_str());
			if (nRetCode == 0)
				break;
		}
	}

	nResult = true;
Exit0:
	return nResult;
}

int KSelfUpdater::_CopyNewFileToClientPath()
{
	int     nResult         = false;
	int     nRetCode        = false;
	DWORD   dwFileAttribute = 0;
	tstring strUpdateFile;
	tstring strNewFile;

	STRING_VECT::iterator it;

	for (it = m_SelfFileList.begin(); it != m_SelfFileList.end(); ++it)
	{
		tstring &strFile = *it;
		strUpdateFile    = tstring(m_szUpdatePath) + strFile;
		strNewFile       = m_strPath + strFile;

		//去掉目标文件的属性，防止删除失败
		dwFileAttribute = GetFileAttributes(strNewFile.c_str());
		dwFileAttribute &= ~dwFileAttribute;
		SetFileAttributes(strNewFile.c_str(), dwFileAttribute);

		nRetCode = _tremove(strNewFile.c_str());
		nRetCode = _trename(strUpdateFile.c_str(), strNewFile.c_str());
		KGLOG_PROCESS_ERROR(nRetCode == 0);
	}

	nResult = true;
Exit0:
	return nResult;
}

int KSelfUpdater::_StartupFlashWindow()
{
	int                 nRetCode   = false;
	int                 nResult    = false;
	STARTUPINFO         StartInfo;
	PROCESS_INFORMATION ProcessInfo;

	ZeroMemory(&StartInfo, sizeof(StartInfo));
	StartInfo.cb = sizeof(StartInfo);

	ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));

	nRetCode = CreateProcess(
		NULL,
		(LPSTR)((tstring("\"") + m_strApp + tstring("\"") + " " + tstring(FLASH_WINDOWS_FLAG)).c_str()), 
		NULL,
		NULL,
		FALSE,
		NORMAL_PRIORITY_CLASS,
		NULL,
		NULL, 
		&StartInfo, 
		&ProcessInfo
		);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}
