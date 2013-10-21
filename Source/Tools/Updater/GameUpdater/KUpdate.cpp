#include "StdAfx.h"
#include "KUpdate.h"
#include "KSystemCheck.h"

KUpdate::KUpdate(void)
{
	m_pEvent          = NULL;
	m_nUpdating       = false;
	m_nStop           = false;
	m_nIsManualUpdate = false;

    m_hSelfUpdateEvent[0] = NULL;
    m_hSelfUpdateEvent[1] = NULL;
}

KUpdate::~KUpdate(void)
{
}

int KUpdate::GetLastVersion(
	const TCHAR szUrl[], 
	const TCHAR szUpdatePath[],
	const TCHAR szLang[], 
	IUpdateEvent* pEvent,
    HANDLE hSuccessSelfUpdateEvent,
    HANDLE hFailedSelfUpdateEvent
							)
{
	int nRetCode = false;
	int nResult  = false; 

	KGLOG_PROCESS_ERROR(szUrl); 
	KGLOG_PROCESS_ERROR(szUpdatePath);
	KGLOG_PROCESS_ERROR(szLang);
	KGLOG_PROCESS_ERROR(pEvent);
    KGLOG_PROCESS_ERROR(
        (hSuccessSelfUpdateEvent && hFailedSelfUpdateEvent) ||
        (hSuccessSelfUpdateEvent == NULL && hFailedSelfUpdateEvent == NULL)
    );

	_tcsncpy(m_szUrl, szUrl, sizeof(m_szUrl) / sizeof(TCHAR));
	m_szUrl[sizeof(m_szUrl) / sizeof(TCHAR) - 1] = _T('\0');

	_tcsncpy(m_szUpdatePath, szUpdatePath, sizeof(m_szUpdatePath) / sizeof(TCHAR));
	m_szUpdatePath[sizeof(m_szUpdatePath) / sizeof(TCHAR) - 1] = _T('\0');

	_tcsncpy(m_szLang, szLang, sizeof(m_szLang) / sizeof(TCHAR));
	m_szLang[sizeof(m_szLang) / sizeof(TCHAR) - 1] = _T('\0');

	m_pEvent = pEvent;

    m_hSelfUpdateEvent[0] = hSuccessSelfUpdateEvent;
    m_hSelfUpdateEvent[1] = hFailedSelfUpdateEvent;

	nRetCode = m_GetLastVerionThread.Create(GetVersionThread, (void *)this);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}


int KUpdate::AutoUpdate(
	const TCHAR szUrl[], 
	const TCHAR szCurrentVersion[], 
	const TCHAR szLang[],
	const TCHAR szAppPath[], 
	IUpdateEvent* pEvent
)
{
	int nRetCode = false;
	int nResult  = false;

	m_nIsManualUpdate = false;
	
	KGLOG_PROCESS_ERROR(szCurrentVersion);
	KGLOG_PROCESS_ERROR(szUrl);
	KGLOG_PROCESS_ERROR(szLang);
	KGLOG_PROCESS_ERROR(szAppPath);
	KGLOG_PROCESS_ERROR(pEvent);

	_tcsncpy(m_szUrl, szUrl, sizeof(m_szUrl) / sizeof(TCHAR));
	m_szUrl[sizeof(m_szUrl) / sizeof(TCHAR) - 1] = _T('\0');

	_tcsncpy(m_szCurrVersion, szCurrentVersion, sizeof(m_szCurrVersion) / sizeof(TCHAR));
	m_szCurrVersion[sizeof(m_szCurrVersion) / sizeof(TCHAR) - 1] = _T('\0');

	_tcsncpy(m_szLang, szLang, sizeof(m_szLang) / sizeof(TCHAR));
	m_szLang[sizeof(m_szLang) / sizeof(TCHAR) - 1] = _T('\0');

	_tcsncpy(m_szAppPath, szAppPath, sizeof(m_szAppPath) / sizeof(TCHAR));
	m_szAppPath[sizeof(m_szAppPath) / sizeof(TCHAR) - 1] = _T('\0');

	_sntprintf(m_szUpdatePath, MAX_PATH, _T("%supdate\\"), m_szAppPath);
	m_szUpdatePath[MAX_PATH - 1] = _T('\0');

	m_pEvent = pEvent;

	m_Notify.SetNotfiy(m_pEvent);

	nRetCode = RunUpdate();
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int KUpdate::ManualUpdate(
			 const TCHAR szPackName[], 
			 const TCHAR szCurrentVersion[],
			 const TCHAR szAppPath[],
			 IUpdateEvent* pEvent
)
{
	int nRetCode = false;
	int nResult  = false;
	
	KGLOG_PROCESS_ERROR(szPackName);
	KGLOG_PROCESS_ERROR(szCurrentVersion);
	KGLOG_PROCESS_ERROR(szAppPath);
	KGLOG_PROCESS_ERROR(pEvent);

	m_nIsManualUpdate = true;

	_tcsncpy(m_szAppPath, szAppPath, sizeof(m_szAppPath) / sizeof(TCHAR));
	m_szAppPath[sizeof(m_szAppPath) / sizeof(TCHAR) - 1] = _T('\0');

	_tcsncpy(m_szCurrVersion, szCurrentVersion, sizeof(m_szCurrVersion) / sizeof(TCHAR));
	m_szCurrVersion[sizeof(m_szCurrVersion) / sizeof(TCHAR) - 1] = _T('\0');

	_tcsncpy(m_szManualPackName, szPackName, sizeof(m_szManualPackName) / sizeof(TCHAR));
	m_szUrl[sizeof(m_szManualPackName) / sizeof(TCHAR) - 1] = _T('\0');

	m_pEvent = pEvent;

	nRetCode = RunUpdate();
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int KUpdate::Abort()
{
	m_nStop = true;

	m_DownloadPack.Abort();
	m_GetLastVerionThread.Destroy();
	m_Thread.Destroy();

	m_pEvent = NULL;

	return true;
}

int KUpdate::RunUpdate()
{
	return m_Thread.Create(UpdateWorkThreadFunction, (void *)this);
}

void KUpdate::UpdateWorkThreadFunction(void *pParam)
{
	if (pParam)
	{
		KUpdate* pThis = (KUpdate *)pParam;
		pThis->UpdateThreadFunction();
	}
}

void KUpdate::GetVersionThread(void *pParam)
{
	if (pParam)
	{
		KUpdate* pThis = (KUpdate *)pParam;
		pThis->GetLastVersionWorkFunction();
	}
}

void KUpdate::GetLastVersionWorkFunction()
{
	int   nRetCode           = false;
	TCHAR szVersion[MAX_PATH];

    nRetCode = _WaitForSelfUpdate();
    KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_DownloadPack.GetLastVersion(m_szUrl, m_szUpdatePath, m_szLang, szVersion);
	KGLOG_PROCESS_ERROR(nRetCode);

	if (m_pEvent)
	{
		nRetCode = m_pEvent->OnLastVersion(szVersion);
		KGLOG_PROCESS_ERROR(nRetCode);
	}
Exit0:
	if (m_pEvent)
		m_pEvent->OnError((UPDATE_FLASH_SHOW_ERROR_MESSAGES)KLastError::GetErrorCode());

	return;
}

void KUpdate::UpdateThreadFunction()
{
	int nRetCode = false;

	if (!m_nIsManualUpdate)
	{
		nRetCode = _AutoUpdate();
        KGLOG_PROCESS_ERROR(nRetCode && "_AutoUpdate()");
	}
	else
	{
		nRetCode = _ManualUpdate();
        KGLOG_PROCESS_ERROR(nRetCode && "_ManualUpdate()");
	}

Exit0:
	m_pEvent->OnError((UPDATE_FLASH_SHOW_ERROR_MESSAGES)KLastError::GetErrorCode());
}

int KUpdate::_AutoUpdate()
{
	int nRetCode = false;
	int nResult  = false;

	//下载更新包
	m_pEvent->OnStateChange(UPDATE_STATUS_DOWNLOADING_PACK);

 	nRetCode = m_DownloadPack.DownloadPacks(&m_Notify, m_szUrl, m_szUpdatePath, m_szLang, m_szCurrVersion);
 	KGLOG_PROCESS_ERROR(nRetCode);
    
	if (!m_DownloadPack.NeedUpdate())
	{
		m_pEvent->OnStateChange(UPDATE_STATUS_NO_NEW_VERSION);
		goto Exit1;
	}
	
	//更新客户端
	m_pEvent->OnStateChange(UPDATE_STATUS_CLIENT_UPGRADING);

	while (true)
	{
		if (m_nStop)
			goto Exit1;

		const TCHAR *pFileName = m_DownloadPack.GetNextPackName();
		if (!pFileName)
			break;
		nRetCode = _ExtractUpdatePackage(pFileName);
		KGLOG_PROCESS_ERROR(nRetCode);
	}

	m_pEvent->OnStateChange(UPDATE_STATUS_CLIENT_UPGRADE_COMPLETE);

Exit1:
	nResult =  true;
Exit0:
	if (nResult)
	{
		_Cleanup(m_szUpdatePath);
	}
    return nResult;
}

int KUpdate::_ManualUpdate()
{
	int            nRetCode = false;
	int            nResult  = false; 
	KVersionParser Parser;

	nRetCode = Parser.CanUpdate(m_szManualPackName, m_szCurrVersion);
	if (nRetCode == 0)
	{
		m_pEvent->OnStateChange(UPDATE_STATUS_NO_NEW_VERSION);
		goto Exit0;
	}
	else if (nRetCode == -1)
	{
		KLastError::SetErrorCode(UPDATE_ERR_MANUAL_PACK_NOT_MATCH);
		goto Exit0;
	}
	else
	{
		m_pEvent->OnStateChange(UPDATE_STATUS_CLIENT_UPGRADING);

		nRetCode = _ExtractUpdatePackage(m_szManualPackName);
		KGLOG_PROCESS_ERROR(nRetCode);

		m_pEvent->OnStateChange(UPDATE_STATUS_CLIENT_UPGRADE_COMPLETE);
	}

	nResult = true;
Exit0:
	return nResult;
}

int KUpdate::_ExtractUpdatePackage(const TCHAR szPack[])
{
	int                 nRetCode = false;
	int                 nResult  = false;
	EXTRACT_PACK_RESULT eRetcode = EXTRACT_FAILD;
	KExtractUpdatePack  Extract;

	nRetCode = Extract.Init(szPack, m_szAppPath);
	KGLOG_PROCESS_ERROR(nRetCode);

	while (true)
	{
		eRetcode = Extract.ExtractNext();
		if (eRetcode == EXTRACT_FINISH)
			break;
		KGLOG_PROCESS_ERROR(eRetcode == EXTRACT_OK);
	}

	nResult = true;
Exit0:	
	if (!nResult)
		KLastError::SetErrorCode(UPDATE_ERR_UPDATE_FAILED);

	Extract.UnInit();
	return nResult;
}

int KUpdate::_Cleanup(const TCHAR szUpdatePath[])
{
	int             nRetCode      = false;
	HANDLE          hFindFile     = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA FindFileData;
	tstring         strPathName;
	tstring         strFile;
	tstring         strFullPathFile;

	strPathName = tstring(szUpdatePath) + _T("*.*");

	hFindFile = ::FindFirstFile(strPathName.c_str(), &FindFileData);
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		while (true)
		{
			strFile = FindFileData.cFileName;
			if (
				(strFile != _T(".")) &&
				(strFile != _T(".."))
			)
			{
				strFullPathFile = tstring(szUpdatePath) + strFile;
				if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					_tremove(strFullPathFile.c_str());
				}
				else
				{
					strFullPathFile += _T("\\");
					_Cleanup(strFullPathFile.c_str());
				}
			}

			nRetCode = ::FindNextFile(hFindFile, &FindFileData);
			if (!nRetCode)
			{
				if (GetLastError() == ERROR_NO_MORE_FILES)
					break;
			}
		}
	}

	::FindClose(hFindFile);
	hFindFile = INVALID_HANDLE_VALUE;

	return true;
}

int KUpdate::_WaitForSelfUpdate()
{
	int nResult  = false;
    DWORD dwRetCode = 0;

	m_pEvent->OnStateChange(UPDATE_STATUS_SELF_UPGRADING);

    KG_PROCESS_SUCCESS(m_hSelfUpdateEvent[0] == NULL);
    KG_PROCESS_SUCCESS(m_hSelfUpdateEvent[1] == NULL);

    dwRetCode = WaitForMultipleObjects(2, m_hSelfUpdateEvent, false, INFINITE);
    KGLOG_PROCESS_ERROR(dwRetCode == WAIT_OBJECT_0);

Exit1:
    nResult = true;
Exit0:
    if (!nResult)   // Indicate self update failed.
        KLastError::SetErrorCode(UPDATE_ERR_SELF_UPDATE_FAILED);
	return nResult;
}

void KUpdate::KNotification::SetNotfiy(IUpdateEvent* pUpdate)
{
	assert(pUpdate);
	m_pControl     = pUpdate;
	m_fLastPrecent = 0.0f;
}

void KUpdate::KNotification::OnProgress(IDownloader *pDownloader, IHttpFile *pHttpFile, int nRecvdSize)
{
	float fDownloadPrecent = 0.0f;

	KGLOG_PROCESS_ERROR(pDownloader);
	KGLOG_PROCESS_ERROR(pHttpFile);

	fDownloadPrecent = ((float)(pDownloader->GetTotalDownloadedSize() * 100)) / ((float)pDownloader->GetTotalFileSize());
	
	if (fDownloadPrecent > m_fLastPrecent)
	{
		m_fLastPrecent = fDownloadPrecent;
		m_pControl->OnDownlodProgress(m_fLastPrecent);
	}

Exit0:
	return;
}