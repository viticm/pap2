///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
// FileName : KHttpDownloaderImp.cpp
// Creator  : chengzhaohui
// Date     : 2009-1-4
// Comment  : 多线程单文件下载
//
///////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <WinSock2.h>
#include <errno.h>
#include <process.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <direct.h>
#include "KHttpFile.h"

KHttpFile::KHttpFile()
{
}

KHttpFile::~KHttpFile(void)
{
}

int KHttpFile::InitDownload(
							const TCHAR szFileUrl[],
							const TCHAR szSavePath[], 
							int nResumeFlag /*= true*/, 
							IHttpFileAsyncCallback *pCallback /*= NULL*/, 
							int nThreadCount /*= 1*/)
{
	int nRetCode = false;
	int nResult  = false;

	m_nFile           = -1;

	m_nErrorCode      = dec_err_notstart;

	m_nSupportResume  = false;
	m_nResume         = nResumeFlag;

	m_nFileSize       = 0;
	m_nDownloadedSize = 0;	
	m_pCallback       = pCallback;
	m_nCheckDiskFlag  = true;

	m_strUrl          = szFileUrl;
	m_strSavePath     = szSavePath;
	if (m_strSavePath[m_strSavePath.size() - 1] != _T('\\') &&
		m_strSavePath[m_strSavePath.size() - 1] != _T('/')
	)
	{
		m_strSavePath.append(_T("\\"));
	}

	nRetCode = m_Downloader.InitSocket(2, 2);
	KGLOG_PROCESS_ERROR(nRetCode);

	m_Downloader.SetNotifyWnd(this);
	m_Downloader.SetTimeout(15000, 15000, 20000);

	nResult = true;
Exit0:
	return nResult;
}

void KHttpFile::UnInitDownload()
{
	m_Downloader.UnInitSocket();
}


int KHttpFile::Download()
{
	int nRetCode    = false;
	int nResult     = false;
	int nRetryCount = 0;

	nRetCode = GetFileInfo();
	KGLOG_PROCESS_ERROR(nRetCode);

	while (nRetryCount <= 5)
	{
		if (m_nFileSize == m_nDownloadedSize)
		{
			m_nErrorCode = dec_err_success;
			goto Exit1;
		}

		nRetCode = m_Downloader.Download(m_strUrl.c_str(), m_strFile.c_str());	
		switch(nRetCode)
		{
		case HTTP_RESULT_SUCCESS:
		case HTTP_RESULT_SAMEAS:
			m_nErrorCode = dec_err_success;
			goto Exit1;
			break;

		case HTTP_RESULT_STOP:
			m_nErrorCode = dec_err_stop;
			goto Exit0;
			break;

		case HTTP_RESULT_FAIL:
			m_nErrorCode = dec_err_disconnection;
			KGLogPrintf(KGLOG_INFO, "disconnection retry %d", nRetryCount++);
			Sleep(5000);
			break;

		case HTTP_RESULT_REDIRECT_FTP:
		case HTTP_RESULT_REDIRECT_HTTP:
			m_nErrorCode = dec_err_cannotconnect;
			KGLogPrintf(KGLOG_INFO, "cannotconnect retry %d", nRetryCount++);
			Sleep(5000);
			break;

		default:
			m_nErrorCode = dec_err_cannotconnect;
			KGLOG_PROCESS_ERROR(false && "unknow result");
			break;
		}
	}

Exit1:
	nResult = true;
Exit0:
	KGLogPrintf(KGLOG_INFO, "download result= %d, filename = %s", m_nErrorCode, m_strFile.c_str());
	return nResult;
}

void KHttpFile::Stop()
{
	m_Downloader.StopDownload();
}

int KHttpFile::GetFileInfo()
{
	int                 nRetCode       = false;
	int                 nResult        = false;
	DWORD               dwFileSize     = 0;
	DWORD               dwDownloadSize = 0;
	_tstring::size_type nFindPos       = 0;	

	if (m_nFileSize > 0)
		goto Exit1;

	//分析需要下载的文件名
	nRetCode = m_Downloader.GetDownloadFileStatus(m_strUrl.c_str(), m_strSavePath.c_str(), dwFileSize, dwDownloadSize);
	if (nRetCode && dwFileSize > 0)
	{
		m_nFileSize = dwFileSize;
		m_nDownloadedSize = dwDownloadSize;
	}

	m_strFile.clear();
	nFindPos = m_strUrl.rfind(_T("/"));
	if (nFindPos == _tstring::npos || nFindPos == m_strUrl.size() - 1)
	{
		m_nErrorCode = dec_err_url;
		goto Exit0;
	}
	m_strFile.append(m_strUrl.begin() + nFindPos + 1, m_strUrl.end());
	m_strFile = m_strSavePath + m_strFile;

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KHttpFile::CheckDiskSpace()
{
	int                           nRetCode        = false;
	int                           nResult         = false;
	TCHAR                         cDisk           = m_strFile[0];
	unsigned __int64              nAvailableSpace = 0;
	struct _diskfree_t DiskFree;

	//磁盘空间是否足够
	if (cDisk >= _T('a') && cDisk <= _T('z'))
		cDisk += 'A' - 'a';
	nRetCode = _getdiskfree(cDisk - _T('A') + 1, &DiskFree);
	KGLOG_PROCESS_ERROR(!nRetCode);
	nAvailableSpace = DiskFree.avail_clusters;
	nAvailableSpace *= DiskFree.sectors_per_cluster;
	nAvailableSpace *= DiskFree.bytes_per_sector;
	KGLOG_PROCESS_ERROR(nAvailableSpace > m_nFileSize - m_nDownloadedSize);

	nResult = true;
Exit0:
	if (!nResult)
	{
		m_nErrorCode = dec_err_noenoughdiskspace;
	}
	return nResult;
}


int KHttpFile::OnProgress(int nFileSize, int nDownloadedSize, int nRecved)
{
	int nRetCode = false;

	m_nFileSize = nFileSize;
	m_nDownloadedSize = nDownloadedSize;

	if (m_nCheckDiskFlag)
	{
		nRetCode = CheckDiskSpace();
		if (!nRetCode)
			m_Downloader.StopDownload();
		m_nCheckDiskFlag = false;
	}

	if (m_pCallback)
	{
		m_pCallback->OnProgress(this, nRecved);
	}

	return true;
}

